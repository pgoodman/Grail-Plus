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
#include <cstring>

#include "fltl/include/helper/Array.hpp"
#include "fltl/include/helper/BlockAllocator.hpp"
#include "fltl/include/helper/StorageChain.hpp"
#include "fltl/include/helper/UnsafeCast.hpp"

#include "fltl/include/trait/Alphabet.hpp"
#include "fltl/include/trait/Uncopyable.hpp"

#include "fltl/include/mpl/Static.hpp"
#include "fltl/include/mpl/If.hpp"
#include "fltl/include/mpl/Or.hpp"

#include "fltl/include/preprocessor/FORCE_INLINE.hpp"

namespace fltl {

    // forward delarations

    template <typename> class PDA;

    namespace pda {
        template <typename> class Symbol;
        template <typename> class SymbolBuffer;
        template <typename> class Transition;
        template <typename> class OpaqueState;
        template <typename> class OpaqueTransition;
        template <typename> class Generator;

        template <typename> class StateGenerator;
        template <typename> class SymbolGenerator;
        template <typename> class TransitionGenerator;
        template <
            typename, typename,
            typename, typename,
            typename, typename
        > class PatternGenerator;
        template <typename> class Pattern;

        template <typename, typename> class Unbound { };

        namespace pattern {
            template <typename, typename> class Bind;
            template <typename> class Init;
        }

        namespace detail {
            template <typename, typename> class ResetPatternGenerator;
            template <typename, typename, typename> class FindNextTransition;
        }

        class symbol_tag { };
        class unbound_symbol_tag { };

        class state_tag { };
        class unbound_state_tag { };

        class transition_tag { };
        class unbound_transition_tag { };

        /// represents any state or symbol.
        class any_state_or_symbol_tag {
        public:
            typedef any_state_or_symbol_tag tag_type;
        };
    }
}

#include "fltl/include/pda/Symbol.hpp"
#include "fltl/include/pda/SymbolBuffer.hpp"
#include "fltl/include/pda/Transition.hpp"
#include "fltl/include/pda/OpaqueState.hpp"
#include "fltl/include/pda/OpaqueTransition.hpp"
#include "fltl/include/pda/Unbound.hpp"
#include "fltl/include/pda/Generator.hpp"
#include "fltl/include/pda/Pattern.hpp"

namespace fltl {

    /// represents a non-deterministic pushdown automaton
    template <typename AlphaT>
    class PDA : protected trait::Uncopyable {
    public:

        friend class pda::Transition<AlphaT>;
        friend class pda::SymbolGenerator<AlphaT>;
        friend class pda::StateGenerator<AlphaT>;
        friend class pda::TransitionGenerator<AlphaT>;
        friend class pda::Pattern<AlphaT>;

        template <
            typename, typename,
            typename, typename,
            typename, typename
        > friend class pda::PatternGenerator;

        template <typename, typename>
        friend class pda::detail::ResetPatternGenerator;

        template <typename, typename, typename>
        friend class pda::detail::FindNextTransition;

        typedef trait::Alphabet<AlphaT> traits_type;
        typedef typename traits_type::alphabet_type alphabet_type;

        /// long forms of opauqe types
        typedef pda::OpaqueState<AlphaT> state_type;
        typedef pda::OpaqueTransition<AlphaT> transition_type;
        typedef pda::Symbol<AlphaT> symbol_type;
        typedef pda::SymbolBuffer<AlphaT> symbol_buffer_type;
        typedef pda::Generator<AlphaT> generator_type;

        /// short forms
        typedef state_type state_t;
        typedef transition_type trans_t;
        typedef symbol_type sym_t;
        typedef symbol_buffer_type sym_buff_t;
        typedef generator_type generator_t;

        typedef PDA<AlphaT> self_type;

    private:

        /// forward declaration of pattern checker
        template <
            typename SourceT,
            typename ReadT,
            typename PopT,
            typename PushT,
            typename SinkT
        >
        class PatternIsValid;

        typedef std::map<
            alphabet_type,
            unsigned,
            typename traits_type::less_type
        > symbol_map_inv_type;

        /// bijective mapping between external alphabet elements and the
        /// symbols used to represent those alphabet elements.
        symbol_map_inv_type symbol_map_inv;

        mutable helper::Array<
            std::pair<alphabet_type, const char *>
        > symbol_map;

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

        /// the number of transitions
        unsigned num_transitions_;

        /// the final states
        std::set<state_type> final_states;

        /// the upper bound for automatically created symbols
        mutable const char *auto_symbol_upper_bound;

        /// the first transition of this PDA
        pda::Transition<AlphaT> *first_transition;

        /// transition allocator
        static helper::StorageChain<helper::BlockAllocator<
            pda::Transition<AlphaT>
        > > transition_allocator;

        /// pattern allocator
        static helper::StorageChain<helper::BlockAllocator<
            pda::Pattern<AlphaT>
        > > pattern_allocator;

    public:

        /// represents any state or symbol
        const pda::any_state_or_symbol_tag _;

        /// constructor
        PDA(void) throw()
            : symbol_map_inv()
            , symbol_map(256U)
            , state_transitions(256U)
            , start_state()
            , next_state_id(0U)
            , next_symbol_id(1U)
            , num_transitions_(0U)
            , final_states()
            , first_transition(0)
            , _()
        {
            static const char * const UB("$0");
            static const char * const EPSILON("epsilon");

            auto_symbol_upper_bound = UB;

            symbol_map.append(std::make_pair<alphabet_type, const char *>(
                mpl::Static<alphabet_type>::VALUE,
                EPSILON
            ));

            start_state = add_state();
        }

        /// destructor
        ~PDA(void) throw() {

            // clean up transitions
            for(unsigned i(0); i < next_state_id; ++i) {
                pda::Transition<AlphaT> *&trans(state_transitions.get(i));

                for(pda::Transition<AlphaT> *curr(trans), *next(0);
                    0 != curr;
                    curr = next) {

                    next = curr->next;

                    // !!! could be some memory issues if a transition
                    //     manages to live longer than its PDA
                    curr->pda = 0;
                    if(!curr->is_deleted) {
                        pda::Transition<AlphaT>::release(curr);
                    }
                }

                trans = 0;
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
            unsigned alpha_id(0);

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
            state_transitions.set_size(id + 1U);
            state_transitions.get(id) = 0;
            return state_type(id);
        }

        /// add a start state to this automaton. this allows multiple start
        /// states. the semantics of this is that there is only one real
        /// start state and there are epsilon transitions from that start
        /// state to all other "start" states. this allows the multiple
        /// start states to behave as a union of PDAs.
        void add_start_state(const state_type state) throw() {

            assert(state.id < next_state_id);

            if(state == start_state) {
                return;
            }

            // add in an epsilon transition from the start state to the
            // current state

            pda::Transition<AlphaT> *trans(make_transition(
                start_state,
                epsilon(),
                epsilon(),
                epsilon(),
                state
            ));

            pda::Transition<AlphaT>::hold(trans);
        }

        /// change the start state
        void set_start_state(const state_type state) throw() {
            assert(state.id < next_state_id);
            start_state = state;
        }

        /// get the start state
        const state_type &get_start_state(void) const throw() {
            return start_state;
        }

        /// return the epsilon symbol
        const symbol_type &epsilon(void) const throw() {
            return mpl::Static<symbol_type>::VALUE;
        }

        /// add a transition. note: transitions are ordered according to their
        /// input symbol and so this effectively performs an insertion sort.
        /// also note: a separate ordering from the input alphabet ordering
        /// is imposed on symbols.
        const transition_type add_transition(
            state_type source,
            symbol_type read,
            symbol_type pop,
            symbol_type push,
            state_type sink
        ) throw() {

            assert(source.id < next_state_id);
            assert(sink.id < next_state_id);

            assert(read.id < next_symbol_id);
            assert(push.id < next_symbol_id);
            assert(pop.id < next_symbol_id);

            assert(0U == read.id || is_in_input_alphabet(read));

            pda::Transition<AlphaT> *trans(make_transition(
                source,
                read,
                pop,
                push,
                sink
            ));

            pda::Transition<AlphaT>::hold(trans);

            ++num_transitions_;

            return transition_type(trans);
        }

        /// add a transition that pushes multiple symbols onto the stack
        void add_transition(
            state_type source,
            symbol_type read,
            symbol_type pop,
            const symbol_buffer_type &push_symbols,
            state_type sink
        ) throw() {

            assert(0U == read.id || is_in_input_alphabet(read));

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

            assert(source.id < next_state_id);
            assert(sink.id < next_state_id);

            assert(read.id < next_symbol_id);
            assert(pop.id < next_symbol_id);

            for(; 0 != num_syms; --num_syms) {

                const state_type curr_sink(1 == num_syms ? sink : add_state());

                add_transition(
                    source,
                    read,
                    pop,
                    push_symbols.symbol_at(num_syms - 1),
                    curr_sink
                );

                source = curr_sink;
                read = epsilon();
                pop = epsilon();
            }
        }

        /// delete a transition
        void remove_transition(transition_type trans_) throw() {
            assert(0 != trans_.transition);
            assert(!trans_.transition->is_deleted);

            pda::Transition<AlphaT> *trans(trans_.transition);
            trans->is_deleted = true;

            // go find the next transition
            if(first_transition == trans) {
                first_transition = pda::TransitionGenerator<
                    AlphaT
                >::find_next_transition(this, trans);
            }

            --num_transitions_;

            pda::Transition<AlphaT>::release(trans);
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
        bool is_accept_state(state_type state) const throw() {
            return 0 != final_states.count(state);
        }

        /// check whether a symbol is part of the input alphabet
        bool is_in_input_alphabet(symbol_type sym) const throw() {
            assert(sym.id < next_symbol_id);
            return 0 == symbol_map.get(sym.id).second;
        }

        /// get the alphabetic representation of an input symbol
        alphabet_type get_alpha(symbol_type sym) const throw() {
            assert(0U != sym.id);
            assert(is_in_input_alphabet(sym));
            return symbol_map.get(sym.id).first;
        }

        /// get the name of a stack symbol
        const char *get_name(symbol_type sym) const throw() {
            assert(!is_in_input_alphabet(sym));
            return symbol_map.get(sym.id).second;
        }

        unsigned num_states(void) const throw() {
            return state_transitions.size();
        }

        unsigned num_accept_states(void) const throw() {
            return final_states.size();
        }

        unsigned num_symbols(void) const throw() {
            return symbol_map.size() - 1;
        }

        unsigned num_transitions(void) const throw() {
            return num_transitions_;
        }

        /// generate all symbols
        inline generator_type
        search(pda::Unbound<AlphaT,pda::symbol_tag> sym) const throw() {
            return pda::Generator<AlphaT>(
                reinterpret_cast<void *>(sym.symbol),
                &(pda::SymbolGenerator<AlphaT>::next),
                &(pda::SymbolGenerator<AlphaT>::reset),
                &(pda::SymbolGenerator<AlphaT>::free),
                const_cast<self_type *>(this),
                0
            );
        }

        /// generate all states
        inline generator_type
        search(
            pda::Unbound<AlphaT,pda::state_tag> state
        ) const throw() {
            return pda::Generator<AlphaT>(
                reinterpret_cast<void *>(state.state),
                &(pda::StateGenerator<AlphaT>::next),
                &(pda::StateGenerator<AlphaT>::reset),
                &(pda::StateGenerator<AlphaT>::free),
                const_cast<self_type *>(this),
                0
            );
        }

        /// generate all transitions
        inline generator_type
        search(pda::Unbound<AlphaT,pda::transition_tag> trans) const throw() {
            return pda::Generator<AlphaT>(
                reinterpret_cast<void *>(trans.transition),
                &(pda::TransitionGenerator<AlphaT>::next),
                &(pda::TransitionGenerator<AlphaT>::reset),
                &(pda::TransitionGenerator<AlphaT>::free),
                const_cast<self_type *>(this),
                0
            );
        }

        /// search for a transition
        template <
            typename SourceT,
            typename ReadT,
            typename PopT,
            typename PushT,
            typename SinkT
        >
        inline generator_type
        search(
            const SourceT &source_state,
            const ReadT &read_symbol,
            const PopT &pop_symbol,
            const PushT &push_symbol,
            const SinkT &sink_state
        ) const throw() {
            return make_search(
                source_state,
                read_symbol,
                pop_symbol,
                push_symbol,
                sink_state,
                0
            );
        }

        /// search for a transition
        template <
            typename SourceT,
            typename ReadT,
            typename PopT,
            typename PushT,
            typename SinkT
        >
        inline generator_type
        search(
            pda::Unbound<AlphaT,pda::transition_tag> trans,
            const SourceT &source_state,
            const ReadT &read_symbol,
            const PopT &pop_symbol,
            const PushT &push_symbol,
            const SinkT &sink_state
        ) const throw() {
            return make_search(
                source_state,
                read_symbol,
                pop_symbol,
                push_symbol,
                sink_state,
                trans.transition
            );
        }

    private:

        /// construct the generator needed for a transition pattern search
        template <
            typename SourceT,
            typename ReadT,
            typename PopT,
            typename PushT,
            typename SinkT
        >
        inline generator_type
        make_search(
            const SourceT &source_state,
            const ReadT &read_symbol,
            const PopT &pop_symbol,
            const PushT &push_symbol,
            const SinkT &sink_state,
            transition_type *trans
        ) const throw() {

            (void) sizeof(char[
                PatternIsValid<SourceT,ReadT,PopT,PushT,SinkT>::RESULT
            ]);

            // initialize the pattern
            pda::Pattern<AlphaT> *pattern(pattern_allocator->allocate());
            pda::pattern::Init<AlphaT>::init(&source_state, &(pattern->source));
            pda::pattern::Init<AlphaT>::init(&read_symbol, &(pattern->read));
            pda::pattern::Init<AlphaT>::init(&pop_symbol, &(pattern->pop));
            pda::pattern::Init<AlphaT>::init(&push_symbol, &(pattern->push));
            pda::pattern::Init<AlphaT>::init(&sink_state, &(pattern->sink));

            // pattern gen type
            typedef pda::PatternGenerator<
                AlphaT,
                typename SourceT::tag_type,
                typename ReadT::tag_type,
                typename PopT::tag_type,
                typename PushT::tag_type,
                typename SinkT::tag_type
            > pattern_generator_type;

            // return the generator
            return generator_type(
                reinterpret_cast<void *>(trans), // binder
                &(pattern_generator_type::next),
                &(pattern_generator_type::reset),
                &(pattern_generator_type::free),
                const_cast<self_type *>(this),
                pattern
            );
        }

        /// template class for checking the validity of the types of the
        /// parameters passed to a pattern search
        template <
            typename SourceT,
            typename ReadT,
            typename PopT,
            typename PushT,
            typename SinkT
        >
        class PatternIsValid {
        public:
            enum {
                SOURCE_VALID = mpl::Or<
                    mpl::IfTypesEqual<SourceT,pda::Unbound<AlphaT,pda::state_tag> >,
                    mpl::IfTypesEqual<SourceT,state_type>,
                    mpl::IfTypesEqual<SourceT,pda::any_state_or_symbol_tag>
                >::RESULT,

                READ_VALID = mpl::Or<
                    mpl::IfTypesEqual<ReadT,pda::Unbound<AlphaT,pda::symbol_tag> >,
                    mpl::IfTypesEqual<ReadT,symbol_type>,
                    mpl::IfTypesEqual<ReadT,pda::any_state_or_symbol_tag>
                >::RESULT,

                POP_VALID = mpl::Or<
                    mpl::IfTypesEqual<PopT,pda::Unbound<AlphaT,pda::symbol_tag> >,
                    mpl::IfTypesEqual<PopT,symbol_type>,
                    mpl::IfTypesEqual<PopT,pda::any_state_or_symbol_tag>
                >::RESULT,

                PUSH_VALID = mpl::Or<
                    mpl::IfTypesEqual<PushT,pda::Unbound<AlphaT,pda::symbol_tag> >,
                    mpl::IfTypesEqual<PushT,symbol_type>,
                    mpl::IfTypesEqual<PushT,pda::any_state_or_symbol_tag>
                >::RESULT,

                SINK_VALID = mpl::Or<
                    mpl::IfTypesEqual<SinkT,pda::Unbound<AlphaT,pda::state_tag> >,
                    mpl::IfTypesEqual<SinkT,state_type>,
                    mpl::IfTypesEqual<SinkT,pda::any_state_or_symbol_tag>
                >::RESULT,

                RESULT = 5 == (
                    SOURCE_VALID + READ_VALID +
                    POP_VALID + PUSH_VALID + SINK_VALID
                ) ? 1 : 0
            };
        };

        /// allocate a transition and link it in to the adjacency list
        pda::Transition<AlphaT> *make_transition(
            state_type source_state,
            symbol_type read,
            symbol_type pop,
            symbol_type push,
            state_type sink_state
        ) throw() {

            pda::Transition<AlphaT> *trans(transition_allocator->allocate());

            trans->source_state = source_state;
            trans->sym_read = read;
            trans->sym_pop = pop;
            trans->sym_push = push;
            trans->sink_state = sink_state;
            trans->pda = this;

            pda::Transition<AlphaT> *prev(0);
            pda::Transition<AlphaT> *curr(state_transitions.get(
                source_state.id
            ));

            // no transitions on this state yet
            if(0 == curr) {
                state_transitions.get(source_state.id) = trans;
                goto done;
            }

            // add in the transition using insertion sort
            for(;
                0 != curr;
                prev = curr, curr = curr->next) {

                // go as far as until we need to add something in
                if(read > curr->sym_read) {
                    continue;
                } else if(pop > curr->sym_pop) {
                    continue;
                } else if(push > curr->sym_push) {
                    continue;
                } else if(sink_state > curr->sink_state) {
                    continue;
                }

                if(*trans == *curr) {

                    // undelete
                    if(curr->is_deleted) {
                        curr->is_deleted = false;
                    }

                    transition_allocator->deallocate(trans);
                    trans = curr;

                    goto done;
                }

                trans->next = curr;
                trans->prev = prev;

                curr->prev = trans;

                if(0 != prev) {
                    prev->next = trans;
                } else {
                    state_transitions.get(source_state.id) = trans;
                }

                goto done;
            }

            // put this transition at the end of the list

            assert(0 != prev);

            prev->next = trans;
            trans->prev = prev;

        done:

            if(0 == first_transition) {
                first_transition = trans;
            } else if(*trans < *first_transition) {
                first_transition = trans;
            }

            return trans;
        }

    };

    // static initialize
    template <typename AlphaT>
    helper::StorageChain<helper::BlockAllocator<
        pda::Pattern<AlphaT>
    > > PDA<AlphaT>::pattern_allocator;

    template <typename AlphaT>
    helper::StorageChain<helper::BlockAllocator<
        pda::Transition<AlphaT>
    > > PDA<AlphaT>::transition_allocator(pattern_allocator);
}

#endif /* FLTL_PDA_HPP_ */
