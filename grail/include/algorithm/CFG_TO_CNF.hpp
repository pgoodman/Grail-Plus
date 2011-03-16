/*
 * CFG_TO_CNF.hpp
 *
 *  Created on: Feb 7, 2011
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

#ifndef FLTL_CFG_TO_CNF_HPP_
#define FLTL_CFG_TO_CNF_HPP_

#include <cassert>
#include <set>
#include <map>
#include <utility>

#include "fltl/include/CFG.hpp"

#include "grail/include/algorithm/CFG_REMOVE_UNITS.hpp"
#include "grail/include/algorithm/CFG_REMOVE_EPSILON.hpp"
#include "grail/include/algorithm/CFG_TO_2CFG.hpp"

#include "grail/include/io/verbose.hpp"

namespace grail { namespace algorithm {

    /// convert a context-free grammar into Chomsky Normal Form.
    template <typename AlphaT>
    class CFG_TO_CNF {

        // take off the templates!
        typedef fltl::CFG<AlphaT> CFG;
        typedef typename CFG::variable_type variable_type;
        typedef typename CFG::terminal_type terminal_type;
        typedef typename CFG::production_type production_type;
        typedef typename CFG::generator_type generator_type;
        typedef typename CFG::symbol_string_type symbol_string_type;
        typedef typename CFG::production_builder_type production_builder_type;

    private:

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

            io::verbose("Adding new start variable...\n");

            // add a new start variable
            variable_type old_start_var(cfg.get_start_variable());
            variable_type new_start_var(cfg.add_variable());
            cfg.set_start_variable(new_start_var);
            cfg.add_production(new_start_var, old_start_var);

            io::verbose("Shortening productions...\n");

            CFG_TO_2CFG<AlphaT>::run(cfg);

            io::verbose("Removing epsilon productions...\n");

            CFG_REMOVE_EPSILON<AlphaT>::run(cfg);

            io::verbose("Removing unit productions...\n");

            CFG_REMOVE_UNITS<AlphaT>::run(cfg);

            io::verbose("Moving terminals to their own variables...\n");

            // keep track of those productions that only generate a terminal
            std::map<terminal_type, variable_type> terminal_rules;
            std::map<variable_type, variable_type> vars_to_replace;

            terminal_type T;
            production_type P;
            variable_type A;

            generator_type terminal_units(cfg.search(~P, (~A) --->* T));

            for(; terminal_units.match_next(); ) {
                if(1 == cfg.num_productions(A)) {
                    if(1 == terminal_rules.count(T)) {
                        vars_to_replace[A] = terminal_rules[T];
                        cfg.unsafe_remove_variable(A);
                    } else {
                        terminal_rules[T] = A;
                    }
                }
            }

            io::verbose("Updating non-unit productions with terminals...\n");

            clean_up_terminals(cfg, terminal_rules, vars_to_replace);

            io::verbose("Done.\n");
        }
    };

}}

#endif /* FLTL_CFG_TO_CNF_HPP_ */
