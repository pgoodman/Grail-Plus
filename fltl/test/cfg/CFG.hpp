/*
 * CFG.hpp
 *
 *  Created on: Jan 24, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
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
