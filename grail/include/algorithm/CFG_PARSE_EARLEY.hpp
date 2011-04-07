/*
 * CFG_EARLEY_PARSE.hpp
 *
 *  Created on: Mar 10, 2011
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

#ifndef FLTL_CFG_EARLEY_PARSE_HPP_
#define FLTL_CFG_EARLEY_PARSE_HPP_

#include <set>
#include <vector>

#include "fltl/include/CFG.hpp"

#include "fltl/include/helper/BlockAllocator.hpp"

#include "grail/include/io/verbose.hpp"
#include "grail/include/io/UTF8FileTokBuffer.hpp"

namespace grail { namespace algorithm {

    /// parse a context free grammar using an Earley-style parser
    template <typename AlphaT, const unsigned MAX_TOK_LENGTH>
    class CFG_PARSE_EARLEY {
    public:

        // take off the templates!
        typedef fltl::CFG<AlphaT> CFG;

        FLTL_CFG_USE_TYPES(CFG);

        class earley_item_type;

        /// Earley set
        class earley_set_type {
        public:
            // first item in the set
            earley_item_type *first;
            earley_item_type *last;

            // next set
            earley_set_type *next;
            earley_set_type *prev;

            // offset into the terminal stream
            unsigned offset;

            earley_set_type(void)
                : first(0)
                , last(0)
                , next(0)
                , prev(0)
                , offset(0)
            { }

            void push(earley_item_type *item) throw() {
                if(0 == item) {
                    return;
                }
                if(0 == first) {
                    first = item;
                    last = item;
                } else {
                    last->next = item;
                    last = item;
                }

                item->next = 0;
            }

            void set_next(earley_set_type *set) throw() {
                set->prev = this;
                set->next = 0;
                next = set;
                set->offset = offset + 1;
            }
        };

        /// Earley item
        class earley_item_type {
        public:
            // dotted production
            unsigned dot;
            production_type production;

            // next item in the set
            earley_item_type *next;

            // the set that first introduced the production being used. that
            // is, sliding the dot changes the set that owns an item, but not
            // the set that initiated an item.
            earley_set_type *initial_set;

            // the next item in the same set that has the same initial
            // set
            earley_item_type *next_with_same_initial_set;

            earley_item_type(void)
                : dot(0)
                , production()
                , next(0)
                , initial_set(0)
                , next_with_same_initial_set(0)
            { }

            void scanned_from(
                earley_item_type *scan
            ) throw() {
                dot = scan->dot + 1U;
                production = scan->production;
                initial_set = scan->initial_set;
            }

            void predicted_from(
                earley_set_type *set,
                const production_type &prod
            ) throw() {
                dot = 0;
                production = prod;
                initial_set = set;
            }
        };

    private:

        enum {
            NUM_BLOCKS = 1024U
        };

        static void
        clear_index(std::vector<earley_item_type *> &index) throw() {
            for(unsigned i(0); i < index.size(); ++i) {
                index[i] = 0;
            }
        }

        /// allocator type for Earley items
        typedef fltl::helper::BlockAllocator<
            earley_item_type, NUM_BLOCKS
        > earley_item_allocator_type;

        /// allocator type for Earley sets
        typedef fltl::helper::BlockAllocator<
            earley_set_type, NUM_BLOCKS
        > earley_set_allocator_type;

        /// check the index for the existence of item, if it's in, return 0,
        /// otherwise return the item and add it to the index
        static earley_item_type *
        indexed_push(
            earley_set_type *set,
            std::vector<earley_item_type *> &index,
            earley_item_allocator_type &allocator,
            earley_item_type *item
        ) throw() {

            const unsigned offset(item->initial_set->offset);
            earley_item_type *prev(0);

            for(earley_item_type *curr(index[offset]);
                0 != curr;
                prev = curr, curr = curr->next_with_same_initial_set) {

                // found an insertion point
                if(item->dot < curr->dot) {
                    item->next_with_same_initial_set = curr;

                    if(0 == prev) {
                        index[offset] = item;
                    } else {
                        prev->next_with_same_initial_set = item;
                    }

                    set->push(item);
                    return item;

                // skip
                } else if(item->dot > curr->dot) {
                    continue;

                // same dot values, check if the productions are equivalent
                } else if(curr->production == item->production) {
                    allocator.deallocate(item);
                    return curr;
                }
            }

            // the set was empty
            if(0 == prev) {
                index[offset] = item;

            // this item is going at the end
            } else {
                prev->next_with_same_initial_set = item;
            }

            set->push(item);
            return item;
        }

    public:


        /// run the parser; assumes that the NULLABLE set is properly filled
        /// for this grammar
        static bool run(
            CFG &cfg,
            std::vector<bool> &is_nullable,
            const bool use_first_set,
            std::vector<std::vector<bool> *> &first_terminals,
            io::UTF8FileTokBuffer<MAX_TOK_LENGTH> &reader
        ) throw() {

            bool parse_result(false);
            const char *token(reader.read());

            // allocator for Earley sets
            static earley_set_allocator_type set_allocator;

            /// allocator for Earley items
            static earley_item_allocator_type item_allocator;

            // the actual start variable; we will end up adding a fake
            // start variable later
            const variable_type ASV(cfg.get_start_variable());

            // is it worth parsing?
            if(0 == cfg.num_productions()
            || !cfg.has_start_variable()
            || 0 == cfg.num_productions(ASV)) {
                if(0 == token || '\0' == *token) {
                    io::verbose("Parsed. Accepted empty language.\n");
                    return true;
                } else {
                    io::verbose("Failed to parse. Language is empty.\n");
                    return false;
                }
            }

            // add in a fake start variable; this variable and its productions
            // will be removed at the end
            variable_type SV(cfg.add_variable());
            production_type SP(cfg.add_production(SV, ASV));
            is_nullable[SV.number()] = is_nullable[ASV.number()];

            if(use_first_set) {
                first_terminals[SV.number()] = new std::vector<bool>(
                    *(first_terminals[ASV.number()])
                );
            }

            // set up the base case for the earley parser
            earley_item_type *curr_item(item_allocator.allocate());
            earley_set_type *curr_set(set_allocator.allocate());
            earley_set_type *prev_set(0);

            earley_item_type *first_item(curr_item);
            earley_set_type *first_set(curr_set);

            curr_set->next = 0;
            curr_item->production = SP;
            curr_item->initial_set = curr_set;
            curr_set->push(first_item);

            // variables used in the patterns
            unsigned dot;
            variable_type A;
            variable_type B;
            terminal_type a;
            production_type prod;

            pattern_type scanner_known((~A) --->* cfg.__(dot) + a + cfg.__);
            pattern_type scanner_unknown((~A) --->* cfg.__(dot) + ~a + cfg.__);
            pattern_type predictor((~A) --->* cfg.__(dot) + ~B + cfg.__);
            pattern_type completer((~A) --->* cfg.__(dot));

            generator_type predictor_related(cfg.search(~prod, B --->* cfg.__));
            pattern_type completer_related(cfg._ --->* cfg.__(dot) + A + cfg.__);

            // terminals
            unsigned i(0);
            alphabet_type lexeme;
            bool solve_for_variable_terminal(false);

            // indexes for the sets to test membership, +1 as there are n+1
            // sets (S_0 ... S_n) for a string of length n.
            std::vector<earley_item_type *> set_index[2];
            set_index[0].assign(1U, static_cast<earley_item_type *>(0));
            set_index[1].assign(1U, static_cast<earley_item_type *>(0));
            unsigned curr_index(0U);

            // for each set
            bool not_at_end(true);
            earley_set_type *next_set(0);
            earley_item_type *next_item(0);

            for(;
                0 != curr_set && not_at_end;
                prev_set = curr_set,
                curr_set = curr_set->next,
                curr_index = 1U - curr_index,
                ++i, token = reader.read()) {

                set_index[curr_index].push_back(0);

                // done the tokens
                if(0 == token || '\0' == *token) {
                    not_at_end = false;
                    io::verbose("    Looking at EOF\n");

                } else {

                    traits_type::unserialize(token, lexeme);

                    // try to get the terminal
                    solve_for_variable_terminal = !cfg.has_terminal(lexeme);
                    if(!solve_for_variable_terminal) {
                        a = cfg.get_terminal(lexeme);

                    // found a token but this grammar has no variable terminals
                    // and so it can't be substituted for anything
                    } else if(0 == cfg.num_variable_terminals()){

                        io::verbose(
                            "    Unrecognized terminal '%s'.\n",
                            token
                        );
                        goto parse_error;
                    }

                    io::verbose("    Looking at '%s'...\n", token);
                }

                // for each item
                curr_item = curr_set->first;
                for(next_item = 0;
                    0 != curr_item;
                    curr_item = curr_item->next) {

                    // move the dot to where the current item is looking so
                    // that the patterns see the right value
                    dot = curr_item->dot;

                    // the item has the form A --> ... * B ...
                    if(predictor.match(curr_item->production)) {

                        // if B is nullable then add A --> ... B * ... to
                        // the item set
                        if(is_nullable[B.number()]) {

                            next_item = item_allocator.allocate();
                            next_item->scanned_from(curr_item);
                            indexed_push(
                                curr_set,
                                set_index[curr_index],
                                item_allocator,
                                next_item
                            );
                        }

                        // if we're using FIRST sets then use them to skip
                        // useless predictions
                        if(use_first_set && not_at_end
                        && !solve_for_variable_terminal
                        && !(first_terminals[B.number()]->operator[](a.number()))) {
                            continue;
                        }

                        // for each B --> alpha, add B --> * alpha to the
                        // item set
                        for(predictor_related.rewind();
                            predictor_related.match_next();) {

                            next_item = item_allocator.allocate();
                            next_item->predicted_from(curr_set, prod);

                            indexed_push(
                                curr_set,
                                set_index[curr_index],
                                item_allocator,
                                next_item
                            );
                        }

                    // the item has the form A --> ... *
                    } else if(completer.match(curr_item->production)) {

                        for(earley_item_type *rel_item(curr_item->initial_set->first);
                            0 != rel_item;
                            rel_item = rel_item->next) {

                            dot = rel_item->dot;
                            if(!completer_related.match(rel_item->production)) {
                                continue;
                            }

                            next_item = item_allocator.allocate();
                            next_item->scanned_from(rel_item);
                            next_item = indexed_push(
                                curr_set,
                                set_index[curr_index],
                                item_allocator,
                                next_item
                            );
                        }

                    // try to "solve" this terminal
                    } else if(not_at_end) {

                        // we don't know the terminal of this lexeme, lets
                        // see if we can substitute a variable terminal
                        if(solve_for_variable_terminal) {

                            // try to match this production as
                            // A --> ... * a ... for some variable terminal
                            // a.
                            if(!scanner_unknown.match(curr_item->production)
                            || !cfg.is_variable_terminal(a)) {
                                continue;
                            }

                            io::verbose(
                                "        Substituting as %s...\n",
                                cfg.get_name(a)
                            );

                        // we know the terminal of this lexeme
                        } else {

                            // try to match this production as
                            // A --> ... * a ... where "a" is the terminal
                            // of the current lexeme.
                            if(!scanner_known.match(curr_item->production)) {
                                continue;
                            }
                        }

                        next_set = curr_set->next;
                        if(0 == next_set) {
                            next_set = set_allocator.allocate();
                            curr_set->set_next(next_set);
                            set_index[1U - curr_index].push_back(0);
                            clear_index(set_index[1U - curr_index]);
                        }

                        next_item = item_allocator.allocate();
                        next_item->scanned_from(curr_item);
                        next_item = indexed_push(
                            next_set,
                            set_index[1U - curr_index],
                            item_allocator,
                            next_item
                        );
                    }
                }
            }

            if(0 != token && '\0' == *token) {

                // handle the case where we accept the empty string
                if(0 == curr_set) {
                    curr_set = prev_set;
                }

                if(0 == curr_set || not_at_end) {
                    goto parse_error;
                }

                // go look for the final production
                for(curr_item = curr_set->first;
                    0 != curr_item;
                    curr_item = curr_item->next) {

                    if(1U == curr_item->dot
                    && SV == curr_item->production.variable()) {
                        io::verbose("Successfully parsed.\n");
                        parse_result = true;
                        goto done;
                    }
                }
            }

        parse_error:

            parse_result = false;
            io::verbose("Failed to parse all input.\n");

        done:

            io::verbose("Cleaning up Earley items/sets...\n");

            next_set = 0;
            for(curr_set = first_set; 0 != curr_set; curr_set = next_set) {
                next_set = curr_set->next;

                for(curr_item = curr_set->first;
                    0 != curr_item;
                    curr_item = next_item) {

                    next_item = curr_item->next;

                    item_allocator.deallocate(curr_item);
                }

                set_allocator.deallocate(curr_set);
            }

            // done; clean up
            cfg.unsafe_remove_variable(SV);

            io::verbose("Done.\n");

            return parse_result;
        }
    };
}}

#endif /* FLTL_CFG_EARLEY_PARSE_HPP_ */
