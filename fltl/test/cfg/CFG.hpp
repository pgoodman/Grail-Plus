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
#include "fltl/lib/CFG.hpp"

namespace fltl { namespace test { namespace cfg {

    FLTL_TEST_CATEGORY(test_equivalence_relations,
        "Test for equivalence of variables, terminals, symbols, and symbol strings."
    );

    FLTL_TEST_CATEGORY(test_string_lengths,
        "Test the length of symbols and symbol strings."
    );

    FLTL_TEST_CATEGORY(test_productions,
        "Test that productions are correctly added to the grammar and that duplicates are ignored."
    );
}}}

#endif /* FLTL_CFG_HPP_ */
