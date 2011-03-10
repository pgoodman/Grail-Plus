/*
 * CFG_REMOVE_DIRECT_LOOPS.hpp
 *
 *  Created on: Mar 6, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_CFG_REMOVE_DIRECT_LOOPS_HPP_
#define FLTL_CFG_REMOVE_DIRECT_LOOPS_HPP_

#include "fltl/include/CFG.hpp"

namespace grail { namespace algorithm {

    /// remove all direct self-loops from a CFG.
    template <typename AlphaT>
    class CFG_REMOVE_DIRECT_LOOPS {
    public:

        // take off the templates!
        typedef fltl::CFG<AlphaT> CFG;
        typedef typename CFG::variable_type variable_type;
        typedef typename CFG::terminal_type terminal_type;
        typedef typename CFG::production_type production_type;
        typedef typename CFG::generator_type generator_type;
        typedef typename CFG::symbol_string_type symbol_string_type;
        typedef typename CFG::production_builder_type production_builder_type;

        static void run(CFG &cfg) throw() {
            variable_type A;
            production_type P;

            // remove self-loops
            generator_type self_loops(cfg.search(~P, (~A) --->* A));
            for(; self_loops.match_next(); ) {
                cfg.remove_production(P);
            }
        }
    };
}}

#endif /* FLTL_CFG_REMOVE_DIRECT_LOOPS_HPP_ */
