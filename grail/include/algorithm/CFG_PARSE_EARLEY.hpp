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
                item->current_set = this;
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
            //earley_item_type *caused_by;
            //earley_item_type *predecessor;

            // the set that first introduced the production being used. that
            // is, sliding the dot changes the set that owns an item, but not
            // the set that initiated an item.
            earley_set_type *initial_set;
            earley_set_type *current_set;

            // the next item in the same set that has the same initial
            // set
            earley_item_type *next_with_same_initial_set;

            earley_item_type(void)
                : dot(0)
                , production()
                , next(0)
                //, caused_by(0)
                //, predecessor(0)
                , initial_set(0)
                , current_set(0)
                , next_with_same_initial_set(0)
            { }

            void scanned_from(
                earley_item_type *scan
            ) throw() {
                dot = scan->dot + 1U;
                production = scan->production;
                //predecessor = scan;
                initial_set = scan->initial_set;
            }

            void predicted_from(
                //earley_item_type *ant,
                earley_set_type *set,
                const production_type &prod
            ) throw() {
                dot = 0;
                production = prod;
                //predecessor = ant;
                initial_set = set;
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
        indexed_push(
            earley_set_type *set,
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
                items = item;

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
            std::vector<std::vector<bool> *> &first_terminals //,
            //const bool build_parse_tree,
            //parse_tree_type **tree
        ) throw() {

            bool parse_result(false);

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
                    io::verbose("    Looking at EOF\n");

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

                    io::verbose("    Looking at '%s'...\n", serialized_token);
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
                        && !(first_terminals[B.number()]->operator[](a.number()))) {
                            continue;
                        }

                        // for each B --> alpha, add B --> * alpha to the
                        // item set
                        for(predictor_related.rewind();
                            predictor_related.match_next();) {

                            next_item = item_allocator.allocate();
                            next_item->predicted_from(
                                //curr_item,
                                curr_set,
                                prod
                            );

                            indexed_push(
                                curr_set,
                                set_index[curr_index],
                                item_allocator,
                                next_item
                            );
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
                            next_item->scanned_from(rel_item);
                            next_item = indexed_push(
                                curr_set,
                                set_index[curr_index],
                                item_allocator,
                                next_item
                            );

                            //next_item->caused_by = curr_item;
                            //++(curr_item->in_degree);
                        }

                    // try to "solve" this terminal
                    } else if(not_at_end) {

                        // we don't know the terminal of this lexeme, lets
                        // see if we can substitute a variable terminal
                        if(solve_for_variable_terminal) {

                            D( printf("   it is a variable terminal.\n"); )

                            // try to match this production as
                            // A --> ... * a ... for some variable terminal
                            // a.
                            if(!scanner_unknown.match(curr_item->production)
                            || !cfg.is_variable_terminal(a)) {
                                continue;
                            }

                            D( printf("   solving for '%s --> ... [%u] %s ...'\n", cfg.get_name(A), dot, cfg.get_name(a)); )

                        // we know the terminal of this lexeme
                        } else {

                            // try to match this production as
                            // A --> ... * a ... where "a" is the terminal
                            // of the current lexeme.
                            if(!scanner_known.match(curr_item->production)) {
                                continue;
                            }

                            D( printf("   scanning for '%s --> ... [%u] %s ...'\n", cfg.get_name(A), dot, cfg.get_alpha(a)); )
                        }

                        next_set = curr_set->next;
                        if(0 == next_set) {
                            next_set = set_allocator.allocate();
                            curr_set->set_next(next_set);
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
#if 0
                        if(build_parse_tree) {

                            io::verbose("Building parse tree...\n");

                            /**tree = make_parse_tree(
                                cfg,
                                first_item,
                                curr_item,
                                num_tokens
                            );*/

                            (void) tree;
                            curr_item->in_degree = 1;

                            //remove_useless(first_set, item_allocator);
                            print_sets(cfg, first_set);
                        }
#endif
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
#if 0

#if 0
        static void remove_useless(
            earley_set_type *first_set,
            fltl::helper::BlockAllocator<
                earley_item_type, NUM_BLOCKS
            > &item_allocator
        ) throw() {

            for(earley_set_type *set(first_set);
                0 != set;
                set = set->next) {

                earley_item_type **prev(&(set->first));

                for(earley_item_type *item(set->first), *curr(0), *next(0);
                    0 != item; ) {

                    curr = item;
                    for(next = item->next;
                        0 == item->in_degree;
                        item = next) {

                        next = item->next;
                        item_allocator.deallocate(item);
                    }

                    *prev = item;

                    if(0 == item) {
                        break;
                    }

                    prev = &(item->next);
                }
            }
        }
#endif

        static void print_dotted_production(
            CFG &cfg,
            earley_item_type *item
        ) throw() {
            production_type prod(item->production);
            const unsigned dot(item->dot);

            printf(
                "        p%p [label=<%s &rarr;",
                reinterpret_cast<void *>(item),
                cfg.get_name(prod.variable())
            );
            symbol_string_type syms(prod.symbols());
            for(unsigned i(0); i < syms.length(); ++i) {
                if(i == dot) {
                    printf(" &middot;");
                }

                if(syms.at(i).is_variable()) {
                    printf(" %s", cfg.get_name(variable_type(syms.at(i))));
                } else {
                    printf(" '%s' ", cfg.get_alpha(terminal_type(syms.at(i))));
                }
            }
            if(dot == syms.length()) {
                printf(" &middot;");
            }
            printf(">]\n");
        }

        static void print_sets(
            CFG &cfg,
            earley_set_type *first_set
        ) throw() {

            printf("digraph {\n");
            printf("    rankdir=LR;\n");

            for(earley_set_type *set(first_set);
                0 != set;
                set = set->next) {

                printf("    subgraph cluster_%u {\n", set->offset);
                printf("        style=filled;\n");
                printf("        node [style=filled,color=white];\n");
                printf("        color=lightgrey;\n");
                printf("        rankdir=TB;\n");
                printf("        label = < S<sub>%u</sub> >;\n", set->offset);

                for(earley_item_type *item(set->first);
                    0 != item;
                    item = item->next) {

                    print_dotted_production(cfg, item);

                    if(0 != item->predecessor
                    && set == item->predecessor->current_set) {

                        printf(
                            "        p%p -> p%p [style=dotted];\n",
                            reinterpret_cast<void *>(item),
                            reinterpret_cast<void *>(item->predecessor)
                        );

                    }

                    if(0 != item->caused_by
                    && set == item->caused_by->current_set) {
                        printf(
                            "        p%p -> p%p;\n",
                            reinterpret_cast<void *>(item),
                            reinterpret_cast<void *>(item->caused_by)
                        );
                    }
                }

                printf("    }\n");

            }

            for(earley_set_type *set(first_set);
                0 != set;
                set = set->next) {

                for(earley_item_type *item(set->first);
                    0 != item;
                    item = item->next) {

                    if(0 != item->predecessor
                    && set != item->predecessor->current_set) {
                        printf(
                            "    p%p -> p%p [style=dotted];\n",
                            reinterpret_cast<void *>(item),
                            reinterpret_cast<void *>(item->predecessor)
                        );
                    }

                    if(0 != item->caused_by
                    && set != item->caused_by->current_set) {
                        printf(
                            "    p%p -> p%p;\n",
                            reinterpret_cast<void *>(item),
                            reinterpret_cast<void *>(item->caused_by)
                        );
                    }
                }
            }

            printf("}\n");

        }

#if 0
        static earley_item_type *get_derivation(
            CFG &cfg,
            earley_item_type *first,
            earley_item_type **last,
            bool *seen_final
        ) throw() {
            if(0 == first) {
                return 0;
            }

            if(*seen_final) {
                return 0;
            }

            earley_item_type *prev(0);
            for(earley_item_type *item(first), *next(0);
                0 != item;
                item = next) {

                if(0 != prev) {
                    prev->scanned_by = item;
                }

                prev = item;
                next = item->scanned_by;

                if(0 == item->caused_by && 0 == item->scanned_by) {
                    *seen_final = true;
                    item->scanned_by = 0;
                    goto done;
                }

                print_dotted_production(cfg, item->production, item->dot);

                item->scanned_by = get_derivation(
                    cfg,
                    item->caused_by,
                    &prev,
                    seen_final
                );

                if(*seen_final) {
                    break;
                }
            }

        done:

            *last = prev;
            return first;
        }
#endif
#endif
#if 0
        static parse_tree_type *make_parse_tree(
            CFG &cfg,
            earley_item_type *first_item,
            earley_item_type *final_item,
            const unsigned num_tokens
        ) throw() {

            (void) first_item;
            (void) cfg;
            (void) num_tokens;
#if 0
            earley_item_type *end(0);
            bool seen_final(false);
            earley_item_type *begin(get_derivation(
                cfg,
                final_item,
                &end,
                &seen_final
            ));

            std::vector<parse_tree_type *> stack;
            std::vector<earley_item_type *> shadow;
            parse_tree_type *last(0);

            for(earley_item_type *item(begin);
                0 != item;
                item = item->scanned_by) {

                production_type prod(item->production);
                symbol_string_type str(prod.symbols());

                printf("\n");
                print_dotted_production(cfg, item->production, item->dot);
                for(unsigned i(0); i < shadow.size(); ++i) {
                    printf("    ");
                    print_dotted_production(cfg, shadow[i]->production, shadow[i]->dot);
                }

                if(item->dot == str.length()) {
                    stack.push_back(new parse_tree_type(prod));
                    shadow.push_back(item);
                } else {
                    assert(!stack.empty());

                    if(str.at(item->dot).is_terminal()) {
                        stack.back()->add_child(new parse_tree_type(
                            terminal_type(str.at(item->dot)),
                            0
                        ));
                    } else {
                        last = stack.back();
                        stack.pop_back();
                        shadow.pop_back();

                        if(!stack.empty()) {
                            stack.back()->add_child(last);
                        }
                    }
                }
            }
#endif
#if 0
            std::vector<parse_tree_type *> stack;
            std::vector<earley_item_type *> shadow;
            parse_tree_type *last(0);

            if(first_item == final_item) {
                return 0;
            }




            for(earley_item_type *next(0), *item(final_item);
                0 != item;
                item = item->scanned_by) {

                (void) next;

                printf("\n%p, %p, S_%u: ", reinterpret_cast<void *>(item->caused_by), reinterpret_cast<void *>(item->scanned_by), item->initial_set->offset);
                print_dotted_production(cfg, item->production, item->dot);

                make_parse_tree(cfg, 0, item->caused_by, num_tokens);

#if 0
                (void) next;

#if 0
                if(!stack.empty()) {
                    const unsigned offset(item->initial_set->offset);
                    earley_item_type *top(shadow.back());

                    if(offset < top->initial_set->offset) {

                        continue;
                    }

                    /*
                    for(earley_item_type *top(shadow.back());
                        offset < top->initial_set->offset; ) {

                        printf("POPPING: ");

                        shadow.pop_back();
                        delete (stack.back());

                        stack.pop_back();

                        assert(!stack.empty());

                        top = shadow.back();
                    }*/
                }
#endif
                printf("\n%p, %p, S_%u: ", reinterpret_cast<void *>(item->caused_by), reinterpret_cast<void *>(item->scanned_by), item->initial_set->offset);
                print_dotted_production(cfg, item->production, item->dot);

                for(earley_item_type *scan(item->scanned_by);
                    0 != scan; scan = scan->scanned_by) {

                    if(0 == scan->caused_by) {
                        break;
                    }

                    printf("    %p, %p, S_%u: ", reinterpret_cast<void *>(scan->caused_by), reinterpret_cast<void *>(scan->scanned_by), scan->initial_set->offset);
                    print_dotted_production(cfg, scan->production, scan->dot);
                }

#if 0
                for(unsigned i(0); i < shadow.size(); ++i) {
                    printf("    ");
                    printf("%p, %p, %p, S_%u: ", reinterpret_cast<void *>(shadow[i]), reinterpret_cast<void *>(shadow[i]->caused_by), reinterpret_cast<void *>(shadow[i]->scanned_by), shadow[i]->initial_set->offset);
                    print_dotted_production(cfg, shadow[i]->production, shadow[i]->dot);
                }

                if(0 != item->caused_by) {
                    printf("CAUSED: ");
                    next = item->caused_by;
                } else {
                    printf("SCANNED: ");
                    next = item->scanned_by;
                }

                production_type prod(item->production);
                symbol_string_type str(prod.symbols());

                if(item->dot == str.length()) {
                    if(0 == item->dot) {
                        assert(!stack.empty());
                        stack.back()->add_child(new parse_tree_type(prod));
                    } else {
                        stack.push_back(new parse_tree_type(prod));
                        shadow.push_back(item);
                    }
                } else {
                    assert(!stack.empty());

                    if(str.at(item->dot).is_terminal()) {
                        stack.back()->add_child(new parse_tree_type(
                            terminal_type(str.at(item->dot)),
                            0
                        ));
                    } else {
                        last = stack.back();
                        stack.pop_back();
                        shadow.pop_back();

                        if(!stack.empty()) {
                            stack.back()->add_child(last);
                        }
                    }
                }
#endif
#endif
            }
#endif

            return last;
        }
#endif
    };
}}

#endif /* FLTL_CFG_EARLEY_PARSE_HPP_ */
