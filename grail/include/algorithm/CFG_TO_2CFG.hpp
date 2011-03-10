/*
 * CFG_TO_2CFG.hpp
 *
 *  Created on: Mar 5, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_CFG_TO_2CFG_HPP_
#define FLTL_CFG_TO_2CFG_HPP_


#include <cassert>
#include <set>
#include <map>
#include <utility>

#include "fltl/include/CFG.hpp"

#include "grail/include/algorithm/CFG_REMOVE_UNITS.hpp"
#include "grail/include/algorithm/CFG_REMOVE_EPSILON.hpp"

namespace grail { namespace algorithm {

    /// convert the productions of a CFG to be either unit productions or
    /// tuples
    template <typename AlphaT>
    class CFG_TO_2CFG {

        // take off the templates!
        typedef fltl::CFG<AlphaT> CFG;
        typedef typename CFG::variable_type variable_type;
        typedef typename CFG::production_type production_type;
        typedef typename CFG::generator_type generator_type;
        typedef typename CFG::symbol_string_type symbol_string_type;

    public:

        /// go look for all productions with three or more symbols on their RHS
        /// and break them into pairs of productions
        static void run(CFG &cfg) throw() {

            variable_type A;
            symbol_string_type str;
            production_type P;

            generator_type long_rules(cfg.search(
                ~P,
                (~A) --->* cfg._ + cfg._ + cfg._ + cfg.__
            ));

            for(; long_rules.match_next(); ) {

                cfg.remove_production(P);

                str = P.symbols();

                unsigned i(str.length() - 2);
                variable_type prev_new_var(cfg.add_variable());
                cfg.add_production(prev_new_var, str.substring(i, 2));

                for(--i; i > 0; --i) {
                    variable_type new_var(cfg.add_variable());
                    cfg.add_production(new_var, str.at(i) + prev_new_var);
                    prev_new_var = new_var;
                }

                cfg.add_production(A, str.at(0) + prev_new_var);
            }
        }
    };

}}

#endif /* FLTL_CFG_TO_2CFG_HPP_ */
