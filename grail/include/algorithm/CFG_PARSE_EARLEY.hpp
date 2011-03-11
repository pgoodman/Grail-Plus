/*
 * CFG_EARLEY_PARSE.hpp
 *
 *  Created on: Mar 10, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_CFG_EARLEY_PARSE_HPP_
#define FLTL_CFG_EARLEY_PARSE_HPP_

#include <set>
#include <vector>

#include "fltl/include/CFG.hpp"

#include "fltl/include/helper/Array.hpp"
#include "fltl/include/helper/BlockAllocator.hpp"

#include "fltl/include/mpl/Static.hpp"

namespace grail { namespace algorithm {

    /// parse a context free grammar using an Earley-style parser
    template <typename AlphaT>
    class CFG_PARSE_EARLEY {
    public:

        // take off the templates!
        typedef fltl::CFG<AlphaT> CFG;
        typedef typename CFG::variable_type variable_type;
        typedef typename CFG::pattern_type pattern_type;
        typedef typename CFG::terminal_type terminal_type;
        typedef typename CFG::production_type production_type;
        typedef typename CFG::generator_type generator_type;
        typedef typename CFG::symbol_string_type symbol_string_type;
        typedef typename CFG::production_builder_type production_builder_type;

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

            earley_set_type(void)
                : first(0)
                , last(0)
                , next(0)
                , prev(0)
            { }

            void push(earley_item_type *item) throw() {
                if(0 == first) {
                    first = item;
                    last = item;
                } else {
                    last->next = item;
                    last = item;
                }

                item->set = this;
                item->next = 0;
            }

            void set_next(earley_set_type *set) throw() {
                set->prev = this;
                set->next = 0;
                next = set;
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

            // the (first) item that caused this item to be generated
            earley_item_type *antecedent;

            // the set to which this item belongs
            earley_set_type *set;

            earley_item_type(void)
                : dot(0)
                , production()
                , next(0)
                , antecedent(0)
                , set(0)
            { }

            void scanned_from(earley_item_type *ant) throw() {
                dot = ant->dot + 1;
                production = ant->production;
                antecedent = ant;
                set = ant->set->next;
            }
        };

    private:

        enum {
            NUM_BLOCKS = 1024U
        };

    public:

        /// run the parser; assumes that the NULLABLE set is properly filled
        /// for this grammar
        static void run(
            CFG &cfg,
            std::vector<bool> &is_nullable,
            fltl::helper::Array<
                std::set<typename fltl::CFG<AlphaT>::terminal_type> *
            > first_terminals
        ) throw() {

            (void) cfg;
            (void) is_nullable;
            (void) first_terminals;

            // allocator for Earley sets
            static fltl::helper::BlockAllocator<
                earley_set_type, NUM_BLOCKS
            > set_allocator;

            /// allocator for Earley items
            static fltl::helper::BlockAllocator<
                earley_item_type, NUM_BLOCKS
            > item_allocator;

            // the actual start variable; we will end up adding a fake
            // start variable later
            const variable_type ASV(cfg.get_start_variable());

            // is it worth parsing?
            if(0 == cfg.num_productions()
            || !cfg.has_start_variable()
            || 0 == cfg.num_productions(ASV)) {
                return;
            }

            // add in a fake start variable; this variable and its productions
            // will be removed at the end

            variable_type SV(cfg.add_variable());
            production_type SP(cfg.add_production(SV, ASV));
            is_nullable[SV.number()] = is_nullable[ASV.number()];

            // set up the base case for the earley parser
            earley_item_type *first_item(item_allocator.allocate());
            earley_set_type *first_set(set_allocator.allocate());

            first_set->next = 0;

            first_item->antecedent = 0;
            first_item->dot = 0;
            first_item->production = SP;

            first_set->push(first_item);

            // variables used in the patterns
            unsigned dot;
            variable_type A;
            variable_type B;
            terminal_type a;
            symbol_string_type alpha;

            pattern_type scanner((~A) --->* cfg.__(dot) + a + cfg.__);
            pattern_type predictor((~A) --->* cfg.__(dot) + ~B + cfg.__);
            pattern_type completer((~A) --->* cfg.__(dot));

            generator_type predictor_related(cfg.search(B --->* ~alpha));
            pattern_type completer_related((~B) --->* cfg.__(dot) + A + cfg.__);

            // for each set
            for(earley_set_type *curr_set(first_set), *next_set(0);
                0 != curr_set;
                curr_set = curr_set->next) {

                // get the terminal

                // for each item
                for(earley_item_type *curr_item(curr_set->first);
                    0 != curr_item;
                    curr_item = curr_item->next) {

                    // move the dot to where the current item is looking so
                    // that the patterns see the right value
                    dot = curr_item->dot;

                    if(scanner.match(curr_item->production)) {

                        next_set = curr_set->next;
                        if(0 == next_set) {
                            next_set = set_allocator.allocate();
                            curr_set->set_next(next_set);
                        }


                    } else if(predictor.match(curr_item->production)) {

                    } else if(completer.match(curr_item->production)) {

                    }
                }
            }

            // done; clean up
            cfg.unsafe_remove_variable(SV);
        }
    };
}}

#endif /* FLTL_CFG_EARLEY_PARSE_HPP_ */
