/*
 * compute_follow_set.hpp
 *
 *  Created on: Jan 25, 2012
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef Grail_Plus_COMPUTE_FOLLOW_SET_HPP_
#define Grail_Plus_COMPUTE_FOLLOW_SET_HPP_

#include <cassert>
#include <vector>

#include "fltl/include/CFG.hpp"
#include "grail/include/cfg/compute_first_set.hpp"

namespace grail { namespace cfg {

    /// compute the follow sets for a CFG.
    template <typename AlphaT>
    void compute_follow_set(
        const fltl::CFG<AlphaT> &cfg,
        const std::vector<bool> &nullable,
        const std::vector<std::vector<bool> *> &first,
        std::vector<std::vector<bool> *> &follow
    ) throw() {
        FLTL_CFG_USE_TYPES(fltl::CFG<AlphaT>);

        follow.assign(cfg.num_variables_capacity() + 2, 0);

        variable_type V;
        variable_type A;
        symbol_string_type s;

        generator_type variables(cfg.search(~V));
        generator_type syms_following_V(cfg.search((~A) --->* cfg.__ + V + ~s));

        // initialize each follow bitset as the empty set of the appropriate
        // size.
        for(; variables.match_next(); ) {
            follow[V.number()] = new std::vector<bool>(cfg.num_terminals() + 2U, false);
        }

        for(bool updated(true); updated; ) {
            updated = false;

            for(variables.rewind(); variables.match_next(); ) {

                for(syms_following_V.rewind(); syms_following_V.match_next(); ) {
                    for(unsigned i(0); i < s.length(); ++i) {
                        if(s.at(i).is_terminal()) {
                            terminal_type u(s.at(i));
                            (*(follow[V.number()]))[u.number()] = true;
                            goto next_production;
                        }

                        variable_type U(s.at(i));

                        if(U == V) {
                            continue;
                        }

                        updated = detail::union_into(follow[V.number()], first[U.number()]) || updated;

                        if(!nullable[U.number()]) {
                            goto next_production;
                        }
                    }

                    // reached the end of the production
                    updated = detail::union_into(follow[V.number()], first[A.number()]) || updated;

                next_production:
                    continue;
                }
            }
        }
    }

}}


#endif /* Grail_Plus_COMPUTE_FOLLOW_SET_HPP_ */
