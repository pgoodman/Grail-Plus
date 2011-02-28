/*
 * CFG_TO_PDA.hpp
 *
 *  Created on: Feb 27, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_CFG_TO_PDA_HPP_
#define FLTL_CFG_TO_PDA_HPP_

#include "fltl/include/CFG.hpp"
#include "fltl/include/PDA.hpp"

namespace grail { namespace algorithm {

    template <typename AlphaT>
    class CFG_TO_PDA {
    public:

        static void run(const fltl::CFG<AlphaT> &cfg, fltl::PDA<AlphaT> &pda) throw() {
            (void) cfg;
            (void) pda;
        }

    };

}}

#endif /* FLTL_CFG_TO_PDA_HPP_ */
