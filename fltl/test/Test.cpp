/*
 * Test.cpp
 *
 *  Created on: Jan 24, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_TEST_CPP_
#define FLTL_TEST_CPP_

#include "fltl/test/Test.hpp"

namespace fltl { namespace test {

    namespace detail {

        // this is evil; it is usually set *before* it is constructed
        TestBase::TestBase(void) throw()
            : next(this->next)
        { }

        TestBase::TestBase(TestBase &that) throw()
            : next(that.next)
        { }

        TestBase &TestBase::operator=(TestBase &that) throw() {
            next = that.next;
            return *this;
        }

        TestBase::~TestBase(void) throw() { }

        /// the first and last tests to run
        TestBase *TestBase::first_test(0);
        TestBase *TestBase::last_test(0);

        unsigned TestBase::num_tests(0U);
        unsigned TestBase::num_passed(0U);
        unsigned TestBase::num_failed(0U);
    }

    /// run all tests
    void run_tests(void) throw() {

        printf("\nTesting...\n");

        for(detail::TestBase *curr(detail::TestBase::first_test);
            0 != curr;
            curr = curr->next) {
            curr->run_test();
        }

        printf("\n");
        printf("Number of tests: %u\n", detail::TestBase::num_tests);
        printf("         passed: %u\n", detail::TestBase::num_passed);
        printf("         failed: %u\n\n", detail::TestBase::num_failed);
    }

}}

#endif /* FLTL_TEST_CPP_ */
