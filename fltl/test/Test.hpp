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

#include "fltl/include/preprocessor/COLOR.hpp"

#define _FLTL_QUOTE(x) #x

#define FLTL_TEST_CATEGORY(func,message) \
    void func (void) throw(); \
    static fltl::test::detail::TestCase<& func > _test_ ## func ( \
        _FLTL_QUOTE( func ), \
        message \
    )
#define _FLTL_TEST_MAKE_TEST(cond, message) \
    if(cond) { \
        ++(fltl::test::detail::TestBase::num_passed); \
        printf("    " FLTL_F_GREEN #cond " " message FLTL_F_DEF "\n"); \
    } else { \
        ++(fltl::test::detail::TestBase::num_failed); \
        printf("    " FLTL_F_RED FLTL_F_BOLD #cond " " message FLTL_F_DEF "\n"); \
    } \
    ++(fltl::test::detail::TestBase::num_tests)

/// document a change in state to give context to some tests that depend on
/// the state change
#define FLTL_TEST_DOC(expr) \
    printf("    " FLTL_F_BLUE #expr FLTL_F_DEF "\n"); \
    expr

#define FLTL_TEST_ASSERT_FALSE(cond) \
    _FLTL_TEST_MAKE_TEST(!(cond), "")

#define FLTL_TEST_ASSERT_TRUE(cond) \
    _FLTL_TEST_MAKE_TEST(cond, "")

#define FLTL_TEST_ASSERT(cond, message) \
    _FLTL_TEST_MAKE_TEST(cond, ": " message)

#define FLTL_TEST_EQUAL_REL(lhs,rhs) \
    FLTL_TEST_ASSERT_TRUE(((lhs) == (rhs))); \
    FLTL_TEST_ASSERT_TRUE(!((lhs) != (rhs)))

#define FLTL_TEST_NOT_EQUAL_REL(lhs,rhs) \
    FLTL_TEST_ASSERT_TRUE(((lhs) != (rhs))); \
    FLTL_TEST_ASSERT_TRUE(!((lhs) == (rhs)))

#define FLTL_TEST_EQUAL(lhs,rhs) \
    FLTL_TEST_ASSERT_TRUE(((lhs) == (rhs)))

#define FLTL_TEST_NOT_EQUAL(lhs,rhs) \
    FLTL_TEST_ASSERT_TRUE(((lhs) != (rhs)))

namespace fltl { namespace test {

    namespace detail {

        class TestBase {
        public:

            typedef void (test_func_type)(void);

            TestBase *next;

            static TestBase *first_test;
            static TestBase *last_test;

            static unsigned num_tests;
            static unsigned num_passed;
            static unsigned num_failed;

            TestBase(TestBase *) throw();
            TestBase(TestBase &) throw();
            TestBase &operator=(TestBase &) throw();

            virtual void run_test(void) const throw() = 0;

            virtual ~TestBase(void) throw();
        };

        /// a simple test case
        template <TestBase::test_func_type *TEST_FUNC>
        class TestCase : public detail::TestBase {
        public:

            static TestCase<TEST_FUNC> self;

            static const char *func_name;
            static const char *message;
            static bool added_already;

            TestCase<TEST_FUNC> *get_this(void) throw() {
                return this;
            }

            /// constructor that gets called at runtime when the tests are
            /// run
            TestCase(const char *_func_name, const char *_message) throw()
                : TestBase(get_this())
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
                : TestBase(get_this())
            { }

            /// destructor
            virtual ~TestCase(void) throw() { }

            /// run this category of tests
            virtual void run_test(void) const throw() {
                printf(
                    "\n" FLTL_F_UNDER "%s" FLTL_F_DEF ": %s\n",
                    func_name,
                    message
                );
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
