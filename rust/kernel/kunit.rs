// SPDX-License-Identifier: GPL-2.0

//! KUnit-based macros for Rust unit tests.
//!
//! C header: [`include/kunit/test.h`](srctree/include/kunit/test.h)
//!
//! Reference: <https://docs.kernel.org/dev-tools/kunit/index.html>

use crate::fmt;
use crate::prelude::*;
use crate::str;
use core::marker::PhantomData;

/// Prints a KUnit error-level message.
///
/// Public but hidden since it should only be used from KUnit generated code.
#[doc(hidden)]
pub fn err(args: fmt::Arguments<'_>) {
    // `args` is unused if `CONFIG_PRINTK` is not set - this avoids a build-time warning.
    #[cfg(not(CONFIG_PRINTK))]
    let _ = args;

    // SAFETY: The format string is null-terminated and the `%pA` specifier matches the argument we
    // are passing.
    #[cfg(CONFIG_PRINTK)]
    unsafe {
        bindings::_printk(
            c"\x013%pA".as_char_ptr(),
            core::ptr::from_ref(&args).cast::<c_void>(),
        );
    }
}

/// Prints a KUnit info-level message.
///
/// Public but hidden since it should only be used from KUnit generated code.
#[doc(hidden)]
pub fn info(args: fmt::Arguments<'_>) {
    // `args` is unused if `CONFIG_PRINTK` is not set - this avoids a build-time warning.
    #[cfg(not(CONFIG_PRINTK))]
    let _ = args;

    // SAFETY: The format string is null-terminated and the `%pA` specifier matches the argument we
    // are passing.
    #[cfg(CONFIG_PRINTK)]
    unsafe {
        bindings::_printk(
            c"\x016%pA".as_char_ptr(),
            core::ptr::from_ref(&args).cast::<c_void>(),
        );
    }
}

/// FIXME
pub struct Location<'a>(bindings::kunit_loc, PhantomData<&'a ()>);

impl<'a> Location<'a> {
    /// FIXME
    pub const fn new(file: &'a str::CStr, line: i32) -> Self {
        Self(
            bindings::kunit_loc {
                file: str::as_char_ptr_in_const_context(file),
                line,
            },
            PhantomData,
        )
    }
}

impl AsRef<bindings::kunit_loc> for Location<'_> {
    fn as_ref(&self) -> &bindings::kunit_loc {
        &self.0
    }
}

/// FIXME
pub fn do_test(
    name: &str,
    condition: &str::CStr,
    file: &str::CStr,
    line: i32,
    passed: bool,
    type_: KUnitRustAssertType,
) {
    // Do nothing if the test passed.
    if passed {
        return;
    }

    // SAFETY: FFI call without safety requirements.
    let kunit_test = unsafe { bindings::kunit_get_current_test() };
    if kunit_test.is_null() {
        // The assertion failed but this task is not running a KUnit test, so we cannot call
        // KUnit, but at least print an error to the kernel log. This may happen if this
        // macro is called from an spawned thread in a test (see
        // `scripts/rustdoc_test_gen.rs`) or if some non-test code calls this macro by
        // mistake (it is hidden to prevent that).
        //
        // This mimics KUnit's failed assertion format.
        err(fmt!("    # {}: ASSERTION FAILED at {file}:{line}\n", name));
        err(fmt!("    Expected {condition} to be true, but is false\n"));
        err(fmt!(
            "    Failure not reported to KUnit since this is a non-KUnit task\n"
        ));
        return;
    }

    let location = Location::new(file, line);
    let assertion = KUnitRustAssert {
        assert: bindings::kunit_assert {},
        condition,
        type_,
    };

    // SAFETY:
    //   - FFI call.
    //   - The `kunit_test` pointer is valid because we got it from
    //     `kunit_get_current_test()` and it was not null. This means we are in a KUnit
    //     test, and that the pointer can be passed to KUnit functions and assertions.
    //   - The string pointers (`file` and `condition` above) point to null-terminated
    //     strings since they are `CStr`s.
    //   - The function pointer (`format`) points to the proper function.
    //   - The assertion pointer points to a valid assertion.
    //   - The pointers passed will remain valid since they point to `static`s.
    //   - The format string is allowed to be null.
    //   - There are, however, problems with this: first of all, this will end up stopping
    //     the thread, without running destructors. While that is problematic in itself,
    //     it is considered UB to have what is effectively a forced foreign unwind
    //     with `extern "C"` ABI. One could observe the stack that is now gone from
    //     another thread. We should avoid pinning stack variables to prevent library UB,
    //     too. For the moment, given that test failures are reported immediately before the
    //     next test runs, that test failures should be fixed and that KUnit is explicitly
    //     documented as not suitable for production environments, we feel it is reasonable.
    unsafe {
        bindings::__kunit_do_failed_assertion(
            kunit_test,
            &location.0,
            bindings::kunit_assert_type_KUNIT_ASSERTION,
            &assertion.assert,
            Some(kunit_rust_assert_format),
            ::core::ptr::null(),
        );
    }

    // SAFETY: FFI call; the `test` pointer is valid because this hidden macro should only
    // be called by the generated documentation tests which forward the test pointer given
    // by KUnit.
    unsafe {
        bindings::__kunit_abort(kunit_test);
    }
}

/// FIXME
pub enum KUnitRustAssertType {
    /// FIXME
    UnaryAssert(bool),
}

///FIXME
pub struct KUnitRustAssert<'a> {
    /// FIXME
    pub assert: bindings::kunit_assert,

    /// FIXME
    pub condition: &'a str::CStr,

    /// FIXME
    pub type_: KUnitRustAssertType,
}

/// FIXME
/// # Safety
///
/// FIXME
unsafe extern "C" fn kunit_rust_assert_format(
    assert: *const bindings::kunit_assert,
    message: *const bindings::va_format,
    stream: *mut bindings::string_stream,
) {
    let assert = unsafe {
        // SAFETY: FIXME
        crate::container_of!(assert, KUnitRustAssert<'_>, assert)
    };
    let assert = unsafe {
        // SAFETY: FIXME
        assert.as_ref_unchecked()
    };
    let stream = unsafe {
        // SAFETY: FIXME
        stream.as_mut_unchecked()
    };
    let message = unsafe {
        // SAFETY: FIXME
        message.as_ref_unchecked()
    };

    match assert.type_ {
        KUnitRustAssertType::UnaryAssert(expected_true) => {
            let unary_assert = bindings::kunit_unary_assert {
                assert: assert.assert,
                condition: assert.condition.as_char_ptr(),
                expected_true,
            };

            unsafe {
                // SAFETY: FIXME
                bindings::kunit_unary_assert_format(&unary_assert.assert, message, stream);
            }
        }
    }
}

/// Asserts that a boolean expression is `true` at runtime.
///
/// Public but hidden since it should only be used from generated tests.
///
/// Unlike the one in `core`, this one does not panic; instead, it is mapped to the KUnit
/// facilities. See [`assert!`] for more details.
#[doc(hidden)]
#[macro_export]
macro_rules! kunit_assert {
    ($name:literal, $condition:expr $(,)?) => {{
        let passed: bool = $condition;

        // Use `file!()` instead of `::core::file!()` here so it can be overridden.
        const FILE: &'static $crate::str::CStr = $crate::c_str!(file!());
        // Use `line!()` instead of `::core::line!()` here so it can be overridden.
        const LINE: i32 = line!() as i32;
        const CONDITION: &'static $crate::str::CStr = $crate::c_str!(stringify!($condition));

        $crate::kunit::do_test(
            $name,
            CONDITION,
            FILE,
            LINE,
            passed,
            $crate::kunit::KUnitRustAssertType::UnaryAssert(true),
        );
    }};
}

/// Asserts that two expressions are equal to each other (using [`PartialEq`]).
///
/// Public but hidden since it should only be used from generated tests.
///
/// Unlike the one in `core`, this one does not panic; instead, it is mapped to the KUnit
/// facilities. See [`assert!`] for more details.
#[doc(hidden)]
#[macro_export]
macro_rules! kunit_assert_eq {
    ($name:literal, $left:expr, $right:expr $(,)?) => {{
        // For the moment, we just forward to the expression assert because, for binary asserts,
        // KUnit supports only a few types (e.g. integers).
        $crate::kunit_assert!($name, $left == $right);
    }};
}

#[doc(hidden)]
#[macro_export]
macro_rules! kunit_assert_matches {
    ($name:literal, $left:expr, $right:pat_param $(,)?) => {{
        // For the moment, we just forward to the expression assert because, for binary asserts,
        // KUnit supports only a few types (e.g. integers).
        match $left {
            $right => {}
            _ => {
                $crate::kunit_assert!($name, false);
            }
        };
    }};
}

trait TestResult {
    fn is_test_result_ok(&self) -> bool;
}

impl TestResult for () {
    fn is_test_result_ok(&self) -> bool {
        true
    }
}

impl<T, E> TestResult for Result<T, E> {
    fn is_test_result_ok(&self) -> bool {
        self.is_ok()
    }
}

/// Returns whether a test result is to be considered OK.
///
/// This will be `assert!`ed from the generated tests.
#[doc(hidden)]
#[expect(private_bounds)]
pub fn is_test_result_ok(t: impl TestResult) -> bool {
    t.is_test_result_ok()
}

/// Represents an individual test case.
#[doc(hidden)]
pub const fn kunit_case(
    name: &'static kernel::str::CStr,
    run_case: unsafe extern "C" fn(*mut kernel::bindings::kunit),
) -> kernel::bindings::kunit_case {
    kernel::bindings::kunit_case {
        run_case: Some(run_case),
        name: kernel::str::as_char_ptr_in_const_context(name),
        attr: kernel::bindings::kunit_attributes {
            speed: kernel::bindings::kunit_speed_KUNIT_SPEED_NORMAL,
        },
        generate_params: None,
        status: kernel::bindings::kunit_status_KUNIT_SUCCESS,
        module_name: core::ptr::null_mut(),
        log: core::ptr::null_mut(),
        param_init: None,
        param_exit: None,
    }
}

/// Registers a KUnit test suite.
///
/// # Safety
///
/// `test_cases` must be a `NULL` terminated array of valid test cases,
/// whose lifetime is at least that of the test suite (i.e., static).
///
/// # Examples
///
/// ```ignore
/// extern "C" fn test_fn(_test: *mut kernel::bindings::kunit) {
///     let actual = 1 + 1;
///     let expected = 2;
///     assert_eq!(actual, expected);
/// }
///
/// static mut KUNIT_TEST_CASES: [kernel::bindings::kunit_case; 2] = [
///     kernel::kunit::kunit_case(c"name", test_fn),
///     pin_init::zeroed(),
/// ];
/// kernel::kunit_unsafe_test_suite!(suite_name, KUNIT_TEST_CASES);
/// ```
#[doc(hidden)]
#[macro_export]
macro_rules! kunit_unsafe_test_suite {
    ($name:ident, $test_cases:ident) => {
        const _: () = {
            const KUNIT_TEST_SUITE_NAME: [::kernel::ffi::c_char; 256] = {
                let name_u8 = ::core::stringify!($name).as_bytes();
                let mut ret = [0; 256];

                if name_u8.len() > 255 {
                    panic!(concat!(
                        "The test suite name `",
                        ::core::stringify!($name),
                        "` exceeds the maximum length of 255 bytes."
                    ));
                }

                let mut i = 0;
                while i < name_u8.len() {
                    ret[i] = name_u8[i] as ::kernel::ffi::c_char;
                    i += 1;
                }

                ret
            };

            static mut KUNIT_TEST_SUITE: ::kernel::bindings::kunit_suite =
                ::kernel::bindings::kunit_suite {
                    name: KUNIT_TEST_SUITE_NAME,
                    #[allow(unused_unsafe)]
                    // SAFETY: `$test_cases` is passed in by the user, and
                    // (as documented) must be valid for the lifetime of
                    // the suite (i.e., static).
                    test_cases: unsafe {
                        ::core::ptr::addr_of_mut!($test_cases)
                            .cast::<::kernel::bindings::kunit_case>()
                    },
                    suite_init: None,
                    suite_exit: None,
                    init: None,
                    exit: None,
                    attr: ::kernel::bindings::kunit_attributes {
                        speed: ::kernel::bindings::kunit_speed_KUNIT_SPEED_NORMAL,
                    },
                    status_comment: [0; 256usize],
                    debugfs: ::core::ptr::null_mut(),
                    log: ::core::ptr::null_mut(),
                    suite_init_err: 0,
                    is_init: false,
                    status: kernel::bindings::kunit_status_KUNIT_SUCCESS,
                };

            #[used(compiler)]
            #[allow(unused_unsafe)]
            #[cfg_attr(not(target_os = "macos"), link_section = ".kunit_test_suites")]
            static mut KUNIT_TEST_SUITE_ENTRY: *const ::kernel::bindings::kunit_suite =
                // SAFETY: `KUNIT_TEST_SUITE` is static.
                unsafe { ::core::ptr::addr_of_mut!(KUNIT_TEST_SUITE) };
        };
    };
}

/// Returns whether we are currently running a KUnit test.
///
/// In some cases, you need to call test-only code from outside the test case, for example, to
/// create a function mock. This function allows to change behavior depending on whether we are
/// currently running a KUnit test or not.
///
/// # Examples
///
/// This example shows how a function can be mocked to return a well-known value while testing:
///
/// ```
/// # use kernel::kunit::in_kunit_test;
/// fn fn_mock_example(n: i32) -> i32 {
///     if in_kunit_test() {
///         return 100;
///     }
///
///     n + 1
/// }
///
/// let mock_res = fn_mock_example(5);
/// assert_eq!(mock_res, 100);
/// ```
pub fn in_kunit_test() -> bool {
    // SAFETY: `kunit_get_current_test()` is always safe to call (it has fallbacks for
    // when KUnit is not enabled).
    !unsafe { bindings::kunit_get_current_test() }.is_null()
}

#[cfg(CONFIG_RUST_KUNIT_SELFTEST)]
#[kunit_tests(rust_kernel_kunit)]
mod tests {
    use super::*;

    #[test]
    fn rust_test_kunit_example_test() {
        assert_eq!(1 + 1, 2);
    }

    #[test]
    fn rust_test_kunit_in_kunit_test() {
        assert!(in_kunit_test());
    }

    #[test]
    #[cfg(not(all()))]
    fn rust_test_kunit_always_disabled_test() {
        // This test should never run because of the `cfg`.
        assert!(false);
    }

    #[test]
    fn foo() {
        assert_eq!(1, 2);
        assert_matches!(Some(2), Some(_));
    }
}
