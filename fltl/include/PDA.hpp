/*
 * PDA.hpp
 *
 *  Created on: Feb 26, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_PDA_HPP_
#define FLTL_PDA_HPP_

#include <map>
#include <set>
#include <cassert>

#include "fltl/include/helper/Array.hpp"
#include "fltl/include/helper/BlockAllocator.hpp"

#include "fltl/include/trait/Alphabet.hpp"
#include "fltl/include/trait/Uncopyable.hpp"

namespace fltl {

    // forward delaration
    template <typename> class PDA;

    namespace pda {
        template <typename> class Symbol;
        template <typename> class Transition;
        template <typename> class OpaqueState;
        template <typename> class OpaqueTransition;
    }
}

#include "fltl/include/pda/Symbol.hpp"
#include "fltl/include/pda/Transition.hpp"
#include "fltl/include/pda/OpaqueState.hpp"
#include "fltl/include/pda/OpaqueTransition.hpp"

namespace fltl {

    /// represents a non-deterministic pushdown automaton
    template <typename AlphaT>
    class PDA : protected trait::Uncopyable {
    public:

        friend class pda::Transition<AlphaT>;

        typedef trait::Alphabet<AlphaT> traits_type;
        typedef typename traits_type::alphabet_type alphabet_type;

        /// long forms of opauqe types
        typedef pda::OpaqueState<AlphaT> state_type;
        typedef pda::OpaqueTransition<AlphaT> transition_type;
        typedef pda::Symbol<AlphaT> symbol_type;

        /// short forms
        typedef state_type state_t;
        typedef transition_type trans_t;

    private:


        typedef std::map<
            alphabet_type,
            unsigned,
            typename traits_type::less_type
        > symbol_map_inv_type;

        /// bijective mapping between external alphabet elements and the
        /// symbols used to represent those alphabet elements.
        symbol_map_inv_type symbol_map_inv;

        helper::Array<alphabet_type> symbol_map;

        /// represents an adjacency list of all transitions leaving a
        /// particular state, keyed by a state id
        helper::Array<pda::Transition<AlphaT> *> state_transitions;

        /// the start state
        unsigned start_state_id;

        /// the id of the next state to be created
        unsigned next_state_id;

        /// the id of the next symbol to be assigned
        unsigned next_symbol_id;

        /// the final states
        std::set<unsigned> final_states;

        /// transition allocator
        static helper::BlockAllocator<
            pda::Transition<AlphaT>
        > transition_allocator;

        /// constructor
        PDA(void) throw()
            : symbol_map_inv()
            , symbol_map(256U)
            , state_transitions(256U)
            , start_state_id(0U)
            , next_state_id(1U)
            , next_symbol_id(1U)
            , final_states()
        {
            state_transitions.append(0);
            symbol_map.append(mpl::Static<alphabet_type>::VALUE);
        }

        /// get the symbol representation for an element of the alphabet
        const symbol_type get_symbol(const alphabet_type alpha) throw() {
            typename symbol_map_inv_type::iterator pos(
                symbol_map_inv.find(alpha)
            );
            unsigned alpha_id;

            // add in the terminal
            if(symbol_map_inv.end() == pos) {
                alpha_id = next_symbol_id;
                ++next_symbol_id;
                alphabet_type copy(traits_type::copy(alpha));
                symbol_map.append(std::make_pair<AlphaT,const char *>(
                    copy, 0
                ));
                symbol_map_inv[copy] = alpha_id;

            // return the terminal
            } else {
                alpha_id = (*pos).second;
            }

            return symbol_type(alpha_id);
        }

        /// add a state to this automaton
        const state_type add_state(void) throw() {
            unsigned id(next_state_id);
            ++next_state_id;

            state_transitions.reserve(id + 1U, true);
        }

        /// add a start state to this automaton
        void add_start_state(const state_type state) throw() {
            assert(0U < state.id);

            // no start state set yet
            if(0 == start_state_id) {
                start_state_id = state.id;
            } else {
                // TODO add epsilon transition
            }
        }
    };

    // static initialize
    template <typename AlphaT>
    helper::BlockAllocator<
        pda::Transition<AlphaT>
    > PDA<AlphaT>::transition_allocator;
}

#endif /* FLTL_PDA_HPP_ */
