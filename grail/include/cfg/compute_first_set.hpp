/*
 * compute_first_set.hpp
 *
 *  Created on: Mar 9, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
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

        bool union_into(std::vector<bool> *dest, const std::vector<bool> *sub) throw() {
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

        FLTL_CFG_USE_TYPES(fltl::CFG<AlphaT>);

        typedef std::vector<bool> terminal_set_type;

        first.assign(cfg.num_variables_capacity() + 2, 0);

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
