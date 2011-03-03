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
        bool gen_next(Generator<AlphaT> *) throw() {
            return false;
        }

        template <typename AlphaT>
        void gen_reset(Generator<AlphaT> *) throw() { }

        template <typename AlphaT>
        void gen_free(Generator<AlphaT> *) throw() { }
    }

    /// implementation of generator for states
    template <typename AlphaT>
    class StateGenerator {
    private:

        friend class PDA<AlphaT>;

        static bool gen_next(Generator<AlphaT> *gen) throw() {
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

        static void gen_reset(Generator<AlphaT> *gen) throw() {
            gen->cursor.state_offset = 0U;
        }

        static void gen_free(Generator<AlphaT> *) throw() { }
    };

    /// implementation of generator for symbols
    template <typename AlphaT>
    class SymbolGenerator {
    private:

        friend class PDA<AlphaT>;

        static bool gen_next(Generator<AlphaT> *gen) throw() {
            const unsigned offset(++(gen->cursor.symbol_offset));

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

        static void gen_reset(Generator<AlphaT> *gen) throw() {
            gen->cursor.symbol_offset = 0U;
        }

        static void gen_free(Generator<AlphaT> *) throw() { }
    };

    /// implementation of generator for transitions
    template <typename AlphaT>
    class TransitionGenerator {
    private:

        friend class PDA<AlphaT>;

        static Transition<AlphaT> *find_next_transition(
            PDA<AlphaT> *pda,
            Transition<AlphaT> *curr
        ) throw() {
            assert(0 != curr);

            Transition<AlphaT> *next(curr->next);
            unsigned offset(curr->source_state.id);
            unsigned num_states(pda->num_states());

            // go find the next non-null transition
            while(0 == next) {
                ++offset;

                if(offset >= num_states) {
                    return 0;
                } else {
                    next = pda->state_transitions.get(offset);
                }
            }

            return next;
        }

        static bool gen_next(Generator<AlphaT> *gen) throw() {

            OpaqueTransition<AlphaT> *binder(helper::unsafe_cast<
                OpaqueTransition<AlphaT> *
            >(gen->binder));

            Transition<AlphaT> *old_trans(gen->cursor.transition);

            binder->assign(old_trans);

            if(0 == old_trans) {
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

            // release the current transition
            Transition<AlphaT>::release(old_trans);

            return true;
        }

        static void gen_reset(Generator<AlphaT> *gen) throw() {
            gen_free(gen);
            gen->cursor.transition = gen->pda->first_transition;
            if(0 != gen->cursor.transition) {
                Transition<AlphaT>::hold(gen->cursor.transition);
            }
        }

        static void gen_free(Generator<AlphaT> *gen) throw() {
            if(0 != gen->cursor.transition) {
                Transition<AlphaT>::release(gen->cursor.transition);
                gen->cursor.transition = 0;
            }
        }
    };

    template <typename AlphaT>
    class PatternGenerator {

    };

    /// generator type for pushdown automata
    template <typename AlphaT>
    class Generator {
    private:

        friend class PDA<AlphaT>;
        friend class SymbolGenerator<AlphaT>;
        friend class StateGenerator<AlphaT>;
        friend class TransitionGenerator<AlphaT>;
        friend class PatternGenerator<AlphaT>;

        typedef Generator<AlphaT> self_type;
        typedef OpaqueTransition<AlphaT> transition_type;
        typedef OpaqueState<AlphaT> state_type;
        typedef Symbol<AlphaT> symbol_type;

        typedef void (reset_gen_type)(self_type *);
        typedef bool (bind_next_type)(self_type *);
        typedef void (free_func_type)(self_type *);

        union {
            unsigned state_offset;
            unsigned symbol_offset;
            Transition<AlphaT> *transition;
        } cursor;

        /// pointer to some sort of type to which we are binding results
        void *binder;
        //pattern::PatternData<AlphaT> *pattern;

        /// the binder function, does the variable binding and tells us if
        /// we can keep going
        bind_next_type *binder_func;

        /// reset the generator
        reset_gen_type *reset_func;

        /// free the generator
        free_func_type *free_func;

        /// have we started?
        bool has_been_used;

        /// pushdown automaton over which we are generating things
        PDA<AlphaT> *pda;

        Generator(
            void *binder_,
            bind_next_type *binder_func_,
            reset_gen_type *reset_func_,
            free_func_type *free_func_,
            PDA<AlphaT> *pda_
        ) throw()
            : binder(binder_)
            , binder_func(binder_func_)
            , reset_func(reset_func_)
            , free_func(free_func_)
            , has_been_used(false)
            , pda(pda_)
        {
            memset(&cursor, 0, sizeof cursor);
        }

    public:

        Generator(void) throw()
            : binder(0)
            , binder_func(&(def::gen_next))
            , reset_func(&(def::gen_reset))
            , free_func(&(def::gen_free))
            , has_been_used(false)
            , pda(0)
        {
            memset(&cursor, 0, sizeof cursor);
        }

        Generator(const self_type &that) throw()
            : binder(that.binder)
            , binder_func(that.binder_func)
            , reset_func(that.reset_func)
            , free_func(that.free_func)
            , has_been_used(false),
            pda(that.pda)
        {
            memcpy(&cursor, &(that.cursor), sizeof cursor);
        }

        ~Generator(void) throw() {
            free_func(this);
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
            has_been_used = false;

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
