/*
 * CFG_REMOVE_USELESS.hpp
 *
 *  Created on: Mar 10, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_CFG_REMOVE_USELESS_HPP_
#define FLTL_CFG_REMOVE_USELESS_HPP_

namespace grail { namespace algorithm {

    /// remove all non-generating variables
    template <typename AlphaT>
    class CFG_REMOVE_USELESS {
    public:

        typedef fltl::CFG<AlphaT> CFG;
        typedef typename CFG::variable_type variable_type;
        typedef typename CFG::generator_type generator_type;

        static void run(CFG &cfg) throw() {
            variable_type V;
            generator_type variables(cfg.search(~V));

            for(; variables.match_next(); ) {
                if(0 == cfg.num_productions(V)) {
                    cfg.remove_variable(V);
                }
            }
        }
    };
}}

#endif /* FLTL_CFG_REMOVE_USELESS_HPP_ */
