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

#include <cassert>
#include <vector>

#include "fltl/include/CFG.hpp"

namespace grail { namespace cfg {

    namespace detail {

        template <typename T>
        inline bool insert(std::vector<bool> *vec, T term) throw() {
            std::vector<bool>::reference bit(vec->operator[](term.number()));
            if(!bit) {
                bit = true;
                return true;
            } else {
                return false;
            }
        }

        bool union_into(std::vector<bool> *dest, std::vector<bool> *sub) throw() {
            if(0 == dest || 0 == sub) {
                return false;
            }

            assert(dest->size() == sub->size());

            bool updated(false);

            for(unsigned i(0), len(dest->size()); i < len; ++i) {
                std::vector<bool>::reference ith(dest->operator[](i));
                if(sub->operator[](i) && !ith) {
                    ith = true;
                    updated = true;
                }
            }

            return updated;
        }

    }

    /// compute the first sets for a CFG.
    template <typename AlphaT>
    void compute_first_set(
        const fltl::CFG<AlphaT> &cfg,
        const std::vector<bool> &nullable,
        std::vector<std::vector<bool> *> &first
    ) throw() {

        typedef typename fltl::CFG<AlphaT>::variable_type variable_type;
        typedef typename fltl::CFG<AlphaT>::production_type production_type;
        typedef typename fltl::CFG<AlphaT>::terminal_type terminal_type;
        typedef typename fltl::CFG<AlphaT>::generator_type generator_type;
        typedef typename fltl::CFG<AlphaT>::symbol_string_type symbol_string_type;

        typedef std::vector<bool> terminal_set_type;

        first.reserve(cfg.num_variables_capacity() + 2);

        // allocate the sets
        variable_type V;
        generator_type variables(cfg.search(~V));
        for(; variables.match_next(); ) {
            first[V.number()] = new std::vector<bool>(
                cfg.num_terminals() + 2, false
            );
        }

        // base case, all productions with a terminal as the first symbol on
        // their RHS
        terminal_type T;
        generator_type first_subsets(cfg.search((~V) --->* ~T + cfg.__));
        bool updated(false);
        for(; first_subsets.match_next(); ) {
            detail::insert(first[V.number()], T);
            updated = true;
        }

        // induction step. note: base case won't necessarily yield anything
        // if every production has a form like "A -> alpha t beta" where
        // alpha is a sequence of one or more nullable variables and t is a
        // terminal.
        variable_type A;
        variable_type reached;
        production_type prod;
        generator_type next_subsets(cfg.search(~prod, V --->* ~A + cfg.__));
        terminal_set_type *curr_set(0);
        terminal_set_type *reached_set(0);

        updated = true;
        for(unsigned old_len(0); updated; ) {
            updated = false;

            for(variables.rewind(); variables.match_next(); ) {

                curr_set = first[V.number()];
                old_len = curr_set->size();

                for(next_subsets.rewind(); next_subsets.match_next(); ) {

                    for(unsigned i(0), len(prod.symbols().length());
                        i < len;
                        ++i) {

                        // found a terminal, add it in; can't move past it
                        if(prod.symbol_at(i).is_terminal()) {

                            updated = detail::insert(
                                curr_set,
                                terminal_type(prod.symbol_at(i))
                            ) || updated;

                            break;

                        // found a variable, union in, try to move past
                        } else {
                            reached = prod.symbol_at(i);
                            reached_set = first[reached.number()];

                            assert(0 != reached_set);

                            updated = detail::union_into(
                                curr_set,
                                reached_set
                            ) || updated;

                            // can't move past
                            if(!nullable[reached.number()]) {
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

}}

#endif /* FLTL_COMPUTE_FIRST_SET_HPP_ */
