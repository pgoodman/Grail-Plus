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

        // normalize the PDA by giving it a single accept state and
        // ensuring that it empties the stack before it accepts
        static typename PDA::state_type normalize(PDA &pda) throw() {

            typename PDA::state_type push_arb_state(pda.add_state());
            typename PDA::state_type state;
            typename PDA::generator_type states(pda.search(~state));
            typename PDA::symbol_type push_pop(pda.add_stack_symbol());
            const typename PDA::symbol_type epsilon(pda.epsilon());

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

        static void run_pda(
            PDA &pda,
            CFG &cfg,
            fltl::helper::Array<typename CFG::terminal_type> &E
        ) throw() {

            io::verbose("Normalizing PDA...\n");

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

            const typename PDA::symbol_type epsilon(pda.epsilon());
            typename CFG::production_builder_type buffer;

            io::verbose("Adding productions...\n");

            // assume there is a path from r to s that brings the PDA from
            // an empty stack to an empty stack.

            // if there is a transition from p to r that brings the PDA from
            // an empty stack to a stack with t on it and reads a...
            typename PDA::generator_type p_to_r_push(pda.search(
                ~p,
                ~a,
                epsilon,
                ~t,
                ~r
            ));

            /// ... and if there is a transition from s to q that pops t off
            /// of the stack when reading b
            typename PDA::generator_type s_to_q_pop(pda.search(
                ~s,
                ~b,
                t,
                epsilon,
                ~q
            ));

            io::verbose("Adding productions of the form A_pq -> a A_rs b...\n");

            typename CFG::symbol_string_type a_str;
            typename CFG::symbol_string_type b_str;

            // then add A_pq -> a A_rs b
            for(; p_to_r_push.match_next(); ) {

                if(epsilon == a) {
                    a_str = cfg.epsilon();
                } else {
                    a_str = E.get(a.number());
                }

                /*
                printf(
                    "%u %s %u, epsilon / %s\n",
                    p.number(),
                    pda.is_in_input_alphabet(a) ? pda.get_alpha(a) : pda.get_name(a),
                    r.number(),
                    pda.is_in_input_alphabet(t) ? pda.get_alpha(t) : pda.get_name(t)
                );
                */

                for(s_to_q_pop.rewind();
                    s_to_q_pop.match_next();) {

                    if(epsilon == b) {
                        b_str = cfg.epsilon();
                    } else {
                        b_str = E.get(b.number());
                    }

                    /*
                    printf(
                        "    %u %s %u, %s / epsilon\n",
                        s.number(),
                        pda.is_in_input_alphabet(b) ? pda.get_alpha(b) : pda.get_name(b),
                        q.number(),
                        pda.is_in_input_alphabet(t) ? pda.get_alpha(t) : pda.get_name(t)
                    );

                    printf(
                        "        A_%u_%u --> %s A_%u_%u %s\n",
                        p.number(), q.number(),
                        a_str.length() == 0 ? "epsilon" : cfg.get_alpha(typename CFG::terminal_type(a_str.at(0))),
                        r.number(), s.number(),
                        b_str.length() == 0 ? "epsilon" : cfg.get_alpha(typename CFG::terminal_type(b_str.at(0)))
                    );
                    */
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
            fltl::helper::Array<typename CFG::terminal_type> &E
        ) throw() {

            char scratch[1024] = {'\0'};

            fltl::helper::Array<typename CFG::variable_type> A(
                pda.num_states() + 1U
            );
            A.set_size(pda.num_states() + 1U);

            typename PDA::state_type source;
            typename PDA::state_type sink;
            typename PDA::symbol_type input;
            typename PDA::generator_type states(pda.search(~source));

            io::verbose("Mapping NFA states to CFG variables.\n");

            for(; states.match_next(); ) {
                sprintf(scratch, "A_%u", source.number());
                A.set(source.number(), cfg.get_variable(scratch));
            }

            typename PDA::generator_type transitions(pda.search(
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
            typename PDA::generator_type nfa_transitions(pda.search(
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
            fltl::helper::Array<
                typename CFG::terminal_type
            > E(pda.num_symbols() + 1U);
            E.set_size(pda.num_symbols() + 1U);

            typename PDA::symbol_type a;
            typename PDA::generator_type symbols_a(pda.search(~a));

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
