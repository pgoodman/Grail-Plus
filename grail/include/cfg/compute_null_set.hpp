/*
 * find_nullable_variables.hpp
 *
 *  Created on: Mar 9, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_FIND_NULLABLE_VARIABLES_HPP_
#define FLTL_FIND_NULLABLE_VARIABLES_HPP_

#include <vector>

#include "fltl/include/CFG.hpp"

namespace grail { namespace cfg {

    /// compute all nullable variables
    template <typename AlphaT>
    void compute_null_set(
        const fltl::CFG<AlphaT> &cfg,
        std::vector<bool> &nullable
    ) throw() {

        typedef typename fltl::CFG<AlphaT>::variable_type variable_type;
        typedef typename fltl::CFG<AlphaT>::generator_type generator_type;
        typedef typename fltl::CFG<AlphaT>::symbol_string_type symbol_string_type;

        variable_type V;
        variable_type Q;

        nullable.assign(cfg.num_variables() + 2, false);

        // base case: directly nullable productions
        generator_type null_prods(cfg.search((~V) --->* cfg.epsilon()));
        for(; null_prods.match_next(); ) {
            nullable[V.number()] = true;
        }

        if(nullable.empty()) {
            return;
        }

        bool found_nullable(true);
        symbol_string_type str;
        variable_type S;
        generator_type productions(cfg.search((~V) --->* ~Q + ~str));
        unsigned i(0);

        // inductive step, build up the set of nullable variables
        // incrementally
        for(; found_nullable; productions.rewind()) {
            found_nullable = false;
            for(; productions.match_next(); ) {

                // either this isn't a nullable production, or we know
                // that this variable is nullable
                if(nullable[V.number()] || !nullable[Q.number()]) {
                    continue;
                }

                for(i = 0; i < str.length(); ++i) {
                    if(str.at(i).is_terminal()) {
                        break;
                    }

                    S = str.at(i);
                    if(!nullable[S.number()]) {
                        break;
                    }
                }

                if(str.length() == i) {
                    nullable[V.number()] = true;
                    found_nullable = true;
                }
            }
        }
    }
}}

#endif /* FLTL_FIND_NULLABLE_VARIABLES_HPP_ */
