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
        template <typename> class SymbolBuffer;
        template <typename> class Transition;
        template <typename> class OpaqueState;
        template <typename> class OpaqueTransition;
    }
}

#include "fltl/include/pda/Symbol.hpp"
#include "fltl/include/pda/SymbolBuffer.hpp"
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
        typedef pda::SymbolBuffer<AlphaT> symbol_buffer_type;

        /// short forms
        typedef state_type state_t;
        typedef transition_type trans_t;
        typedef symbol_type sym_t;
        typedef symbol_buffer_type sym_buff_t;

    private:

        typedef std::map<
            alphabet_type,
            unsigned,
            typename traits_type::less_type
        > symbol_map_inv_type;

        /// bijective mapping between external alphabet elements and the
        /// symbols used to represent those alphabet elements.
        symbol_map_inv_type symbol_map_inv;

        helper::Array<std::pair<alphabet_type, const char *> > symbol_map;

        typedef std::map<
            const char *,
            unsigned,
            typename trait::Alphabet<const char *>::less_type
        > named_symbol_map_inv_type;

        /// bijective mapping between the
        named_symbol_map_inv_type named_symbol_map_inv;

        /// represents an adjacency list of all transitions leaving a
        /// particular state, keyed by a state id
        helper::Array<pda::Transition<AlphaT> *> state_transitions;

        /// the start state
        state_type start_state;

        /// the id of the next state to be created
        unsigned next_state_id;

        /// the id of the next symbol to be assigned
        unsigned next_symbol_id;

        /// the final states
        std::set<state_type> final_states;

        /// the upper bound for automatically created symbols
        mutable const char *auto_symbol_upper_bound;

        /// transition allocator
        static helper::BlockAllocator<
            pda::Transition<AlphaT>
        > transition_allocator;

    public:

        /// constructor
        PDA(void) throw()
            : symbol_map_inv()
            , symbol_map(256U)
            , state_transitions(256U)
            , start_state()
            , next_state_id(1U)
            , next_symbol_id(1U)
            , final_states()
        {
            static const char * const UB("$0");
            auto_symbol_upper_bound = UB;

            state_transitions.append(0);
            symbol_map.append(std::make_pair<alphabet_type, const char *>(
                mpl::Static<alphabet_type>::VALUE,
                0
            ));

            start_state = add_state();
        }

        ~PDA(void) throw() {

            // clean up transitions
            for(unsigned i(1); i < next_state_id; ++i) {
                pda::Transition<AlphaT> *&trans(state_transitions.get(i));
                if(0 != trans) {
                    pda::Transition<AlphaT>::release(trans);
                    trans = 0;
                }
            }

            // clean up symbols
            for(unsigned i(1); i < next_symbol_id; ++i) {
                std::pair<alphabet_type, const char *> &pp(
                    symbol_map.get(i)
                );
                traits_type::destroy(pp.first);
                trait::Alphabet<const char *>::destroy(pp.second);
            }
        }

        /// get the symbol representation for an element of the alphabet
        const symbol_type get_alphabet_symbol(const alphabet_type alpha) throw() {
            typename symbol_map_inv_type::iterator pos(
                symbol_map_inv.find(alpha)
            );
            unsigned alpha_id;

            // add in the terminal
            if(symbol_map_inv.end() == pos) {

                alpha_id = next_symbol_id;
                ++next_symbol_id;
                alphabet_type copy(traits_type::copy(alpha));
                symbol_map.append(std::make_pair<alphabet_type,const char *>(
                    copy, 0
                ));
                symbol_map_inv[copy] = alpha_id;

            // return the terminal
            } else {
                alpha_id = (*pos).second;
            }

            return symbol_type(alpha_id);
        }

        /// create a special stack symbol
        const symbol_type add_stack_symbol(void) throw() {

            char buffer[1024] = {'\0'};

            unsigned long prev_ub(strtoul(
                &(auto_symbol_upper_bound[1]),
                0,
                10
            ));

            // make the new name
            sprintf(buffer, "$%lu", prev_ub + 1);
            const char *name(trait::Alphabet<const char *>::copy(buffer));
            auto_symbol_upper_bound = name;

            unsigned alpha_id(next_symbol_id);
            ++next_symbol_id;

            symbol_map.append(std::make_pair(
                mpl::Static<alphabet_type>::VALUE, name
            ));

            return symbol_type(alpha_id);
        }

        /// get a stack symbol by its name, or add one if it doesn't exist.
        /// this assumes that the stack symbol
        const symbol_type get_stack_symbol(const char *name) throw() {
            assert(0 != name);
            assert('\0' != *name);

            typename named_symbol_map_inv_type::iterator pos(
                named_symbol_map_inv.find(name)
            );

            unsigned alpha_id(0);

            // need to add it in
            if(pos == named_symbol_map_inv.end()) {
                alpha_id = next_symbol_id;
                ++next_symbol_id;

                const char *name_copy(trait::Alphabet<const char *>::copy(name));

                if('$' == *name_copy) {
                    if(0 < strcmp(name, auto_symbol_upper_bound)) {
                        auto_symbol_upper_bound = name_copy;
                    }
                }

                named_symbol_map_inv[name_copy] = alpha_id;
                symbol_map.append(std::make_pair(
                    mpl::Static<alphabet_type>::VALUE,
                    name_copy
                ));

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
            state_transitions.get(id) = 0;
            return state_type(id);
        }

        /// add a start state to this automaton. this allows multiple start
        /// states. the semantics of this is that there is only one real
        /// start state and there are epsilon transitions from that start
        /// state to all other "start" states. this allows the multiple
        /// start states to behave as a union of PDAs.
        void add_start_state(const state_type state) throw() {

            assert(0U < state.id);
            assert(state.id < next_state_id);

            if(state == start_state) {
                return;
            }

            // add in an epsilon transition from the start state to the
            // current state

            pda::Transition<AlphaT> *trans(make_transition(
                start_state,
                epsilon()
            ));

            trans->sink_state = state;
            pda::Transition<AlphaT>::hold(trans);
        }

        /// get the start state
        const state_type get_start_state(void) throw() {
            return start_state;
        }

        /// return the epsilon symbol
        const symbol_type epsilon(void) const throw() {
            return mpl::Static<symbol_type>::VALUE;
        }

        /// add a transition
        const transition_type add_transition(
            state_type source,
            symbol_type read,
            symbol_type pop,
            symbol_type push,
            state_type sink
        ) throw() {

            assert(0U != source.id);
            assert(0U != sink.id);
            assert(source.id < next_state_id);
            assert(sink.id < next_state_id);

            assert(read.id < next_symbol_id);
            assert(push.id < next_symbol_id);
            assert(pop.id < next_symbol_id);

            assert(is_in_input_alphabet(read));

            pda::Transition<AlphaT> *trans(make_transition(source, read));
            trans->sink_state = sink;
            trans->sym_pop = pop;
            trans->sym_push = push;

            pda::Transition<AlphaT>::hold(trans);

            return transition_type(trans);
        }

        /// add a transition that pushes multiple symbols onto the stack
        const void add_transition(
            state_type source,
            symbol_type read,
            symbol_type pop,
            const symbol_buffer_type &push_symbols,
            state_type sink
        ) throw() {

            assert(is_in_input_alphabet(read));

            unsigned num_syms(push_symbols.size());

            // simple epsilon transition
            if(0 == num_syms) {
                add_transition(source, read, pop, epsilon(), sink);
                return;

            // normal transition
            } else if(1 == num_syms) {
                add_transition(
                    source, read, pop, push_symbols.symbol_at(0), sink
                );
                return;
            }

            assert(0U != source.id);
            assert(0U != sink.id);
            assert(source.id < next_state_id);
            assert(sink.id < next_state_id);

            assert(read.id < next_symbol_id);
            assert(pop.id < next_symbol_id);

            for(; 0 != num_syms; --num_syms) {

                add_transition(
                    source,
                    read,
                    pop,
                    push_symbols.symbol_at(num_syms - 1),
                    0 == num_syms ? sink : add_state()
                );

                read = epsilon();
                pop = epsilon();
            }
        }

        /// add an accepting state
        void add_accept_state(state_type state) throw() {
            final_states.insert(state);
        }

        /// remove an accept state
        void remove_accept_state(state_type state) throw() {
            final_states.erase(state);
        }

        /// is a state an accepting state
        bool is_accept_state(state_type state) throw() {
            return 0 != final_states.count(state);
        }

        /// check whether a symbol is part of the input alphabet
        bool is_in_input_alphabet(symbol_type sym) throw() {
            assert(sym.id < next_symbol_id);
            return 0 == symbol_map.get(sym.id).second;
        }

    private:

        /// allocate a transition and link it in to the adjacency list
        pda::Transition<AlphaT> *make_transition(
            state_type source_state,
            symbol_type read
        ) throw() {

            pda::Transition<AlphaT> *trans(transition_allocator.allocate());

            trans->source_state = source_state;
            trans->sym_read = read;

            pda::Transition<AlphaT> *curr(state_transitions.get(
                source_state.id
            ));

            // no transitions on this state yet
            if(0 == curr) {
                state_transitions.get(source_state.id) = trans;
                return trans;
            }

            // add in the transition using insertion sort
            for(pda::Transition<AlphaT> *prev(0);
                0 != curr;
                prev = curr, curr = curr->next) {

                if(trans->sym_read.id <= curr->sym_read.id) {

                    trans->next = curr;
                    trans->prev = curr->prev;
                    curr->prev = trans;

                    if(0 != prev) {
                        prev->next = trans;
                    }

                    break;
                }
            }

            return trans;
        }

    };

    // static initialize
    template <typename AlphaT>
    helper::BlockAllocator<
        pda::Transition<AlphaT>
    > PDA<AlphaT>::transition_allocator;
}

#endif /* FLTL_PDA_HPP_ */
