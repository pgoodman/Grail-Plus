/*
 * CFG.hpp
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

#ifndef FLTL_CFG_HPP_
#define FLTL_CFG_HPP_


#include "fltl/test/Test.hpp"
#include "fltl/include/CFG.hpp"

namespace fltl { namespace test { namespace cfg {

    FLTL_TEST_CATEGORY(test_equivalence_relations,
        "Test for equivalence of variables, terminals, symbols, and symbol strings."
    );

    FLTL_TEST_CATEGORY(test_string_lengths,
        "Test the length of symbols and symbol strings."
    );

    FLTL_TEST_CATEGORY(test_add_productions,
        "Test that productions are correctly added to the grammar and that duplicates are ignored."
    );

    FLTL_TEST_CATEGORY(test_remove_productions,
        "Test that productions are correctly removed from the grammar."
    );

    FLTL_TEST_CATEGORY(test_extract_symbols,
        "Test that symbols and symbol strings can be extracted from productions and symbol strings."
    );

    FLTL_TEST_CATEGORY(test_pattern_match,
        "Test the pattern-matching feature for productions."
    );

    FLTL_TEST_CATEGORY(test_generate_terminals,
        "Test that generators give the right results for terminals."
    );

    FLTL_TEST_CATEGORY(test_generate_variables,
        "Test that generators give the right results for variables."
    );

    FLTL_TEST_CATEGORY(test_generate_productions,
        "Test that generators give the right results for productions."
    );

    FLTL_TEST_CATEGORY(test_generate_search,
        "Test that generators give the right results for searches."
    );
}}}

#endif /* FLTL_CFG_HPP_ */
