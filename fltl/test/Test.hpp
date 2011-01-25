/*
 * Test.hpp
 *
 *  Created on: Jan 24, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_TEST_HPP_
#define FLTL_TEST_HPP_

#include <cstdio>

#define _FLTL_QUOTE(x) #x

#define FLTL_TEST(func,message) \
    void func (void) throw(); \
    static fltl::test::detail::TestCase<& func > _test_ ## func ( \
        _FLTL_QUOTE( func ), \
        message \
    )

#define FLTL_TEST_ASSERT(cond, message) \
    if(cond) { \
        printf("    PASSED(" #cond "): " message "\n"); \
    } else { \
        printf("    FAILED(" #cond "): " message "\n"); \
    }

namespace fltl { namespace test {

    namespace detail {

        class TestBase {
        public:

            typedef void (test_func_type)(void);

            TestBase *next;

            static TestBase *first_test;
            static TestBase *last_test;

            TestBase(void) throw();

            virtual void run_test(void) const throw() = 0;
        };

        /// a simple test case
        template <TestBase::test_func_type *TEST_FUNC>
        class TestCase : public detail::TestBase {
        public:

            static TestCase<TEST_FUNC> self;

            static const char *func_name;
            static const char *message;
            static bool added_already;

            /// constructor that gets called at runtime when the tests are
            /// run
            TestCase(const char *_func_name, const char *_message) throw()
                : TestBase()
            {
                if(!added_already) {

                    func_name = _func_name;
                    message = _message;
                    added_already = true;

                    // initialize
                    TestCase<TEST_FUNC>::self.next = 0;

                    if(0 == TestBase::first_test) {
                        TestBase::first_test = &(TestCase<TEST_FUNC>::self);
                        TestBase::last_test = TestBase::first_test;
                    } else {
                        TestBase::last_test->next = &(TestCase<TEST_FUNC>::self);
                        TestBase::last_test = &(TestCase<TEST_FUNC>::self);
                    }
                }
            }

            /// constructor that gets called before any tests are called
            TestCase(void) throw()
                : TestBase()
            { }

            virtual void run_test(void) const throw() {
                printf("CATEGORY(%s): %s\n", func_name, message);
                TEST_FUNC();
            }
        };

        template <TestBase::test_func_type *TEST_FUNC>
        const char *TestCase<TEST_FUNC>::func_name("<missing function name>");

        template <TestBase::test_func_type *TEST_FUNC>
        const char *TestCase<TEST_FUNC>::message("<missing message>");

        template <TestBase::test_func_type *TEST_FUNC>
        TestCase<TEST_FUNC> TestCase<TEST_FUNC>::self;

        template <TestBase::test_func_type *TEST_FUNC>
        bool TestCase<TEST_FUNC>::added_already(false);
    }

    /// run all tests
    void run_tests(void) throw();
}}

#endif /* FLTL_TEST_HPP_ */
