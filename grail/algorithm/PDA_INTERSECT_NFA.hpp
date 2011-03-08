/*
 * PDA_INTERSECT_NFA.hpp
 *
 *  Created on: Mar 7, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_PDA_INTERSECT_NFA_HPP_
#define FLTL_PDA_INTERSECT_NFA_HPP_

#include "fltl/include/NFA.hpp"
#include "fltl/include/PDA.hpp"

#include "grail/algorithm/NFA_REMOVE_EPSILON.hpp"

namespace grail { namespace algorithm {

    /// intersect a PDA and an NFA
    template <typename AlphaT>
    class PDA_INTERSECT_NFA {
    public:

        typedef fltl::PDA<AlphaT> PDA;
        typedef fltl::NFA<AlphaT> NFA;

        static void run(const PDA &pda, NFA &nfa, PDA &out) throw() {

            NFA_REMOVE_EPSILON<AlphaT>::run(nfa);

            (void) pda;
            (void) nfa;
            (void) out;
        }

    };

}}

#endif /* FLTL_PDA_INTERSECT_NFA_HPP_ */
