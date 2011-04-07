/*
 * PDA_TO_CFG.hpp
 *
 *  Created on: Mar 3, 2011
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

#ifndef FLTL_PDA_TO_CFG_HPP_
#define FLTL_PDA_TO_CFG_HPP_

#include <cstdio>

#include "fltl/include/CFG.hpp"
#include "fltl/include/PDA.hpp"

#include "fltl/include/helper/Array.hpp"
#include "fltl/include/helper/Table.hpp"

#include "grail/include/io/verbose.hpp"

#include "grail/include/algorithm/CFG_REMOVE_USELESS.hpp"

namespace grail { namespace algorithm {

    /// convert a context-free grammar into a pushdown automaton
    /// follows algorithm of Introduction to the Theory of Computation,
    /// Sipser, p. 120
    template <typename AlphaT>
    class PDA_TO_CFG {
    private:

        typedef fltl::PDA<AlphaT> PDA;
        typedef fltl::CFG<AlphaT> CFG;

        FLTL_PDA_USE_TYPES_PREFIX(PDA, pda);

        FLTL_CFG_USE_TYPES_PREFIX(CFG, cfg);

        // normalize the PDA by giving it a single accept state and
        // ensuring that it empties the stack before it accepts
        static pda_state_type normalize(PDA &pda) throw() {

            pda_state_type push_arb_state(pda.add_state());
            pda_state_type state;
            pda_generator_type states(pda.search(~state));
            pda_symbol_type push_pop(pda.add_stack_symbol());
            const pda_symbol_type epsilon(pda.epsilon());

            // get every final state to point to an intermediate state
            // the intermediate state is in normalized form by pushing on
            // an arbitrary symbol
            for(; states.match_next(); ) {
                if(!pda.is_accept_state(state)) {
                    continue;
                }
                pda.remove_accept_state(state);
                pda.add_transition(
                    state,
                    epsilon,
                    epsilon,
                    push_pop,
                    push_arb_state
                );
            }

            pda_state_type empty_stack_state(pda.add_state());

            // send the intermediate state to the new final state
            pda.add_transition(
                push_arb_state,
                epsilon,
                push_pop,
                epsilon,
                empty_stack_state
            );

            pda_symbol_type symbol;
            pda_generator_type symbols(pda.search(~symbol));

            // have the empty_stack_state pop every symbol (but push_pop)
            for(; symbols.match_next(); ) {
                if(push_pop == symbol) {
                    continue;
                }

                pda.add_transition(
                    empty_stack_state,
                    epsilon,
                    symbol,
                    epsilon,
                    empty_stack_state
                );
            }

            // set a new start and final states and add their respective
            // transitions
            pda_symbol_type bottom(pda.add_stack_symbol());
            pda_state_type final_state(pda.add_state());
            pda_state_type start_state(pda.add_state());

            pda.add_transition(
                start_state,
                epsilon,
                epsilon,
                bottom,
                pda.get_start_state()
            );
            pda.add_transition(
                empty_stack_state,
                epsilon,
                bottom,
                epsilon,
                final_state
            );
            pda.set_start_state(start_state);
            pda.add_accept_state(final_state);

            // make sure that every transition either pushes or pops
            pda_transition_type trans;
            pda_generator_type transitions(pda.search(~trans));
            pda_symbol_type push;
            pda_symbol_type pop;
            pda_state_type push_pop_state;

            for(; transitions.match_next(); ) {
                push = trans.push();
                pop = trans.pop();

                // no push or pop, replace with a useless push/pop sequence
                if(epsilon == push && epsilon == pop) {
                    push_pop_state = pda.add_state();
                    pda.remove_transition(trans);
                    pda.add_transition(
                        trans.source(),
                        trans.read(),
                        epsilon,
                        push_pop,
                        push_pop_state
                    );
                    pda.add_transition(
                        push_pop_state,
                        epsilon,
                        push_pop,
                        epsilon,
                        trans.sink()
                    );

                // pushes and pops, repace with a pop, then a push
                } else if(epsilon != push && epsilon != pop) {
                    push_pop_state = pda.add_state();
                    pda.remove_transition(trans);
                    pda.add_transition(
                        trans.source(),
                        trans.read(),
                        trans.pop(),
                        epsilon,
                        push_pop_state
                    );
                    pda.add_transition(
                        push_pop_state,
                        epsilon,
                        epsilon,
                        trans.push(),
                        trans.sink()
                    );
                }
            }

            return final_state;
        }

        static void run_pda(
            PDA &pda,
            CFG &cfg,
            fltl::helper::Array<cfg_terminal_type> &E
        ) throw() {

            io::verbose("Normalizing PDA...\n");

            pda_state_type final_state(normalize(pda));
            const unsigned num_states(pda.num_states());

            // represents the grammar variables $A_{pq}$ where $p, q \in Q$.
            fltl::helper::Table<
                cfg_variable_type
            > A(num_states, num_states);

            // the various generators over states
            pda_state_type p;
            pda_state_type q;
            pda_state_type r;
            pda_state_type s;

            pda_generator_type states_p(pda.search(~p));
            pda_generator_type states_q(pda.search(~q));
            pda_generator_type states_r(pda.search(~r));

            pda_symbol_type a;
            pda_symbol_type b;
            pda_symbol_type t;

            io::verbose("Creating mapping between states and variables...\n");

            // initialize the variable table
            char scratch[1024] = {'\0'};
            for(; states_p.match_next(); states_q.rewind()) {
                for(; states_q.match_next(); ) {
                    sprintf(scratch, "A_%u_%u", p.number(), q.number());
                    A(p.number(), q.number()) = cfg.get_variable(scratch);
                }

                // add in rules A_pp -> epsilon
                cfg.add_production(
                    A(p.number(), p.number()),
                    cfg.epsilon()
                );
            }

            const pda_symbol_type epsilon(pda.epsilon());
            cfg_symbol_buffer_type buffer;

            io::verbose("Adding productions...\n");

            // assume there is a path from r to s that brings the PDA from
            // an empty stack to an empty stack.

            // if there is a transition from p to r that brings the PDA from
            // an empty stack to a stack with t on it and reads a...
            pda_generator_type p_to_r_push(pda.search(
                ~p,
                ~a,
                epsilon,
                ~t,
                ~r
            ));

            /// ... and if there is a transition from s to q that pops t off
            /// of the stack when reading b
            pda_generator_type s_to_q_pop(pda.search(
                ~s,
                ~b,
                t,
                epsilon,
                ~q
            ));

            io::verbose("Adding productions of the form A_pq -> a A_rs b...\n");

            cfg_symbol_string_type a_str;
            cfg_symbol_string_type b_str;

            // then add A_pq -> a A_rs b
            for(; p_to_r_push.match_next(); ) {

                if(epsilon == a) {
                    a_str = cfg.epsilon();
                } else {
                    a_str = E.get(a.number());
                }

                // t becomes bound by p_to_r_push

                for(s_to_q_pop.rewind();
                    s_to_q_pop.match_next();) {

                    if(epsilon == b) {
                        b_str = cfg.epsilon();
                    } else {
                        b_str = E.get(b.number());
                    }

                    cfg.add_production(
                        A(p.number(), q.number()),
                        buffer.clear()
                         << a_str
                         << A(r.number(), s.number())
                         << b_str
                    );
                }
            }

            // set the start variable
            cfg.set_start_variable(A(
                pda.get_start_state().number(),
                final_state.number()
            ));

            io::verbose("Adding productions of the form A_pq -> A_pr A_rq...\n");

            // add in rules $A_{pq} \to A_{pr} A_{rq}$ to the grammar
            for(states_p.rewind(); states_p.match_next(); ) {
                for(states_q.rewind(); states_q.match_next(); ) {
                    for(states_r.rewind(); states_r.match_next(); ) {
                        cfg.add_production(
                            A(p.number(), q.number()),
                            buffer.clear()
                             << A(p.number(), r.number())
                             << A(r.number(), q.number())
                        );
                    }
                }
            }
        }

        static void run_nfa(
            PDA &pda,
            CFG &cfg,
            fltl::helper::Array<cfg_terminal_type> &E
        ) throw() {

            char scratch[1024] = {'\0'};

            fltl::helper::Array<cfg_variable_type> A(
                pda.num_states() + 1U
            );
            A.set_size(pda.num_states() + 1U);

            pda_state_type source;
            pda_state_type sink;
            pda_symbol_type input;
            pda_generator_type states(pda.search(~source));

            io::verbose("Mapping NFA states to CFG variables.\n");

            for(; states.match_next(); ) {
                sprintf(scratch, "A_%u", source.number());
                A.set(source.number(), cfg.get_variable(scratch));
            }

            pda_generator_type transitions(pda.search(
                ~source,
                ~input,
                pda._,
                pda._,
                ~sink
            ));

            io::verbose("Adding productions...\n");

            for(; transitions.match_next(); ) {

                if(input == pda.epsilon()) {
                    cfg.add_production(
                        A.get(source.number()),
                        A.get(sink.number())
                    );
                } else {

                    cfg.add_production(
                        A.get(source.number()),
                        E.get(input.number()) + A.get(sink.number())
                    );
                }
            }

            for(states.rewind(); states.match_next(); ) {
                if(!pda.is_accept_state(source)) {
                    continue;
                }

                cfg.add_production(A.get(source.number()), cfg.epsilon());
            }

            cfg.set_start_variable(A.get(pda.get_start_state().number()));
        }

    public:

        static void run(PDA &pda, CFG &cfg) throw() {

            if(0 == pda.num_accept_states()) {
                return;
            }

            io::verbose("Checking to see if PDA is an NFA...\n");

            // count the number of nfa transitions in this pda
            unsigned num_nfa_transitions(0);
            pda_generator_type nfa_transitions(pda.search(
                pda._,
                pda._,
                pda.epsilon(),
                pda.epsilon(),
                pda._
            ));

            for(; nfa_transitions.match_next(); ) {
                ++num_nfa_transitions;
            }

            io::verbose("Importing PDA/NFA input alphabet into CFG...\n");

            // transfer input alphabet symbols from the PDA into terminal
            // symbols of the CFG
            fltl::helper::Array<cfg_terminal_type> E(pda.num_symbols() + 1U);
            E.set_size(pda.num_symbols() + 1U);

            pda_symbol_type a;
            pda_generator_type symbols_a(pda.search(~a));

            for(; symbols_a.match_next(); ) {
                if(pda.is_in_input_alphabet(a)) {
                    E.set(a.number(), cfg.get_terminal(pda.get_alpha(a)));
                }
            }

            // this PDA is an NFA
            if(pda.num_transitions() == num_nfa_transitions) {
                io::verbose("Converting NFA to CFG...\n");
                run_nfa(pda, cfg, E);
            } else {
                io::verbose("Converting PDA to CFG...\n");
                run_pda(pda, cfg, E);
            }

            io::verbose("Done.\n");
        }
    };
}}

#endif /* FLTL_PDA_TO_CFG_HPP_ */
