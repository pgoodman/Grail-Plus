/*
 * Test.cpp
 *
 *  Created on: Jan 24, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef FLTL_TEST_CPP_
#define FLTL_TEST_CPP_

#include "fltl/test/Test.hpp"

namespace fltl { namespace test {

    namespace detail {

        // this is evil; it is usually set *before* it is constructed
        TestBase::TestBase(TestBase *self) throw()
            : next(self->next)
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
