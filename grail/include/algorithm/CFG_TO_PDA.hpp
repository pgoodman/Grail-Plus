/*
 * CFG_TO_PDA.hpp
 *
 *  Created on: Feb 27, 2011
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

#ifndef FLTL_CFG_TO_PDA_HPP_
#define FLTL_CFG_TO_PDA_HPP_

#include <map>

#include "fltl/include/CFG.hpp"
#include "fltl/include/PDA.hpp"

namespace grail { namespace algorithm {

    /// convert a context-free grammar into a pushdown automaton
    /// follows algorithm of Introduction to the Theory of Computation,
    /// Sipser, p. 117
    template <typename AlphaT>
    class CFG_TO_PDA {
    public:

        static void run(
            const fltl::CFG<AlphaT> &cfg,
            fltl::PDA<AlphaT> &pda
        ) throw() {

            if(0 == cfg.num_variables()
            || 0 == cfg.num_productions()
            || 0 != cfg.num_variable_terminals()) {
                return;
            }

            typedef fltl::CFG<AlphaT> CFG;
            typedef fltl::PDA<AlphaT> PDA;

            // make the initial symbols needed in the construction
            typename PDA::symbol_type bottom_of_stack(pda.add_stack_symbol());
            typename PDA::state_type q_start(pda.get_start_state());
            typename PDA::state_type q_loop(pda.add_state());
            typename PDA::state_type q_accept(pda.add_state());

            typename PDA::symbol_buffer_type buffer;

            // create a mapping between CFG variables and PDA symbols
            std::map<
                typename CFG::variable_type,
                typename PDA::symbol_type
            > var_symbols;

            // fill the mapping
            typename CFG::variable_type V;
            typename CFG::generator_type vars(cfg.search(~V));
            for(; vars.match_next(); ) {
                var_symbols[V] = pda.get_stack_symbol(cfg.get_name(V));
            }

            // creating a mapping between CFG terminals and PDA symbols
            std::map<
                typename CFG::terminal_type,
                typename PDA::symbol_type
            > term_symbols;

            // fill the mapping
            typename CFG::terminal_type T;
            typename PDA::symbol_type sym;
            typename CFG::generator_type terms(cfg.search(~T));
            for(; terms.match_next(); ) {
                sym = pda.get_alphabet_symbol(cfg.get_alpha(T));
                term_symbols[T] = sym;

                // handle the case where a terminal is on the top of the
                // stack
                pda.add_transition(q_loop, sym, sym, pda.epsilon(), q_loop);
            }

            // add in the initial transition
            pda.add_accept_state(q_accept);
            pda.add_transition(
                q_start, pda.epsilon(), pda.epsilon(),
                buffer << var_symbols[cfg.get_start_variable()]
                       << bottom_of_stack,
                q_loop
            );

            // add in productions
            typename CFG::symbol_string_type str;
            typename CFG::symbol_type cfg_sym;
            typename CFG::generator_type prods(cfg.search((~V) --->* ~str));

            // handle the case where a variable is on the top of the
            // stack
            for(; prods.match_next(); ) {

                buffer.clear();

                for(unsigned i(0); i < str.length(); ++i) {
                    cfg_sym = str.at(i);

                    if(cfg_sym.is_variable()) {
                        typename CFG::variable_type cfg_var(cfg_sym);
                        buffer.append(var_symbols[cfg_var]);

                    } else if(cfg_sym.is_terminal()) {
                        typename CFG::terminal_type cfg_term(cfg_sym);
                        buffer.append(term_symbols[cfg_term]);
                    }
                }

                pda.add_transition(
                    q_loop,
                    pda.epsilon(),
                    var_symbols[V],
                    buffer,
                    q_loop
                );
            }

            // handle the case where the empty stack marker is on top
            pda.add_transition(
                q_loop,
                pda.epsilon(),
                bottom_of_stack,
                pda.epsilon(),
                q_accept
            );
        }

    };

}}

#endif /* FLTL_CFG_TO_PDA_HPP_ */
