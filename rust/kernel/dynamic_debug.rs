// SPDX-License-Identifier: GPL-2.0

// Copyright (C) 2026 Thomas Weißschuh <linux@weissschuh.net>

#![doc(hidden)]

//! Dynamic debug support for printk.
//!
//! C header: [`include/linux/dynamic_debug.h`](srctree/include/linux/dynamic_debug.h)

use crate::num::casts;
use crate::num::Bounded;
use crate::ptr::project;
use crate::static_assert;
use crate::str::{as_char_ptr_in_const_context, CStr};
use crate::sync::atomic;
use core::marker::PhantomData;
use core::mem::{align_of, size_of};
use pin_init::zeroed;

mod flags {
    use crate::num::casts::u32_into_u8;

    pub(super) const NONE: u8 = u32_into_u8::<{ bindings::_DPRINTK_FLAGS_NONE }>();
    pub(super) const PRINT: u8 = u32_into_u8::<{ bindings::_DPRINTK_FLAGS_PRINT }>();
    pub(super) const INCL_STACK: u8 = u32_into_u8::<{ bindings::_DPRINTK_FLAGS_INCL_STACK }>();
}

pub type LineNumber = Bounded<u32, 18>;
pub type ClassId = Bounded<u32, 6>;

static_assert!(ClassId::USABLE_BITS == bindings::CLS_BITS);
static_assert!(
    LineNumber::USABLE_BITS + ClassId::USABLE_BITS
        == casts::usize_into_u32::<{ size_of::<BitfieldBindgenUnit>() }>() * u8::BITS
);

pub const DEFAULT_CLASS: ClassId = ClassId::new::<{ bindings::_DPRINTK_CLASS_DFLT }>();

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

#[repr(transparent)]
pub struct Descriptor<'a>(bindings::_ddebug, PhantomData<&'a ()>);

static_assert!(size_of::<Descriptor<'_>>() == size_of::<bindings::_ddebug>());
static_assert!(align_of::<Descriptor<'_>>() == 8);

// SAFETY: Only the flags field is changed at runtime using atomic operations.
unsafe impl<'a> Sync for Descriptor<'a> {}

impl<'a> Descriptor<'a> {
    pub const fn new(
        ddebug_default_enabled: bool,
        modname: &'a CStr,
        filename: &'a CStr,
        format: &'a CStr,
        line_number: LineNumber,
        class_id: ClassId,
    ) -> Self {
        let flags = if ddebug_default_enabled {
            flags::NONE | flags::PRINT
        } else {
            flags::NONE
        };

        Self(
            #[allow(clippy::needless_update)]
            bindings::_ddebug {
                modname: as_char_ptr_in_const_context(modname),
                /* The function can not be retrieved on Rust. */
                function: as_char_ptr_in_const_context(c"__unknown__"),
                filename: as_char_ptr_in_const_context(filename),
                format: as_char_ptr_in_const_context(format),
                _bitfield_1: encode_bitfield(line_number, class_id),
                flags,
                #[cfg(CONFIG_JUMP_LABEL)]
                key: Self::new_static_key(),
                ..zeroed()
            },
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
                        type_: casts::u32_as_usize(bindings::JUMP_TYPE_FALSE),
                    },
                },
            },
        }
    }

    /// # Safety
    ///
    /// The given `desc` must be valid throughout the call of the function.
    #[inline(always)]
    pub unsafe fn branch_and_stack<F: FnOnce()>(desc: *mut Descriptor<'static>, func: F) {
        // TODO: Use static keys.

        let flags_ptr = project!(mut desc, .0.flags);
        // SAFETY: As per the function safety requirement `desc` is valid, and properly aligned,
        // so `flags_ptr` is so, too. Updates from C are atomic.
        let flags = unsafe { atomic::atomic_load(flags_ptr, atomic::ordering::Relaxed) };

        if flags & flags::PRINT != 0 {
            func();

            if flags & flags::INCL_STACK != 0 {
                // SAFETY: Can be called from any context where printk can be used.
                unsafe {
                    bindings::dump_stack();
                }
            }
        }
    }
}

#[macro_export]
#[expect(clippy::crate_in_macro_def)]
macro_rules! dynamic_debug_metadata_cls {
    ($enabled:expr, $cls:expr, $fmt:expr) => {{
        use $crate::dynamic_debug::Descriptor;

        #[link_section = "__dyndbg"]
        #[used(compiler)]
        static mut descriptor: Descriptor = Descriptor::new(
            enabled,
            crate::__LOG_PREFIX,
            kernel::c_str!(file!()),
            kernel::c_str!($fmt),
            $crate::dynamic_debug::LineNumber::new::<{ line!() }>(),
            $cls,
        );

        core::ptr::addr_of_mut!(descriptor)
    }};
}

#[macro_export]
macro_rules! dynamic_func_call_cls {
    ($cls:expr, $fmt:expr, $func:expr) => {{
        const enabled: bool = cfg!(debug_assertions);

        let descriptor = $crate::dynamic_debug_metadata_cls!(enabled, $cls, $fmt);

        // SAFETY: `descriptor` has static lifetime, so it is always valid.
        unsafe {
            $crate::dynamic_debug::Descriptor::branch_and_stack(descriptor, $func);
        }
    }};
}

#[macro_export]
macro_rules! dynamic_func_call {
    ($fmt:expr, $func:expr) => {{
        $crate::dynamic_func_call_cls!($crate::dynamic_debug::DEFAULT_CLASS, $fmt, $func);
    }};
}

#[macro_export]
macro_rules! __format_string {
    ($fmt:literal) => {
        $fmt
    };

    ($fmt:literal, $($arg:tt)*) => {
        $fmt
    };
}

#[macro_export]
macro_rules! dynamic_pr_debug {
    ($($arg:tt)+) => {{
        $crate::dynamic_func_call!($crate::__format_string!($($arg)+), || {
            $crate::print_macro!($crate::print::format_strings::DEBUG, false, $($arg)+);
        });
    }};
}

#[macro_export]
macro_rules! dynamic_dev_debug {
    ($dev:expr, $($arg:tt)+) => {{
        match (&$dev, $crate::prelude::fmt!($($arg)+)) {
            (dev, args) => {
                $crate::dynamic_func_call!($crate::__format_string!($($arg)+), || {
                    unsafe { $crate::dev_printk!($crate::bindings::KERN_DEBUG, dev, args); }
                });
            }
        }
    }};
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
