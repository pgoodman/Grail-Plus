/*
 * CFG_TO_CNF.hpp
 *
 *  Created on: Feb 7, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_CFG_TO_CNF_HPP_
#define FLTL_CFG_TO_CNF_HPP_

#include <cassert>
#include <set>
#include <map>
#include <utility>

#include "fltl/lib/CFG.hpp"

#include "grail/include/Algorithm.hpp"

#include "grail/algorithm/CFG_REMOVE_UNITS.hpp"
#include "grail/algorithm/CFG_REMOVE_EPSILON.hpp"

namespace grail { namespace algorithm {

    /// convert a context-free grammar into Chomsky Normal Form.
    template <typename AlphaT>
    class CFG_TO_CNF {

        // take off the templates!
        typedef fltl::lib::CFG<AlphaT> CFG;
        typedef typename CFG::variable_type variable_type;
        typedef typename CFG::terminal_type terminal_type;
        typedef typename CFG::production_type production_type;
        typedef typename CFG::generator_type generator_type;
        typedef typename CFG::symbol_string_type symbol_string_type;
        typedef typename CFG::production_builder_type production_builder_type;

    private:

        /// go look for all productions with three or more symbols on their RHS
        /// and break them into pairs of productions
        inline static void remove_long_productions(CFG &cfg) throw() {

            variable_type A;
            symbol_string_type str;
            production_type P;

            generator_type long_rules(cfg.search(
                ~P,
                (~A) --->* cfg._ + cfg._ + cfg._ + cfg.__
            ));

            for(; long_rules.match_next(); ) {

                cfg.remove_production(P);

                str = P.symbols();

                unsigned i(str.length() - 2);
                variable_type prev_new_var(cfg.add_variable());
                cfg.add_production(prev_new_var, str.substring(i, 2));

                for(--i; i > 0; --i) {
                    variable_type new_var(cfg.add_variable());
                    cfg.add_production(new_var, str.at(i) + prev_new_var);
                    prev_new_var = new_var;
                }

                cfg.add_production(A, str.at(0) + prev_new_var);
            }
        }

        // go look for productions with two symbols, if either of the symbols
        // are terminals, then replace them with variables
        inline static void clean_up_terminals(
            CFG &cfg,
            std::map<terminal_type, variable_type> &terminal_rules,
            std::map<variable_type, variable_type> &vars_to_replace
        ) throw() {

            production_type P;
            symbol_string_type str;
            variable_type A;
            variable_type B;
            terminal_type T;

            generator_type pairs(cfg.search(~P, cfg._ --->* cfg._ + cfg._));
            for(; pairs.match_next(); ) {
                str = P.symbols();

                const bool first_is_term(str.at(0).is_terminal());
                const bool second_is_term(str.at(1).is_terminal());

                if(first_is_term) {
                    T = str.at(0);
                    if(0 == terminal_rules.count(T)) {
                        A = cfg.add_variable();
                        cfg.add_production(A, T);
                        terminal_rules[T] = A;
                    } else {
                        A = terminal_rules[T];
                    }
                }

                if(second_is_term) {
                    T = str.at(1);
                    if(0 == terminal_rules.count(T)) {
                        B = cfg.add_variable();
                        cfg.add_production(B, T);
                        terminal_rules[T] = B;
                    } else {
                        B = terminal_rules[T];
                    }
                }

                // two terminals
                if(first_is_term && second_is_term) {
                    cfg.remove_production(P);
                    cfg.add_production(P.variable(), A + B);

                // first symbol is a terminal
                } else if(first_is_term) {
                    cfg.remove_production(P);
                    B = str.at(1);
                    if(vars_to_replace.count(B)) {
                        B = vars_to_replace[B];
                    }
                    cfg.add_production(P.variable(), A + B);

                // second symbol is a terminal
                } else if(second_is_term) {
                    cfg.remove_production(P);
                    A = str.at(0);
                    if(vars_to_replace.count(A)) {
                        A = vars_to_replace[A];
                    }
                    cfg.add_production(P.variable(), A + B);
                }
            }
        }

    public:

        /// convert a context-free grammar to chomsky normal form.
        static void run(CFG &cfg) throw() {

            if(0 == cfg.num_productions()) {
                return;
            }

            // add a new start variable
            variable_type old_start_var(cfg.get_start_variable());
            variable_type new_start_var(cfg.add_variable());
            cfg.set_start_variable(new_start_var);
            cfg.add_production(new_start_var, old_start_var);

            remove_long_productions(cfg);

            CFG_REMOVE_EPSILON<AlphaT>::run(cfg);
            CFG_REMOVE_UNITS<AlphaT>::run(cfg);

            // keep track of those productions that only generate a terminal
            std::map<terminal_type, variable_type> terminal_rules;
            std::map<variable_type, variable_type> vars_to_replace;

            terminal_type T;
            production_type P;
            variable_type A;
            variable_type B;

            generator_type terminal_units(cfg.search(~P, (~A) --->* T));

            for(; terminal_units.match_next(); ) {
                if(1 == cfg.num_productions(A)) {
                    if(1 == terminal_rules.count(T)) {
                        vars_to_replace[A] = terminal_rules[T];
                        cfg.remove_relation(A);
                    } else {
                        terminal_rules[T] = A;
                    }
                }
            }

            clean_up_terminals(cfg, terminal_rules, vars_to_replace);
        }
    };

}}

#endif /* FLTL_CFG_TO_CNF_HPP_ */
