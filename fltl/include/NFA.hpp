/*
 * NFA.hpp
 *
 *  Created on: Mar 6, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_NFA_HPP_
#define FLTL_NFA_HPP_

#include "fltl/include/PDA.hpp"

#include "fltl/include/preprocessor/FORCE_INLINE.hpp"

#include "fltl/include/helper/UnsafeCast.hpp"

namespace fltl {

    template <typename> class NFA;

    namespace nfa {
        template <typename> class OpaqueTransition;
    }
}

#include "fltl/include/nfa/OpaqueTransition.hpp"

namespace fltl {

    /// represents a non-deterministic finite automaton
    /// for the sake of simplicity, it is implemented in terms of a PDA.
    template <typename AlphaT>
    class NFA : protected PDA<AlphaT> {
    public:

        typedef typename PDA<AlphaT>::traits_type traits_type;
        typedef typename PDA<AlphaT>::alphabet_type alphabet_type;

        typedef typename PDA<AlphaT>::state_t state_t;
        typedef typename PDA<AlphaT>::state_type state_type;

        typedef typename PDA<AlphaT>::sym_t sym_t;
        typedef typename PDA<AlphaT>::symbol_type symbol_type;

        typedef typename PDA<AlphaT>::generator_t generator_t;
        typedef typename PDA<AlphaT>::generator_type generator_type;

        typedef typename PDA<AlphaT>::sym_buff_t sym_buff_t;
        typedef typename PDA<AlphaT>::symbol_buffer_type symbol_buffer_type;

        typedef nfa::OpaqueTransition<AlphaT> transition_type;
        typedef transition_type trans_t;

        typedef NFA<AlphaT> self_type;

        using PDA<AlphaT>::_;

        NFA(void) throw()
            : PDA<AlphaT>()
        { }

        ~NFA(void) throw() { }

        inline const symbol_type
        get_symbol(const alphabet_type alpha) throw() {
            return this->get_alphabet_symbol(alpha);
        }

        using PDA<AlphaT>::add_state;
        using PDA<AlphaT>::add_start_state;
        using PDA<AlphaT>::set_start_state;
        using PDA<AlphaT>::get_start_state;
        using PDA<AlphaT>::epsilon;

        const transition_type add_transition(
            state_type source,
            symbol_type read,
            state_type sink
        ) throw() {
            return transition_type(PDA<AlphaT>::add_transition(
                source,
                read,
                this->epsilon(),
                this->epsilon(),
                sink
            ));
        }

        void remove_transition(transition_type trans) throw() {
            return PDA<AlphaT>::remove_transition(*helper::unsafe_cast<
                typename PDA<AlphaT>::transition_type *
            >(&trans));
        }

        //using PDA<AlphaT>::remove_transition;
        using PDA<AlphaT>::add_accept_state;
        using PDA<AlphaT>::remove_accept_state;
        using PDA<AlphaT>::is_accept_state;
        using PDA<AlphaT>::get_alpha;
        using PDA<AlphaT>::num_states;
        using PDA<AlphaT>::num_accept_states;
        using PDA<AlphaT>::num_symbols;
        using PDA<AlphaT>::num_transitions;

        /// generate all symbols
        FLTL_FORCE_INLINE generator_type
        search(pda::Unbound<AlphaT,pda::symbol_tag> sym) const throw() {
            return PDA<AlphaT>::search(sym);
        }

        /// generate all states
        FLTL_FORCE_INLINE generator_type
        search(
            pda::Unbound<AlphaT,pda::state_tag> state
        ) const throw() {
            return PDA<AlphaT>::search(state);
        }

        /// generate all transitions
        FLTL_FORCE_INLINE generator_type
        search(pda::Unbound<AlphaT,pda::transition_tag> trans) const throw() {
            return PDA<AlphaT>::search(trans);
        }

        /// search for a transition
        template <
            typename SourceT,
            typename ReadT,
            typename SinkT
        >
        FLTL_FORCE_INLINE generator_type
        search(
            const SourceT &source_state,
            const ReadT &read_symbol,
            const SinkT &sink_state
        ) const throw() {
            return PDA<AlphaT>::search(
                source_state,
                read_symbol,
                this->_,
                this->_,
                sink_state
            );
        }

        /// search for a transition
        template <
            typename SourceT,
            typename ReadT,
            typename SinkT
        >
        FLTL_FORCE_INLINE generator_type
        search(
            pda::Unbound<AlphaT,pda::transition_tag> trans,
            const SourceT &source_state,
            const ReadT &read_symbol,
            const SinkT &sink_state
        ) const throw() {
            return PDA<AlphaT>::search(
                trans,
                source_state,
                read_symbol,
                this->_,
                this->_,
                sink_state
            );
        }
    };
}

#endif /* FLTL_NFA_HPP_ */
