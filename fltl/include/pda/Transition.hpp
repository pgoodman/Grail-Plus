/*
 * Transition.hpp
 *
 *  Created on: Feb 26, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_TRANSITION_HPP_
#define FLTL_TRANSITION_HPP_

namespace fltl { namespace pda {

    template <typename AlphaT>
    class Transition {
    private:

        typedef Transition<AlphaT> self_type;
        typedef Symbol<AlphaT> symbol_type;
        typedef OpaqueState<AlphaT> state_type;

        friend class PDA<AlphaT>;
        friend class OpaqueTransition<AlphaT>;
        friend class TransitionGenerator<AlphaT>;
        friend class Pattern<AlphaT>;

        template <
            typename, typename,
            typename, typename,
            typename, typename
        > friend class PatternGenerator;

        template <typename, typename, typename>
        friend class pda::detail::FindNextTransition;

        /// the symbol read from input
        symbol_type sym_read;

        /// the symbol that must be on the top of the stack for this
        /// transition to be followed. this symbol is popped off of
        /// the stack.
        symbol_type sym_pop;

        /// symbol to push onto the stack after the transition has been
        /// followed
        symbol_type sym_push;

        /// the next transition that shares the same source state.
        self_type *next;
        self_type *prev;

        /// source and sink states, respectively.
        state_type source_state;
        state_type sink_state;

        /// reference count
        unsigned ref_count;

        /// was this transition deleted?
        bool is_deleted;

        /// the PDA of this producton
        PDA<AlphaT> *pda;

        static void hold(self_type *trans) throw() {
            assert(0 != trans);
            ++(trans->ref_count);
        }

        static void release(self_type *trans) throw() {
            assert(0 != trans);
            if(0 == --(trans->ref_count)) {

                if(0 != trans->pda) {

                    if(0 != trans->next) {
                        trans->next->prev = trans->prev;
                    }

                    if(0 != trans->prev) {
                        trans->prev->next = trans->next;
                    } else {
                        trans->pda->state_transitions.get(
                            trans->source_state.id
                        ) = trans->next;
                    }
                }

                PDA<AlphaT>::transition_allocator->deallocate(trans);
            }
        }

    public:

        Transition(void) throw()
            : next(0)
            , prev(0)
            , ref_count(0)
            , is_deleted(false)
            , pda(0)
        { }

        ~Transition(void) throw() {
            pda = 0;
            next = 0;
            prev = 0;
        }
    };

}}

#endif /* FLTL_TRANSITION_HPP_ */
