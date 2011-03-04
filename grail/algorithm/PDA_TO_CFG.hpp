/*
 * PDA_TO_CFG.hpp
 *
 *  Created on: Mar 3, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_PDA_TO_CFG_HPP_
#define FLTL_PDA_TO_CFG_HPP_

#include "fltl/include/CFG.hpp"
#include "fltl/include/PDA.hpp"

namespace grail { namespace algorithm {

    /// convert a context-free grammar into a pushdown automaton
    template <typename AlphaT>
    class PDA_TO_CFG {
    public:

        static void run(
            fltl::PDA<AlphaT> &pda,
            fltl::CFG<AlphaT> &cfg
        ) throw() {
            pda.search(pda._,pda._,pda._,pda._,pda._);

            (void) pda;
            (void) cfg;
        }
    };
}}

#endif /* FLTL_PDA_TO_CFG_HPP_ */
