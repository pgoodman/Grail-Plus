/*
 * CFG_REMOVE_LR.hpp
 *
 *  Created on: Mar 5, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_CFG_REMOVE_LR_HPP_
#define FLTL_CFG_REMOVE_LR_HPP_

#include <cassert>
#include <set>
#include <map>
#include <utility>

#include "fltl/include/CFG.hpp"

#include "grail/algorithm/CFG_REMOVE_EPSILON.hpp"
#include "grail/algorithm/CFG_REMOVE_UNITS.hpp"

namespace grail { namespace algorithm {

    /// remove all left recursion from a context-free grammar.
    template <typename AlphaT>
    class CFG_REMOVE_LR {
    private:

        // take off the templates!
        typedef fltl::CFG<AlphaT> CFG;
        typedef typename CFG::symbol_type symbol_type;
        typedef typename CFG::variable_type variable_type;
        typedef typename CFG::terminal_type terminal_type;
        typedef typename CFG::production_type production_type;
        typedef typename CFG::generator_type generator_type;
        typedef typename CFG::symbol_string_type symbol_string_type;

        /// remove direct left recursion from a variable of a grammar
        static void remove_direct_left_recursion(
            CFG &cfg,
            variable_type A
        ) throw() {
            symbol_string_type alpha;
            symbol_string_type beta;
            production_type LR_prod;
            generator_type LR_productions(cfg.search(
                ~LR_prod,
                A --->* A + ~alpha
            ));
            generator_type productions(cfg.search(A --->* ~beta));

            if(!LR_productions.match_next()) {
                return;
            }

            variable_type tail(cfg.add_variable());
            cfg.add_production(tail, cfg.epsilon());

            for(LR_productions.rewind();
                LR_productions.match_next();
                productions.rewind()) {

                if(alpha.is_empty()) {
                    continue;
                }

                cfg.add_production(tail, alpha + tail);

                for(; productions.match_next(); ) {
                    if(beta.is_empty() || A == beta.at(0)) {
                        continue;
                    }

                    cfg.add_production(A, beta + tail);
                }

                cfg.remove_production(LR_prod);
            }
        }

        /// turn all indirect left recursion into direct left recursion
        /// by expanding out productions, and remove direct recursion using
        /// the above procedure
        static void remove_left_recursion(CFG &cfg) throw() {

            variable_type A_i;
            variable_type A_j;

            generator_type A_i_gen(cfg.search(~A_i));
            generator_type A_j_gen(cfg.search(~A_j));

            symbol_string_type gamma;
            symbol_string_type delta;

            production_type A_i_A_j_gamma;

            generator_type A_i_productions(cfg.search(
                ~A_i_A_j_gamma,
                A_i --->* A_j + ~gamma
            ));
            generator_type A_j_productions(cfg.search(A_j --->* ~delta));

            bool added_direct_lr(false);
            symbol_string_type delta_gamma;

            for(; A_i_gen.match_next(); A_j_gen.rewind()) {

                remove_direct_left_recursion(cfg, A_i);

                for(; A_j_gen.match_next(); ) {

                    if(A_j >= A_i) {
                        break;
                    }

                    added_direct_lr = false;

                    for(A_i_productions.rewind();
                        A_i_productions.match_next(); ) {

                        for(A_j_productions.rewind();
                            A_j_productions.match_next(); ) {

                            delta_gamma = cfg.add_production(
                                A_i,
                                delta + gamma
                            ).symbols();

                            if(!delta_gamma.is_empty()
                            && A_i == delta_gamma.at(0)) {
                                added_direct_lr = true;
                            }
                        }

                        cfg.remove_production(A_i_A_j_gamma);
                    }

                    if(added_direct_lr) {
                        remove_direct_left_recursion(cfg, A_i);
                    }
                }
            }
        }

    public:

        static void run(CFG &cfg) throw() {

            variable_type start_var(cfg.add_variable());
            terminal_type epsilon_placeholder;
            production_type prod;
            bool using_epsilon_placeholder(false);

            cfg.add_production(start_var, cfg.get_start_variable());
            cfg.set_start_variable(start_var);

            CFG_REMOVE_UNITS<AlphaT>::run(cfg);
            CFG_REMOVE_EPSILON<AlphaT>::run(cfg);

            // if cfg generates the empty word then lets replace that
            // by making it mean
            if(cfg.search(~prod, start_var --->* cfg.epsilon()).match_next()) {
                using_epsilon_placeholder = true;
                epsilon_placeholder = cfg.add_variable_terminal();
                cfg.remove_production(prod);
                prod = cfg.add_production(start_var, epsilon_placeholder);
            }

            remove_left_recursion(cfg);

            if(using_epsilon_placeholder) {
                cfg.remove_production(prod);
                cfg.add_production(start_var, cfg.epsilon());
            }
        }
    };
}}

#if 0
        typedef std::set<variable_type> variable_set_type;

        typedef std::map<
            variable_type,
            variable_set_type *
        > left_corner_map_type;

        /// compute all left corners
        static void find_left_corners(
            CFG &cfg,
            left_corner_map_type &left_corners /*,
            variable_set_type &nullable */
        ) throw() {
            variable_type A;
            variable_type B;
            symbol_string_type str;
            generator_type variables(cfg.search(~A));

            for(; variables.match_next(); ) {
                left_corners[A] = new std::set<variable_type>;
            }

            generator_type productions(cfg.search((~A) --->* ~B + ~str));
            std::set<variable_type> *corners(0);
            std::set<variable_type> *reachable_corners(0);
            variable_type prev_var;
            variable_type C;

            unsigned old_size(0);
            //unsigned i(0);
            bool updated(true);

            for(; updated; productions.rewind()) {

                updated = false;

                for(; productions.match_next(); ) {

                    corners = left_corners[A];
                    old_size = corners->size();

                    reachable_corners = left_corners[B];
                    corners->insert(B);
                    corners->insert(
                        reachable_corners->begin(),
                        reachable_corners->end()
                    );

#if 0
                    corners->insert(B);

                    // go through the symbols and look back at the previous
                    // variable. if the previous variable is nullable, then
                    // the current variable is a left corner of A
                    prev_var = B;
                    for(i = 0; i < str.length(); ++i) {
                        if(!str.at(i).is_variable()) {
                            break;
                        }
                        C = str.at(i);

                        if(0 == nullable.count(prev_var)) {
                            break;
                        }

                        // we can reach these variables
                        reachable_corners = left_corners[C];
                        corners->insert(C);
                        corners->insert(
                            reachable_corners->begin(),
                            reachable_corners->end()
                        );
                        prev_var = C;
                    }
#endif

                    // we've found new left corners
                    if(corners->size() > old_size) {
                        updated = true;
                    }
                }
            }
        }
#if 0
        /// compute all nullable variables
        static void find_nullable(
            CFG &cfg,
            variable_set_type &nullable
        ) throw() {
            variable_type V;
            variable_type Q;

            // base case: directly nullable productions
            generator_type null_prods(cfg.search((~V) --->* cfg.epsilon()));
            for(; null_prods.match_next(); ) {
                nullable.insert(V);
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
                    if(0 != nullable.count(V)
                    || 0 == nullable.count(Q)) {
                        continue;
                    }

                    for(i = 0; i < str.length(); ++i) {
                        if(str.at(i).is_terminal()) {
                            break;
                        }

                        S = str.at(i);
                        if(0 == nullable.count(S)) {
                            break;
                        }
                    }

                    if(i == str.length()) {
                        nullable.insert(V);
                        found_nullable = true;
                    }
                }
            }
        }
#endif

        /// is a variable a proper left corner of some set?
        static bool is_proper_left_corner(
            const variable_set_type *left_corners,
            const variable_type var
        ) throw() {
            return 1U == left_corners->count(var);
        }

        typedef std::pair<variable_type, symbol_type> variable_subtract_type;
        typedef std::map<
            variable_subtract_type,
            variable_type
        > new_variable_map_type;

        /// "subtract" the corner of a variable from another
        static variable_type subtract(
            CFG &cfg,
            new_variable_map_type &new_variables,
            variable_type A,
            symbol_type &X
        ) throw() {
            const variable_subtract_type id(A,X);

            // create it
            if(0 == new_variables.count(id)) {
                new_variables[id] = cfg.add_variable();
            }

            return new_variables[id];
        }
#endif
#if 0
            variable_type A;
            variable_type X;
            symbol_string_type A_sub_X_str;
            symbol_string_type X_str;
            production_type prod;
            generator_type A_productions(cfg.search(
                ~prod,
                (~A) --->* ~X + ~A_sub_X_str
            ));
            generator_type X_productions(cfg.search(X --->* ~X_str));
            bool changed(true);
            bool added_direct_lr(false);
            unsigned old_num_productions;

            for(; changed; A_productions.rewind()) {
                changed = false;

                for(; A_productions.match_next(); ) {

                    // directly left recursive, skip!
                    assert(A != X);

                    added_direct_lr = !X_str.is_empty() && A == X_str.at(0);

                    for(X_productions.rewind(); X_productions.match_next();) {
                        cfg.add_production(A, X_str + A_sub_X_str);
                    }

                    // expand out the right-hand side
                    cfg.remove_production(prod);
                    old_num_productions = cfg.num_productions();

                    if(added_direct_lr) {
                        remove_direct_left_recursion(cfg, A);
                    }

#if 0
                    /*
                    if(cfg.has_default_production(A)) {
                        --old_num_productions;
                    }*/

                    for(X_productions.rewind(); X_productions.match_next();) {
                        cfg.add_production(A, X_str + A_sub_X_str);
                    }

                    if(old_num_productions < cfg.num_productions()) {
                        printf("%u %u\n", old_num_productions, cfg.num_productions());
                        changed = true;

                        if(++j > 100) {
                            return;
                        }
                    }
#endif

                    changed = true;
                }

                grail::io::fprint(stdout, cfg);
                printf("\n-------------------------------------\n");


            }
#endif
#if 0
#if 0
            // set of all nullable variables
            variable_set_type nullable;
            find_nullable(cfg, nullable);
#endif

            CFG_REMOVE_EPSILON<AlphaT>::run(cfg);

            // set of all left corners
            left_corner_map_type left_corners;
            find_left_corners(cfg, left_corners);

            // set of variables of the form A-X that are added to the grammar
            // when the left corners are expanded
            new_variable_map_type new_variables;

            //variable_type A;
            //variable_type B;

            typename variable_set_type::iterator it_curr;
            typename variable_set_type::iterator it_end;
#if 0
            printf("nullable:\n");
            for(it_curr = nullable.begin(), it_end = nullable.end();
                it_curr != it_end;
                ++it_curr) {
                printf("  %s\n", cfg.get_name(*it_curr));
            }
#endif
            typename left_corner_map_type::iterator map_curr(left_corners.begin());
            typename left_corner_map_type::iterator map_end(left_corners.end());
            std::pair<variable_type, variable_set_type *> elm;
            printf("\nleft corners:\n");

            for(; map_curr != map_end; ++map_curr) {
                elm = *map_curr;

                printf("  %s:\n", cfg.get_name(elm.first));

                for(it_curr = elm.second->begin(), it_end = elm.second->end();
                    it_curr != it_end;
                    ++it_curr) {
                    printf("    %s\n", cfg.get_name(*it_curr));
                }
            }
#endif

#endif /* FLTL_CFG_REMOVE_LR_HPP_ */
