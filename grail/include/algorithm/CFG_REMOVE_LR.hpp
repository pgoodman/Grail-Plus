/*
 * CFG_REMOVE_LR.hpp
 *
 *  Created on: Mar 5, 2011
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

#ifndef FLTL_CFG_REMOVE_LR_HPP_
#define FLTL_CFG_REMOVE_LR_HPP_

#include <cassert>
#include <set>
#include <map>
#include <utility>

#include "fltl/include/CFG.hpp"

#include "grail/include/algorithm/CFG_REMOVE_EPSILON.hpp"
#include "grail/include/algorithm/CFG_REMOVE_UNITS.hpp"

namespace grail { namespace algorithm {

    /// remove all left recursion from a context-free grammar.
    template <typename AlphaT>
    class CFG_REMOVE_LR {
    private:

        // take off the templates!
        typedef fltl::CFG<AlphaT> CFG;

        FLTL_CFG_USE_TYPES(CFG);

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

#endif /* FLTL_CFG_REMOVE_LR_HPP_ */
