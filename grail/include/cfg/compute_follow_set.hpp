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
        const std::vector<std::vector<bool> *> &first,
        std::vector<std::vector<bool> *> &follow
    ) throw() {
        FLTL_CFG_USE_TYPES(fltl::CFG<AlphaT>);

        follow.reserve(first.size());

        // initialize each follow bitset as the empty set of the appropriate
        // size.
        for(size_t i(0); i < follow.size(); ++i) {
            const bool no_follow(false);
            std::vector<bool> *follow_var(new std::vector<bool>(
                first[i]->size(), no_follow
            ));

            follow[i] = follow_var;
        }

        variable_type V;
        symbol_type s;

        generator_type variables(cfg.search(~V));
        generator_type syms_following_V(cfg.search(cfg._ --->* cfg.__ + V + ~s + cfg.__));

        for(; variables.match_next(); ) {

            for(syms_following_V.rewind(); syms_following_V.match_next(); ) {
                if(s.is_variable()) {
                    variable_type U(s);
                    detail::union_into(follow[V.number()], follow[U.number()]);
                } else {
                    terminal_type u(s);
                    (*(follow[V.number()]))[u.number()] = true;
                }
            }
        }
    }

}}


#endif /* Grail_Plus_COMPUTE_FOLLOW_SET_HPP_ */