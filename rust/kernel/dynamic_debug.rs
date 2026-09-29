// SPDX-License-Identifier: GPL-2.0

// Copyright (C) 2026 Thomas Weißschuh <linux@weissschuh.net>

//! Dynamic debug support for printk.
//!
//! C header: [`include/linux/dynamic_debug.h`](srctree/include/linux/dynamic_debug.h)

use crate::{
    num::Bounded,
    prelude::*,
    ptr::project,
    static_assert,
    str::{
        as_char_ptr_in_const_context,
        CStr, //
    },
    sync::atomic,
    types::Opaque,
};
use core::{
    marker::PhantomData,
    mem::{
        align_of,
        size_of, //
    },
};
use pin_init::zeroed;

mod flags {
    use super::cv;

    pub(super) const NONE: u8 = cv!(bindings::_DPRINTK_FLAGS_NONE);
    pub(super) const PRINT: u8 = cv!(bindings::_DPRINTK_FLAGS_PRINT);
    pub(super) const INCL_STACK: u8 = cv!(bindings::_DPRINTK_FLAGS_INCL_STACK);
}

/// Line number type.
///
/// Constrained to ensure that all values can be encoded in a [`Descriptor`].
pub type LineNumber = Bounded<u32, 18>;

/// Class ID type.
///
/// Constrained to ensure that all values can be encoded in a [`Descriptor`].
pub type ClassId = Bounded<u32, 6>;

static_assert!(ClassId::USABLE_BITS == bindings::CLS_BITS);
static_assert!(
    LineNumber::USABLE_BITS + ClassId::USABLE_BITS
        == cv!(size_of::<BitfieldBindgenUnit>() => u32) * u8::BITS
);

type BitfieldBindgenUnit = bindings::__BindgenBitfieldUnit<[u8; 3]>;

const fn encode_bitfield(line_number: LineNumber, class_id: ClassId) -> BitfieldBindgenUnit {
    let line_number = line_number.get();
    let class_id = class_id.get();

    // TODO: Use bindings::_ddebug::new_bitfield_1() when usable in const functions.
    let bytes = if cfg!(target_endian = "big") {
        [
            (line_number >> 10) as u8,
            (line_number >> 2) as u8,
            (line_number as u8) << 6 | class_id as u8,
        ]
    } else {
        [
            line_number as u8,
            (line_number >> 8) as u8,
            (line_number >> 16) as u8 | (class_id as u8) << 2,
        ]
    };

    BitfieldBindgenUnit::new(bytes)
}

/// Wrapper around a `struct _ddebug` descriptor.
#[repr(transparent)]
pub struct Descriptor<'a>(Opaque<bindings::_ddebug>, PhantomData<&'a ()>);

static_assert!(size_of::<Descriptor<'_>>() == size_of::<bindings::_ddebug>());
// Required by the dynamic debug core.
static_assert!(align_of::<Descriptor<'_>>() == 8);

// SAFETY: Only the flags field is changed at runtime using atomic operations.
unsafe impl Sync for Descriptor<'static> {}

impl<'a> Descriptor<'a> {
    /// Create a new descriptor.
    pub const fn new(
        default_enabled: bool,
        modname: &'a CStr,
        filename: &'a CStr,
        function: &'a CStr,
        format: &'a CStr,
        line_number: LineNumber,
        class_id: Option<ClassId>,
    ) -> Self {
        let class_id = match class_id {
            Some(c) => c,
            None => cv!(bindings::_DPRINTK_CLASS_DFLT),
        };

        let flags = if default_enabled {
            flags::NONE | flags::PRINT
        } else {
            flags::NONE
        };

        Self(
            Opaque::new(
                #[allow(clippy::needless_update)]
                bindings::_ddebug {
                    modname: as_char_ptr_in_const_context(modname),
                    function: as_char_ptr_in_const_context(function),
                    filename: as_char_ptr_in_const_context(filename),
                    format: as_char_ptr_in_const_context(format),
                    _bitfield_1: encode_bitfield(line_number, class_id),
                    flags,
                    #[cfg(CONFIG_JUMP_LABEL)]
                    key: Self::new_static_key(),
                    ..zeroed()
                },
            ),
            PhantomData,
        )
    }

    #[cfg(CONFIG_JUMP_LABEL)]
    const fn new_static_key() -> bindings::_ddebug__bindgen_ty_1 {
        /*
         * Always use static_key_false for now. branch_and_stack() below does not use it (yet),
         * and the dynamic debug core doesn't know how it was initialized in any case.
         */
        bindings::_ddebug__bindgen_ty_1 {
            dd_key_false: bindings::static_key_false {
                key: bindings::static_key {
                    enabled: bindings::atomic_t { counter: 0 },
                    __bindgen_anon_1: bindings::static_key__bindgen_ty_1 {
                        type_: cv!(bindings::JUMP_TYPE_FALSE),
                    },
                },
            },
        }
    }

    /// Get a pointer suitable for FFI function calls.
    #[inline]
    pub const fn as_ffi_ptr(&self) -> *mut bindings::_ddebug {
        self.0.get()
    }

    /// Execute `func` if the descriptor is active.
    /// Also print a stacktrace if enabled.
    #[inline(always)]
    pub fn branch_and_stack<F>(&'static self, func: F)
    where
        F: FnOnce(&Self),
    {
        // TODO: Use static keys.

        let flags_ptr = project!(mut self.as_ffi_ptr(), .flags);
        // SAFETY: `self` is valid, so a pointer to its member is so, too.
        // Updates of `flags` from C are atomic.
        let flags = unsafe { atomic::atomic_load(flags_ptr, atomic::ordering::Relaxed) };

        if flags & flags::PRINT != 0 {
            func(self);

            if flags & flags::INCL_STACK != 0 {
                // SAFETY: Can be called from any context where printk can be used.
                unsafe {
                    bindings::dump_stack();
                }
            }
        }
    }
}

/// Allocate a dynamic debug descriptor.
#[macro_export]
#[expect(clippy::crate_in_macro_def)]
macro_rules! dynamic_debug_metadata_cls {
    ($default_enabled:expr, $cls:expr, $fmt:expr) => {{
        use $crate::{
            c_str,
            cv,
            dynamic_debug::Descriptor, //
        };

        #[link_section = "__dyndbg"]
        static descriptor: Descriptor = Descriptor::new(
            $default_enabled,
            crate::__LOG_PREFIX,
            c_str!(file!()),
            // Rust does not provide the current function, approximate with module_path!().
            c_str!(module_path!()),
            c_str!($fmt),
            cv!(line!()),
            $cls,
        );

        &descriptor
    }};
}

/// Add a runtime-configurable function invocation for a specific class.
#[macro_export]
macro_rules! dynamic_func_call_cls {
    ($cls:expr, $fmt:expr, $func:expr) => {{
        const default_enabled: bool = cfg!(debug_assertions);

        let descriptor = $crate::dynamic_debug_metadata_cls!(default_enabled, $cls, $fmt);

        descriptor.branch_and_stack($func);
    }};
}

/// Add a runtime-configurable function invocation.
#[macro_export]
macro_rules! dynamic_func_call {
    ($fmt:expr, $func:expr) => {{
        $crate::dynamic_func_call_cls!(None, $fmt, $func);
    }};
}

#[doc(hidden)]
#[macro_export]
macro_rules! __format_string {
    ($fmt:literal) => {
        $fmt
    };

    ($fmt:literal, $($arg:tt)*) => {
        $fmt
    };
}

#[macros::kunit_tests(rust_dynamic_debug)]
mod tests {
    use super::*;

    #[test]
    fn test_bitfield_encoding() -> Result<(), ()> {
        struct TestCase {
            line_number: u32,
            class_id: u32,
        }

        let test_cases = [
            TestCase {
                line_number: 0b101010101010101010,
                class_id: 0b101010,
            },
            TestCase {
                line_number: 0b111111111111111111,
                class_id: 0b000000,
            },
            TestCase {
                line_number: 0b000000000000000000,
                class_id: 0b111111,
            },
            TestCase {
                line_number: 0b101100111000111100,
                class_id: 0b111111,
            },
        ];

        for case in test_cases {
            let line_number = LineNumber::try_new(case.line_number).ok_or(())?;
            let class_id = ClassId::try_new(case.class_id).ok_or(())?;

            assert_eq!(
                bindings::_ddebug::new_bitfield_1(*line_number, *class_id,),
                encode_bitfield(line_number, class_id),
            );
        }

        Ok(())
    }
}
