/*
 * PDA_TO_CFG.hpp
 *
 *  Created on: Mar 3, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_PDA_TO_CFG_HPP_
#define FLTL_PDA_TO_CFG_HPP_

#include <cstdio>

#include "fltl/include/CFG.hpp"
#include "fltl/include/PDA.hpp"

#include "fltl/include/helper/Array.hpp"
#include "fltl/include/helper/Table.hpp"

namespace grail { namespace algorithm {

    /// convert a context-free grammar into a pushdown automaton
    /// follows algorithm of Introduction to the Theory of Computation,
    /// Sipser, p. 120
    template <typename AlphaT>
    class PDA_TO_CFG {
    private:

        typedef fltl::PDA<AlphaT> PDA;
        typedef fltl::CFG<AlphaT> CFG;

        // normalize the PDA by giving it a single accept state and
        // ensuring that it empties the stack before it accepts
        static typename PDA::state_type normalize(PDA &pda) throw() {

            typename PDA::state_type push_arb_state(pda.add_state());
            typename PDA::state_type state;
            typename PDA::generator_type states(pda.search(~state));
            typename PDA::symbol_type push_pop(pda.add_stack_symbol());
            const typename PDA::symbol_type epsilon(pda.epsilon());

            // get every final state to point to an intermediate state
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

            typename PDA::state_type empty_stack_state(pda.add_state());

            // send the intermediate state to the new final state
            pda.add_transition(
                push_arb_state,
                epsilon,
                push_pop,
                epsilon,
                empty_stack_state
            );

            typename PDA::symbol_type symbol;
            typename PDA::generator_type symbols(pda.search(~symbol));

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
            typename PDA::symbol_type bottom(pda.add_stack_symbol());
            typename PDA::state_type final_state(pda.add_state());
            typename PDA::state_type start_state(pda.add_state());

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
            typename PDA::transition_type trans;
            typename PDA::generator_type transitions(pda.search(~trans));
            typename PDA::symbol_type push;
            typename PDA::symbol_type pop;
            typename PDA::state_type push_pop_state;

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

    public:

        static void run(PDA &pda, CFG &cfg) throw() {

            if(0 == pda.num_accept_states()) {
                return;
            }

            typename PDA::state_type final_state(normalize(pda));
            const unsigned num_states(pda.num_states());

            // represents the grammar variables $A_{pq}$ where $p, q \in Q$.
            fltl::helper::Table<
                typename CFG::variable_type
            > A(num_states, num_states);

            // the various generators over states
            typename PDA::state_type p;
            typename PDA::state_type q;
            typename PDA::state_type r;
            typename PDA::state_type s;

            typename PDA::generator_type states_p(pda.search(~p));
            typename PDA::generator_type states_q(pda.search(~q));
            typename PDA::generator_type states_r(pda.search(~r));

            typename PDA::symbol_type a;
            typename PDA::symbol_type b;
            typename PDA::symbol_type t;

            typename PDA::generator_type symbols_a(pda.search(~a));

            // initialize the variable table
            char scratch[1024] = {'\0'};
            for(; states_p.match_next(); states_q.rewind()) {
                for(; states_q.match_next(); ) {
                    sprintf(scratch, "A_%u_%u", p.number(), q.number());
                    A(p.number(), q.number()) = cfg.get_variable(scratch);
                }
            }

            // transfer input alphabet symbols from the PDA into terminal
            // symbols of the CFG
            fltl::helper::Array<
                typename CFG::terminal_type
            > E(pda.num_symbols());

            for(; symbols_a.match_next(); ) {
                if(pda.is_in_input_alphabet(a)) {
                    E.get(a.number()) = cfg.get_terminal(pda.get_alpha(a));
                }
            }

            const typename PDA::symbol_type epsilon(pda.epsilon());

            typename PDA::generator_type p_a_epsilon_contains_t_r(
                pda.search(~p, ~a, epsilon, ~t, ~r)
            );

            typename PDA::generator_type s_b_t_contains_epsilon_q(
                pda.search(~s, ~b, t, epsilon, ~q)
            );

            typename CFG::production_builder_type buffer;

            // add in the productions $A_{pq} \to a A_{rs} b$
            for(; p_a_epsilon_contains_t_r.match_next(); ) {
                for(s_b_t_contains_epsilon_q.rewind();
                    s_b_t_contains_epsilon_q.match_next();) {

                    cfg.add_production(
                        A(p.number(), q.number()),
                        buffer.clear()
                         << E.get(a.number())
                         << A(r.number(), s.number())
                         << E.get(b.number())
                    );
                }
            }

            // set the start variable
            cfg.set_start_variable(A(
                pda.get_start_state().number(),
                final_state.number()
            ));

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

                // add in rules $A_{pp} \to \epsilon$
                cfg.add_production(
                    A(p.number(), p.number()),
                    cfg.epsilon()
                );
            }
        }
    };
}}

#endif /* FLTL_PDA_TO_CFG_HPP_ */
