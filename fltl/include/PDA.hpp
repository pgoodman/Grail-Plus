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

#include "fltl/include/trait/Alphabet.hpp"

namespace fltl {

    namespace pda {
        template <typename> class Symbol;
        template <typename> class Transition;
        template <typename> class OpaqueState;
        template <typename> class OpaqueTransition;
    }

    /// represents a non-deterministic pushdown automaton
    template <typename AlphaT>
    class PDA {
    public:

        typedef trait::Alphabet<AlphaT> traits_type;
        typedef typename traits_type::alphabet_type alphabet_type;

        /// long forms of opauqe types
        typedef pda::OpaqueState<AlphaT> state_type;
        typedef pda::OpaqueTransition<AlphaT> transition_type;

        /// short forms
        typedef state_type state_t;
        typedef transition_type trans_t;

    private:

        /// bijective mapping between external alphabet elements and the
        /// symbols used to represent those alphabet elements.
        typedef std::map<
            alphabet_type,
            cfg::Symbol<AlphaT>,
            typename traits_type::less_type
        > symbol_map_inv_type;
        symbol_map_inv_type symbol_map_inv;
        helper::Array<alphabet_type> symbol_map;

        /// represents an adjacency list of all transitions leaving a
        /// particular state, keyed by a state id
        helper::Array<pda::Transition<AlphaT> *> state_transitions;

        /// the start state
        unsigned start_state;

        /// the final states
        std::set<unsigned> final_states;

        PDA(void) throw()
            : symbol_map_inv()
            , symbol_map(256U)
            , state_transitions(256U)
            , start_state(0U)
            , final_states()
        { }

        const state_type add_state(void) throw() {

        }

        void add_start_state(const state_type state) throw() {

        }


    };
}

#include "fltl/include/pda/Symbol.hpp"
#include "fltl/include/pda/Transition.hpp"
#include "fltl/include/pda/OpaqueState.hpp"
#include "fltl/include/pda/OpaqueTransition.hpp"

#endif /* FLTL_PDA_HPP_ */
