/*
 * NFA.hpp
 *
 *  Created on: Mar 6, 2011
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

#ifndef FLTL_NFA_HPP_
#define FLTL_NFA_HPP_

#include "fltl/include/PDA.hpp"

#include "fltl/include/preprocessor/FORCE_INLINE.hpp"

#include "fltl/include/helper/UnsafeCast.hpp"
#include "fltl/include/trait/Alphabet.hpp"

namespace fltl {

    template <typename> class NFA;

    namespace nfa {
        template <typename> class OpaqueTransition;
    }
}

#include "fltl/include/nfa/OpaqueTransition.hpp"

#define FLTL_NFA_USE_TYPES_PREFIX_FUNC(type, prefix, func) \
    typedef type func(prefix, nfa_type); \
    typedef typename type::state_type func(prefix, state_type); \
    typedef typename type::symbol_type func(prefix, symbol_type); \
    typedef typename type::transition_type func(prefix, transition_type); \
    typedef typename type::generator_type func(prefix, generator_type); \
    typedef typename type::traits_type func(prefix, traits_type); \
    typedef typename type::alphabet_type func(prefix, alphabet_type); \
    typedef typename type::symbol_buffer_type func(prefix, symbol_buffer_type)

#define FLTL_NFA_NO_PREFIX(prefix, str) str
#define FLTL_NFA_USE_PREFIX(prefix, str) prefix ## _ ## str

#define FLTL_NFA_USE_TYPES(type) \
    FLTL_NFA_USE_TYPES_PREFIX_FUNC(type, foo, FLTL_NFA_NO_PREFIX)

#define FLTL_NFA_USE_TYPES_PREFIX(type, prefix) \
    FLTL_NFA_USE_TYPES_PREFIX_FUNC(type, prefix, FLTL_NFA_USE_PREFIX)

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
            PDA<AlphaT>::remove_transition(*helper::unsafe_cast<
                typename PDA<AlphaT>::transition_type *
            >(&trans));
        }

        //using PDA<AlphaT>::remove_transition;
        using PDA<AlphaT>::add_accept_state;
        using PDA<AlphaT>::remove_accept_state;
        using PDA<AlphaT>::is_accept_state;
        using PDA<AlphaT>::get_alpha;
        using PDA<AlphaT>::num_states;
        using PDA<AlphaT>::num_states_capacity;
        using PDA<AlphaT>::num_accept_states;
        using PDA<AlphaT>::num_symbols;
        using PDA<AlphaT>::num_transitions;
        using PDA<AlphaT>::is_in_input_alphabet;
        using PDA<AlphaT>::get_name;
        using PDA<AlphaT>::set_name;

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
