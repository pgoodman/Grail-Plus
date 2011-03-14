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

#include "grail/include/cfg/ParseTree.hpp"

#define D(x)

namespace grail { namespace algorithm {

    /// parse a context free grammar using an Earley-style parser
    template <typename AlphaT>
    class CFG_PARSE_EARLEY {
    public:

        // take off the templates!
        typedef fltl::CFG<AlphaT> CFG;
        typedef typename CFG::alphabet_type alphabet_type;
        typedef typename CFG::symbol_type symbol_type;
        typedef typename CFG::traits_type traits_type;
        typedef typename CFG::variable_type variable_type;
        typedef typename CFG::pattern_type pattern_type;
        typedef typename CFG::terminal_type terminal_type;
        typedef typename CFG::production_type production_type;
        typedef typename CFG::generator_type generator_type;
        typedef typename CFG::symbol_string_type symbol_string_type;
        typedef typename CFG::production_builder_type production_builder_type;

        typedef cfg::ParseTree<AlphaT> parse_tree_type;

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
            unsigned short dot;
            production_type production;

            // next item in the set
            earley_item_type *next;

            // the (first) item that caused this item to be generated
            earley_item_type *caused_by;
            earley_item_type *scanned_by;

            // the set that first introduced the production being used. that
            // is, sliding the dot changes the set that owns an item, but not
            // the set that initiated an item.
            earley_set_type *initial_set;

            // the next item in the same set that has the same initial
            // set
            earley_item_type *next_with_same_initial_set;

            // has this item been scanned?
            bool was_scanned;

            earley_item_type(void)
                : dot(0)
                , production()
                , next(0)
                , caused_by(0)
                , scanned_by(0)
                , initial_set(0)
                , next_with_same_initial_set(0)
                , was_scanned(false)
            { }

            void scanned_from(
                earley_item_type *scan,
                earley_item_type *ant
            ) throw() {
                dot = scan->dot + 1U;
                production = scan->production;
                scanned_by = scan;
                caused_by = ant;
                initial_set = scan->initial_set;
            }

            void predicted_from(
                earley_item_type *ant,
                earley_set_type *set,
                const production_type &prod
            ) throw() {
                dot = 0;
                production = prod;
                scanned_by = ant;
                caused_by = 0;
                initial_set = set;
                ant->was_scanned = true;
            }
        };

    private:

        enum {
            NUM_BLOCKS = 1024U
        };

        static void
        clear_index(fltl::helper::Array<earley_item_type *> &index) throw() {
            for(unsigned i(0); i < index.size(); ++i) {
                index.set(i, 0);
            }
        }

        /// check the index for the existence of item, if it's in, return 0,
        /// otherwise return the item and add it to the index
        static earley_item_type *
        indexed(
            fltl::helper::Array<earley_item_type *> &index,
            fltl::helper::BlockAllocator<
                earley_item_type, NUM_BLOCKS
            > &allocator,
            earley_item_type *item
        ) throw() {

            const unsigned offset(item->initial_set->offset);
            earley_item_type *&items(index.get(offset));
            earley_item_type *prev(0);

            for(earley_item_type *curr(items);
                0 != curr;
                prev = curr, curr = curr->next_with_same_initial_set) {

                // found an insertion point
                if(item->dot < curr->dot) {
                    item->next_with_same_initial_set = curr;

                    if(0 == prev) {
                        items = item;
                    } else {
                        prev->next_with_same_initial_set = item;
                    }

                    return item;

                // skip
                } else if(item->dot > curr->dot) {
                    continue;

                // same dot values, check if the productions are equivalent
                } else if(curr->production == item->production) {
                    allocator.deallocate(item);
                    return 0;
                }
            }

            // the set was empty
            if(0 == prev) {
                items = item;

            // this item is going at the end
            } else {
                prev->next_with_same_initial_set = item;
            }

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
            const bool build_parse_tree,
            parse_tree_type **tree
        ) throw() {

            bool parse_result(false);

            (void) cfg;
            (void) is_nullable;
            /*
            const char *tokens[] = {
                "platoon", "one", "assault", "checkpoint",
                "one", "in", "vee", "formation",
                "at", "fifty", "miles", "per",
                "hour",
                0
            };
            const unsigned num_tokens(13U);
            */

            const char *tokens[] = {
                "1", "+", "2", "*", "(", "3", "+", "4", ")"
            };
            const unsigned num_tokens(9U);


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
                if(0 == tokens[0] || '\0' == *(tokens[0])) {
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
            fltl::helper::Array<earley_item_type *> set_index[2];
            set_index[0].reserve(num_tokens + 1U);
            set_index[0].set_size(num_tokens + 1U);
            set_index[1].reserve(num_tokens + 1U);
            set_index[1].set_size(num_tokens + 1U);
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
                ++i) {

                // get the terminal
                const char *serialized_token(tokens[i]);

                // done the tokens
                if(i == num_tokens) {
                    not_at_end = false;
                    printf("looking at EOF\n");

                } else {

                    traits_type::unserialize(
                        serialized_token,
                        lexeme
                    );

                    // try to get the terminal
                    solve_for_variable_terminal = !cfg.has_terminal(lexeme);
                    if(!solve_for_variable_terminal) {
                        a = cfg.get_terminal(lexeme);

                    // found a token but this grammar has no variable terminals
                    // and so it can't be substituted for anything
                    } else if(0 == cfg.num_variable_terminals()){

                        io::verbose("Failed to parse all input. Unknown lexeme '%s'.\n", serialized_token);
                        return false;
                    }

                    printf("looking at token '%s'...\n", serialized_token);
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

                        D( printf("   predicting for '%s --> ... [%u] %s ...'\n", cfg.get_name(A), dot, cfg.get_name(B)); )

                        // if B is nullable then add A --> ... B * ... to
                        // the item set
                        if(is_nullable[B.number()]) {

                            D( printf("      predicted '%s --> epsilon'\n", cfg.get_name(B)); )

                            next_item = item_allocator.allocate();
                            next_item->scanned_from(curr_item, curr_item);
                            curr_set->push(indexed(
                                set_index[curr_index],
                                item_allocator,
                                next_item
                            ));
                        }

                        // if we're using FIRST sets then use them to skip
                        // useless predictions
                        if(use_first_set && not_at_end
                        && !(first_terminals[B.number()]->operator[](a.number()))) {
                            continue;
                        }

                        // for each B --> alpha, add B --> * alpha to the
                        // item set
                        for(predictor_related.rewind();
                            predictor_related.match_next();) {

                            next_item = item_allocator.allocate();
                            next_item->predicted_from(
                                curr_item,
                                curr_set,
                                prod
                            );

                            curr_set->push(indexed(
                                set_index[curr_index],
                                item_allocator,
                                next_item
                            ));
                        }

                    // the item has the form A --> ... *
                    } else if(completer.match(curr_item->production)) {

                        D( printf("   completing for '%s --> ... [%u]'\n", cfg.get_name(A), dot); )

                        for(earley_item_type *rel_item(curr_item->initial_set->first);
                            0 != rel_item;
                            rel_item = rel_item->next) {

                            dot = rel_item->dot;
                            if(!completer_related.match(rel_item->production)) {
                                continue;
                            }

                            next_item = item_allocator.allocate();
                            next_item->scanned_from(rel_item, curr_item);
                            curr_set->push(indexed(
                                set_index[curr_index],
                                item_allocator,
                                next_item
                            ));

                        skip_complete_add_item:
                            continue;
                        }

                    // try to "solve" this terminal
                    } else if(not_at_end) {

                        // we don't know the terminal of this lexeme, lets
                        // see if we can substitute a variable terminal
                        if(solve_for_variable_terminal) {

                            printf("   it is a variable terminal.\n");

                            // try to match this production as
                            // A --> ... * a ... for some variable terminal
                            // a.
                            if(!scanner_unknown.match(curr_item->production)
                            || !cfg.is_variable_terminal(a)) {
                                continue;
                            }

                            printf("   solving for '%s --> ... [%u] %s ...'\n", cfg.get_name(A), dot, cfg.get_name(a));

                        // we know the terminal of this lexeme
                        } else {

                            // try to match this production as
                            // A --> ... * a ... where "a" is the terminal
                            // of the current lexeme.
                            if(!scanner_known.match(curr_item->production)) {
                                continue;
                            }

                            printf("   scanning for '%s --> ... [%u] %s ...'\n", cfg.get_name(A), dot, cfg.get_alpha(a));
                        }

                        next_set = curr_set->next;
                        if(0 == next_set) {
                            next_set = set_allocator.allocate();
                            curr_set->set_next(next_set);
                            clear_index(set_index[1U - curr_index]);
                        }

                        next_item = item_allocator.allocate();
                        next_item->scanned_from(curr_item, curr_item);
                        next_set->push(indexed(
                            set_index[1U - curr_index],
                            item_allocator,
                            next_item
                        ));
                    }
                }
            }

            if(i > num_tokens) {

                // handle the case where we accept the empty string
                if(0 == curr_set) {
                    curr_set = prev_set;
                }

                if(0 == curr_set) {
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

                        if(build_parse_tree) {

                            io::verbose("Building parse tree...\n");
                            *tree = make_parse_tree(
                                cfg,
                                first_item,
                                curr_item
                            );
                        }
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
#if 1
        static void print_dotted_production(CFG &cfg, earley_item_type *item) throw() {
            printf("%s -->", cfg.get_name(item->production.variable()));
            symbol_string_type syms(item->production.symbols());
            for(unsigned i(0); i < syms.length(); ++i) {
                if(i == item->dot) {
                    printf(" @");
                }

                if(syms.at(i).is_variable()) {
                    printf(" %s", cfg.get_name(variable_type(syms.at(i))));
                } else {
                    printf(" %s", cfg.get_alpha(terminal_type(syms.at(i))));
                }
            }
            if(item->dot == syms.length()) {
                printf(" @");
            }
            printf("\n");
        }

        static void print_derivation(CFG &cfg, earley_item_type *item, int depth) throw() {

            for(; 0 != item; item = item->caused_by) {
                printf("%*s", depth, "");
                print_dotted_production(cfg, item);

                //if(0 != item->caused_by) {
                //    print_derivation(cfg, item->caused_by, depth + 4);
                //}
            }
        }

#endif
        static parse_tree_type *make_parse_tree(
            CFG &cfg,
            earley_item_type *first_item,
            earley_item_type *final_item
        ) throw() {

            (void) cfg;
            (void) first_item;

            std::vector<parse_tree_type *> stack;
            parse_tree_type *last(0);


            print_derivation(cfg, final_item, 0);

            return 0;
#if 0
                production_type prod(item->production);
                symbol_string_type str(prod.symbols());
                unsigned num_symbols(str.length());

                // end of a derivation
                if(item->dot == num_symbols) {
                    stack.push_back(new parse_tree_type(prod));
                    printf("    ");
                    for(unsigned i(0); i < stack.size(); ++i) {
                        printf("%s ", cfg.get_name(variable_type(stack[i]->symbol)));
                    }
                    printf("\n");
                } else {

                    assert(!stack.empty());

                    printf("    ");
                    for(unsigned i(0); i < stack.size(); ++i) {
                        printf("%s ", cfg.get_name(variable_type(stack[i]->symbol)));
                    }
                    printf("\n");

                    // add in a terminal parse tree
                    if(str.at(item->dot).is_terminal()) {

                        stack.back()->add_child(new parse_tree_type(
                            terminal_type(str.at(item->dot)),
                            0
                        ));
                    } else {

                        last = stack.back();
                        stack.pop_back();

                        if(!stack.empty()) {
                            stack.back()->add_child(last);
                        }
                    }
                }
            }

            return last;
#endif
            /*
            // reverse the list
            for(earley_item_type *item(final_item), *prev(0), *next(0);
                0 != item;
                item = next) {
                next = item->antecedent;
                item->antecedent = prev;
                prev = item;
            }

            // build the tree
            std::vector<parse_tree_type *> stack;
            parse_tree_type *last(0);

            for(earley_item_type *item(first_item->antecedent);
                0 != item;
                item = item->antecedent) {

                print_dotted_production(cfg, item);

                production_type prod(item->production);
                symbol_string_type str(prod.symbols());
                unsigned num_symbols(str.length());

                // end of a derivation
                if(item->dot == num_symbols) {
                    assert(!stack.empty());

                    last = stack.back();
                    stack.pop_back();

                    if(!stack.empty()) {

                        if(0 < item->dot) {
                            if(str.at(item->dot - 1) == last->symbol) {

                            }
                        } else {
                            stack.back()->add_child(last);
                        }
                    }

                // pointing at a symbol
                } else {

                    if(0 == item->dot) {
                        parse_tree_type *child(new parse_tree_type(prod));
                        stack.push_back(child);
                    }

                    assert(!stack.empty());

                    // add in a terminal parse tree
                    if(str.at(item->dot).is_terminal()) {
                        stack.push_back(new parse_tree_type(
                            terminal_type(str.at(item->dot)),
                            0
                        ));
                    }
                }
            }
            */

            return last;
        }
    };
}}

#endif /* FLTL_CFG_EARLEY_PARSE_HPP_ */
