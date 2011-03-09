/*
 * Generator.hpp
 *
 *  Created on: Mar 2, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_PDA_GENERATOR_HPP_
#define FLTL_PDA_GENERATOR_HPP_

namespace fltl { namespace pda {

    namespace def {
        template <typename AlphaT>
        bool next(Generator<AlphaT> *) throw() {
            return false;
        }

        template <typename AlphaT>
        void reset(Generator<AlphaT> *) throw() { }

        template <typename AlphaT>
        void free(Generator<AlphaT> *) throw() { }
    }

    /// implementation of generator for states
    template <typename AlphaT>
    class StateGenerator {
    private:

        friend class PDA<AlphaT>;

        static bool next(Generator<AlphaT> *gen) throw() {
            const unsigned offset(gen->cursor.state_offset);
            ++(gen->cursor.state_offset);

            OpaqueState<AlphaT> *binder(helper::unsafe_cast<
                OpaqueState<AlphaT> *
            >(gen->binder));

            if(offset >= gen->pda->num_states()) {
                binder->id = 0U;
                return false;
            }

            binder->id = offset;
            return true;
        }

        static void reset(Generator<AlphaT> *gen) throw() {
            gen->cursor.state_offset = 0U;
        }

        static void free(Generator<AlphaT> *) throw() { }
    };

    /// implementation of generator for symbols
    template <typename AlphaT>
    class SymbolGenerator {
    private:

        friend class PDA<AlphaT>;

        static bool next(Generator<AlphaT> *gen) throw() {
            const unsigned offset(gen->cursor.symbol_offset);
            ++(gen->cursor.symbol_offset);

            Symbol<AlphaT> *binder(helper::unsafe_cast<
                Symbol<AlphaT> *
            >(gen->binder));

            if(offset >= gen->pda->num_symbols()) {
                binder->id = 0U;
                return false;
            }

            binder->id = offset;
            return true;
        }

        static void reset(Generator<AlphaT> *gen) throw() {
            gen->cursor.symbol_offset = 0U;
        }

        static void free(Generator<AlphaT> *) throw() { }
    };

    /// implementation of generator for transitions
    template <typename AlphaT>
    class TransitionGenerator {
    private:

        friend class PDA<AlphaT>;

        template <
            typename, typename,
            typename, typename,
            typename, typename
        > friend class PatternGenerator;

        template <typename, typename>
        friend class detail::ResetPatternGenerator;

        template <typename, typename, typename>
        friend class detail::FindNextTransition;

        static Transition<AlphaT> *find_next_transition(
            PDA<AlphaT> *pda,
            Transition<AlphaT> *curr
        ) throw() {
            assert(0 != curr);

            Transition<AlphaT> *next(curr->next);
            unsigned offset(curr->source_state.id);
            unsigned num_states(pda->num_states());

        check_next:

            // go find the next non-null transition
            while(0 == next) {
                ++offset;

                if(offset >= num_states) {
                    return 0;
                } else {
                    next = pda->state_transitions.get(offset);
                }
            }

            if(next->is_deleted) {
                next = next->next;
                goto check_next;
            }

            return next;
        }

        static bool next(Generator<AlphaT> *gen) throw() {

            OpaqueTransition<AlphaT> *binder(helper::unsafe_cast<
                OpaqueTransition<AlphaT> *
            >(gen->binder));

            Transition<AlphaT> *old_trans(gen->cursor.transition);
            Transition<AlphaT> *orig_trans(old_trans);

            if(0 == orig_trans) {
                unbind(gen);
                return false;
            }

            if(old_trans->is_deleted) {
                old_trans = find_next_transition(
                    gen->pda,
                    old_trans
                );
            }

            binder->assign(old_trans);
            Transition<AlphaT>::release(orig_trans);

            if(0 == old_trans) {
                gen->cursor.transition = 0;
                unbind(gen);
                return false;
            }

            // try to find the next transition
            gen->cursor.transition = find_next_transition(
                gen->pda,
                old_trans
            );

            // hold on to the next transition
            if(0 != gen->cursor.transition) {
                Transition<AlphaT>::hold(gen->cursor.transition);
            }

            return true;
        }

        static void reset(Generator<AlphaT> *gen) throw() {

            free(gen);

            gen->cursor.transition = gen->pda->first_transition;
            if(0 != gen->cursor.transition) {
                Transition<AlphaT>::hold(gen->cursor.transition);
            }
        }

        static void free(Generator<AlphaT> *gen) throw() {
            if(0 != gen->cursor.transition) {
                Transition<AlphaT>::release(gen->cursor.transition);
                gen->cursor.transition = 0;
            }

            unbind(gen);
        }

        inline static void unbind(Generator<AlphaT> *gen) throw() {
            if(0 != gen->binder) {
                helper::unsafe_cast<
                    OpaqueTransition<AlphaT> *
                >(gen->binder)->assign(0);
            }
        }
    };

    namespace detail {

        template <typename AlphaT, typename TagT>
        class ResetPatternGenerator {
        public:
            FLTL_FORCE_INLINE static void reset(
                Generator<AlphaT> *gen
            ) throw() {
                TransitionGenerator<AlphaT>::reset(gen);
            }
        };

        template <typename AlphaT>
        class ResetPatternGenerator<AlphaT, state_tag> {
        public:
            FLTL_FORCE_INLINE static void reset(
                Generator<AlphaT> *gen
            ) throw() {

                TransitionGenerator<AlphaT>::free(gen);

                gen->cursor.transition = gen->pda->state_transitions.get(
                    gen->pattern->source->id
                );

                if(0 != gen->cursor.transition) {
                    Transition<AlphaT>::hold(gen->cursor.transition);
                }
            }
        };

        template <typename AlphaT, typename SourceT, typename ReadT>
        class FindNextTransition {
        public:

            FLTL_FORCE_INLINE static Transition<AlphaT> *next(
                PDA<AlphaT> *pda,
                Transition<AlphaT> *curr,
                Pattern<AlphaT> *
            ) throw() {
                return TransitionGenerator<AlphaT>::find_next_transition(
                    pda, curr
                );
            }
        };

        template <typename AlphaT,typename SourceT>
        class FindNextTransition<AlphaT, SourceT, symbol_tag> {
        public:

            FLTL_FORCE_INLINE static Transition<AlphaT> *next(
                PDA<AlphaT> *pda,
                Transition<AlphaT> *curr,
                Pattern<AlphaT> *pattern
            ) throw() {

                Transition<AlphaT> *trans(curr->next);
                unsigned state(curr->source_state.id);
                const unsigned num_states(pda->num_states());

            try_next_state:

                while(0 == trans) {
                    ++state;

                    if(state >= num_states) {
                        return 0;
                    }

                    trans = pda->state_transitions.get(state);
                }

                if(trans->is_deleted) {
                    trans = trans->next;
                    goto try_next_state;
                }

                // skip past any symbols we don't care about
                if(pattern->read->id < trans->sym_read.id) {
                    trans = 0;
                    goto try_next_state;
                }

                return trans;
            }
        };

        template <typename AlphaT,typename ReadT>
        class FindNextTransition<AlphaT, state_tag, ReadT> {
        public:

            FLTL_FORCE_INLINE static Transition<AlphaT> *next(
                PDA<AlphaT> *,
                Transition<AlphaT> *curr,
                Pattern<AlphaT> *
            ) throw() {
                Transition<AlphaT> *trans(curr->next);
                while(0 != trans && trans->is_deleted) {
                    trans = trans->next;
                }
                return trans;
            }
        };

        template <typename AlphaT>
        class FindNextTransition<AlphaT, state_tag, symbol_tag> {
        public:

            FLTL_FORCE_INLINE static Transition<AlphaT> *next(
                PDA<AlphaT> *,
                Transition<AlphaT> *curr,
                Pattern<AlphaT> *pattern
            ) throw() {
                Transition<AlphaT> *trans(curr->next);
                while(0 != trans && trans->is_deleted) {
                    trans = trans->next;
                }
                if(0 != trans) {
                    if(pattern->read->id < trans->sym_read.id) {
                        return 0;
                    }
                }
                return trans;
            }
        };
    }

    template <
        typename AlphaT,
        typename SourceT,
        typename ReadT,
        typename PopT,
        typename PushT,
        typename SinkT
    >
    class PatternGenerator {
    private:

        friend class PDA<AlphaT>;

        static bool next(Generator<AlphaT> *gen) throw() {

            Transition<AlphaT> *trans(gen->cursor.transition);
            Transition<AlphaT> *orig(trans);
            Pattern<AlphaT> *pattern(gen->pattern);

            OpaqueTransition<AlphaT> *binder(helper::unsafe_cast<
                OpaqueTransition<AlphaT> *
            >(gen->binder));

            if(0 == orig) {
                TransitionGenerator<AlphaT>::unbind(gen);
                return false;
            }

        check_pattern:

            // we're at the end, release the original transition
            if(0 == trans) {
                TransitionGenerator<AlphaT>::unbind(gen);
                Transition<AlphaT>::release(orig);
                gen->cursor.transition = 0;
                return false;
            }

            // can we bind this pattern?
            if(!pattern::Bind<AlphaT,SourceT>::bind(trans->source_state, pattern->source)
            || !pattern::Bind<AlphaT,ReadT>::bind(trans->sym_read, pattern->read)
            || !pattern::Bind<AlphaT,PopT>::bind(trans->sym_pop, pattern->pop)
            || !pattern::Bind<AlphaT,PushT>::bind(trans->sym_push, pattern->push)
            || !pattern::Bind<AlphaT,SinkT>::bind(trans->sink_state, pattern->sink)) {

                // nope :(

                trans = detail::FindNextTransition<
                    AlphaT,SourceT,ReadT
                >::next(gen->pda, trans, gen->pattern);

                goto check_pattern;
            }

            // we've found the current transition, bind it if necessary

            if(0 != binder) {
                binder->assign(trans);
            }

            // release what we started with
            Transition<AlphaT>::release(orig);

            // look for the next transition
            gen->cursor.transition = detail::FindNextTransition<
                AlphaT,SourceT,ReadT
            >::next(gen->pda, trans, gen->pattern);

            // hold on to the next transition if we found one
            if(0 != gen->cursor.transition) {
                Transition<AlphaT>::hold(gen->cursor.transition);
            }

            return true;
        }

        static void reset(Generator<AlphaT> *gen) throw() {
            detail::ResetPatternGenerator<AlphaT,SourceT>::reset(gen);
        }

        static void free(Generator<AlphaT> *gen) throw() {
            TransitionGenerator<AlphaT>::free(gen);
        }
    };

    /// generator type for pushdown automata
    template <typename AlphaT>
    class Generator {
    private:

        friend class PDA<AlphaT>;
        friend class SymbolGenerator<AlphaT>;
        friend class StateGenerator<AlphaT>;
        friend class TransitionGenerator<AlphaT>;

        template <typename, typename>
        friend class detail::ResetPatternGenerator;

        template <typename, typename, typename>
        friend class detail::FindNextTransition;

        template <
            typename, typename,
            typename, typename,
            typename, typename
        > friend class PatternGenerator;

        typedef Generator<AlphaT> self_type;
        typedef OpaqueTransition<AlphaT> transition_type;
        typedef OpaqueState<AlphaT> state_type;
        typedef Symbol<AlphaT> symbol_type;

        typedef void (reset_type)(self_type *);
        typedef bool (bind_next_type)(self_type *);
        typedef void (free_func_type)(self_type *);

        union {
            unsigned state_offset;
            unsigned symbol_offset;
            Transition<AlphaT> *transition;
        } cursor;

        /// pointer to some sort of type to which we are binding results
        void *binder;

        /// the binder function, does the variable binding and tells us if
        /// we can keep going
        bind_next_type *binder_func;

        /// reset the generator
        reset_type *reset_func;

        /// free the generator
        free_func_type *free_func;

        /// have we started?
        bool has_been_used;

        /// pushdown automaton over which we are generating things
        PDA<AlphaT> *pda;

        /// pattern against which to match
        Pattern<AlphaT> *pattern;

        Generator(
            void *binder_,
            bind_next_type *binder_func_,
            reset_type *reset_func_,
            free_func_type *free_func_,
            PDA<AlphaT> *pda_,
            Pattern<AlphaT> *pattern_
        ) throw()
            : binder(binder_)
            , binder_func(binder_func_)
            , reset_func(reset_func_)
            , free_func(free_func_)
            , has_been_used(false)
            , pda(pda_)
            , pattern(pattern_)
        {
            memset(&cursor, 0, sizeof cursor);

            if(0 != pattern) {
                Pattern<AlphaT>::hold(pattern);
            }
        }

    public:

        Generator(void) throw()
            : binder(0)
            , binder_func(&(def::next))
            , reset_func(&(def::reset))
            , free_func(&(def::free))
            , has_been_used(false)
            , pda(0)
            , pattern(0)
        {
            memset(&cursor, 0, sizeof cursor);
        }

        Generator(const self_type &that) throw()
            : binder(that.binder)
            , binder_func(that.binder_func)
            , reset_func(that.reset_func)
            , free_func(that.free_func)
            , has_been_used(false)
            , pda(that.pda)
            , pattern(that.pattern)
        {
            if(0 != pattern) {
                Pattern<AlphaT>::hold(pattern);
            }
            memcpy(&cursor, &(that.cursor), sizeof cursor);
        }

        ~Generator(void) throw() {
            free_func(this);
            if(0 != pattern) {
                Pattern<AlphaT>::release(pattern);
                pattern = 0;
            }
            pda = 0;
        }

        self_type &operator=(self_type &that) throw() {
            assert(
                0 == pda &&
                "Illegal assignment to an initialized generator."
            );

            free_func(this);

            pda = that.pda;
            memcpy(&cursor, &(that.cursor), sizeof cursor);

            binder = that.binder;
            binder_func = that.binder_func;
            reset_func = that.reset_func;
            free_func = that.free_func;
            pattern = that.pattern;
            has_been_used = false;

            if(0 != pattern) {
                Pattern<AlphaT>::hold(pattern);
            }

            return *this;
        }

        /// rewind the generator to its initial conditions
        inline void rewind(void) throw() {
            reset_func(this);
        }

        /// try to generate the next object(s) according to the binder
        /// conditions and then bind the binder objects. returns true if
        /// we were able to find a successful binding, false otherwise.
        inline bool match_next(void) throw() {
            if(!has_been_used) {
                has_been_used = true;
                reset_func(this);
            }
            return binder_func(this);
        }
    };

}}

#endif /* FLTL_PDA_GENERATOR_HPP_ */
