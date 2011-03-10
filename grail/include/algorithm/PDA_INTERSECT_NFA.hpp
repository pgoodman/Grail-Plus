/*
 * PDA_INTERSECT_NFA.hpp
 *
 *  Created on: Mar 7, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_PDA_INTERSECT_NFA_HPP_
#define FLTL_PDA_INTERSECT_NFA_HPP_

#include <utility>
#include <map>

#include "fltl/include/NFA.hpp"
#include "fltl/include/PDA.hpp"

#include "grail/include/algorithm/NFA_REMOVE_EPSILON.hpp"

namespace grail { namespace algorithm {

    /// intersect a PDA and an NFA
    /// implementation follows Hopcroft, Motwani, Ullman, p. 292
    template <typename AlphaT>
    class PDA_INTERSECT_NFA {
    public:

        typedef fltl::PDA<AlphaT> PDA;
        typedef fltl::NFA<AlphaT> NFA;

        typedef typename PDA::state_type pda_state_type;
        typedef typename NFA::state_type nfa_state_type;

        typedef typename PDA::generator_type pda_generator_type;
        typedef typename NFA::generator_type nfa_generator_type;

        typedef typename PDA::symbol_type pda_symbol_type;
        typedef typename NFA::symbol_type nfa_symbol_type;

        typedef std::pair<pda_state_type,nfa_state_type> state_pair_type;

        typedef std::map<
            state_pair_type,
            pda_state_type
        > state_pair_map_type;

        /// get a state in the constructed automaton
        static pda_state_type get_state(
            PDA &out,
            state_pair_map_type &map,
            pda_state_type q,
            nfa_state_type p
        ) throw() {
            const state_pair_type pp(q, p);
            if(0 == map.count(pp)) {
                map[pp] = out.add_state();
            }
            return map[pp];
        }

        /// construct the output automaton
        static void run(const PDA &pda, NFA &nfa, PDA &out) throw() {

            NFA_REMOVE_EPSILON<AlphaT>::run(nfa);

            state_pair_map_type state_pairs;

            // assign the start state
            state_pairs[std::make_pair(
                pda.get_start_state(),
                nfa.get_start_state()
            )] = out.get_start_state();

            pda_state_type q;
            pda_state_type r;

            nfa_state_type p;
            nfa_state_type s;

            nfa_symbol_type a_nfa;

            pda_symbol_type a_pda;
            pda_symbol_type X;
            pda_symbol_type gamma;

            pda_symbol_type a_out;
            pda_symbol_type X_out;
            pda_symbol_type gamma_out;

            const pda_symbol_type epsilon(pda.epsilon());

            pda_generator_type pda_transitions(pda.search(
                ~q,
                ~a_pda,
                ~X,
                ~gamma,
                ~r
            ));

            nfa_generator_type nfa_transitions(nfa.search(
                ~p,
                a_nfa,
                ~s
            ));
            nfa_generator_type nfa_states(nfa.search(~s));
            nfa_generator_type *gen(0);

            // add in all of the main transitions
            for(; pda_transitions.match_next(); ) {

                // switch generators (because we really only care about s)
                // depending on whether the PDA is taking an epsilon
                // transition or not
                if(epsilon == a_pda) {
                    gen = &nfa_states;
                    a_out = out.epsilon();
                } else {
                    gen = &nfa_transitions;
                    a_nfa = nfa.get_symbol(pda.get_alpha(a_pda));
                    a_out = out.get_alphabet_symbol(pda.get_alpha(a_pda));
                }

                // translate the symbols to the new PDA

                if(pda.is_in_input_alphabet(X)) {
                    X_out = out.get_alphabet_symbol(pda.get_alpha(X));
                } else {
                    X_out = out.get_stack_symbol(pda.get_name(X));
                }

                if(pda.is_in_input_alphabet(gamma)) {
                    gamma_out = out.get_alphabet_symbol(pda.get_alpha(gamma));
                } else {
                    gamma_out = out.get_stack_symbol(pda.get_name(gamma));
                }

                // go over the NFA
                for(gen->rewind();
                    gen->match_next(); ) {

                    out.add_transition(
                        get_state(out, state_pairs, q, p),
                        a_out,
                        X_out,
                        gamma_out,
                        get_state(out, state_pairs, r, s)
                    );
                }
            }

            // collect the final states
            pda_generator_type pda_states(pda.search(~r));
            for(; pda_states.match_next(); ) {

                if(!pda.is_accept_state(r)) {
                    continue;
                }

                for(nfa_states.rewind(); nfa_states.match_next(); ) {
                    if(!nfa.is_accept_state(s)) {
                        continue;
                    }

                    out.add_accept_state(get_state(out, state_pairs, r, s));
                }
            }
        }

    };

}}

#endif /* FLTL_PDA_INTERSECT_NFA_HPP_ */
