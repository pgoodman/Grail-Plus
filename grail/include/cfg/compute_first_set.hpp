/*
 * compute_first_set.hpp
 *
 *  Created on: Mar 9, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_COMPUTE_FIRST_SET_HPP_
#define FLTL_COMPUTE_FIRST_SET_HPP_

#include <set>
#include <map>

#include "fltl/include/CFG.hpp"

#include "fltl/include/helper/Array.hpp"

namespace grail { namespace cfg {

    /// compute the first sets for a CFG.
    template <typename AlphaT>
    void compute_first_set(
        const fltl::CFG<AlphaT> &cfg,
        const std::vector<bool> &nullable,
        fltl::helper::Array<
            std::set<typename fltl::CFG<AlphaT>::terminal_type> *
        > &first
    ) throw() {

        typedef typename fltl::CFG<AlphaT>::variable_type variable_type;
        typedef typename fltl::CFG<AlphaT>::terminal_type terminal_type;
        typedef typename fltl::CFG<AlphaT>::generator_type generator_type;
        typedef typename fltl::CFG<AlphaT>::symbol_string_type symbol_string_type;

        typedef std::set<terminal_type> terminal_set_type;

        first.reserve(cfg.num_variables() + 2);

        // allocate the sets
        variable_type V;
        generator_type variables(cfg.search(~V));
        for(; variables.match_next(); ) {
            first[V.number()] = new std::set<terminal_type>;
        }

        // base case, all productions with a terminal as the first symbol on
        // their RHS
        terminal_type T;
        generator_type first_subsets(cfg.search((~V) --->* ~T + cfg.__));
        bool updated(false);
        for(; first_subsets.match_next(); ) {
            first[V.number()]->insert(T);
            updated = true;
        }

        // induction step
        variable_type prefix;
        variable_type reached;
        symbol_string_type suffix;
        generator_type next_subsets(cfg.search(V --->* ~prefix + ~suffix));
        terminal_set_type *curr_set(0);
        terminal_set_type *reached_set(0);

        for(unsigned old_len(0); updated; ) {
            updated = false;

            for(variables.rewind(); variables.match_next(); ) {

                curr_set = first[V.number()];
                old_len = curr_set->size();

                for(next_subsets.rewind(); next_subsets.match_next(); ) {

                    // collect for the prefix
                    if(V != prefix) {
                        reached_set = first[prefix.number()];

                        assert(0 != reached_set);

                        curr_set->insert(
                            reached_set->begin(),
                            reached_set->end()
                        );
                    }

                    // can't move past this variable
                    if(!nullable[prefix.number()]) {
                        break;
                    }

                    for(unsigned i(0); i < suffix.length(); ++i) {

                        // found a terminal, add it in; can't move past it
                        if(suffix.at(i).is_terminal()) {
                            curr_set->insert(terminal_type(suffix.at(i)));
                            break;

                        // found a variable, union in, try to move past
                        } else {
                            reached = suffix.at(i);
                            reached_set = first[reached.number()];

                            assert(0 != reached_set);

                            curr_set->insert(
                                reached_set->begin(),
                                reached_set->end()
                            );

                            // can't move past
                            if(!nullable[reached.number()]) {
                                break;
                            }
                        }
                    }
                }

                // continue iff we grew at least on set
                if(curr_set->size() > old_len) {
                    updated = true;
                }
            }
        }
    }

}}

#endif /* FLTL_COMPUTE_FIRST_SET_HPP_ */
