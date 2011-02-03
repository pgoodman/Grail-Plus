/*
 * Generator.hpp
 *
 *  Created on: Jan 27, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_GENERATOR_HPP_
#define FLTL_GENERATOR_HPP_

namespace fltl { namespace lib { namespace cfg {

    namespace detail {

        template <typename AlphaT>
        bool default_gen_next(Generator<AlphaT> *) throw() {
            return false;
        }

        template <typename AlphaT>
        void default_gen_reset(Generator<AlphaT> *) throw() { }

        template <typename AlphaT>
        class SimpleGenerator {
        public:

            static void
            reset_next_production(Generator<AlphaT> *state) throw() {
                state->cursor.production = state->cfg->first_production;
            }


#if 0
            static bool
            find_next_production(Generator<AlphaT> *state) throw() {

                if(0 == state->variable) {
                    return false;
                }

                Variable<AlphaT> *var(state->variable);
                Production<AlphaT> *prod(state->production);

            get_next_production:

                // move to the next production if we can
                if(0 != prod) {
                    prod = prod->next;
                }

                // find the production to bind
                if(0 != var) {
                    for(; 0 == prod; ) {

                        var = var->next;
                        if(0 != var) {
                            prod = var->first_production;
                        } else {
                            break;
                        }
                    }
                }

                // update the state
                if(0 == var || 0 == prod) {
                    state->variable = 0;
                    state->production = 0;
                    return false;
                } else {

                    // make sure to skip over deleted productions that are
                    // still hanging around
                    if(prod->is_deleted) {
                        goto get_next_production;
                    }

                    state->variable = var;
                    state->production = prod;
                }

                return true;
            }
#endif

            static Production<AlphaT> *
            find_next_production(Production<AlphaT> *prod) throw() {
                // go look for the next production
                cfg::Production<AlphaT> *next_prod(prod->next);
                cfg::Variable<AlphaT> *curr_var(prod->var);

                while(true) {
                    for(; 0 != next_prod; next_prod = next_prod->next) {
                        if(!next_prod->is_deleted) {
                            goto found_next_prod;
                        }
                    }

                    curr_var = curr_var->next;

                    if(0 == curr_var) {
                        goto found_next_prod;
                    }

                    next_prod = curr_var->first_production;
                }

            found_next_prod:
                return next_prod;
            }

            /// generate productions
            static bool
            bind_next_production(Generator<AlphaT> *state) throw() {

                // get the binder
                OpaqueProduction<AlphaT> *binder(
                    helper::unsafe_cast<OpaqueProduction<AlphaT> *>(
                        state->binder
                    )
                );

                // update or clear the binding
                cfg::Production<AlphaT> *prod(state->cursor.production);
                OpaqueProduction<AlphaT> opaque_prod(prod);

                *binder = opaque_prod;

                if(0 == prod) {
                    return false;
                }

                state->cursor.production = find_next_production(prod);

                return true;
            }

            /// reset the variable generator
            static void reset_next_variable(Generator<AlphaT> *state) throw() {
                state->variable = state->cfg->variable_map.get(0);
            }

            /// generate variables
            static bool bind_next_variable(Generator<AlphaT> *state) throw() {

                Symbol<AlphaT> *binder(
                    helper::unsafe_cast<Symbol<AlphaT> *>(state->binder)
                );

                Variable<AlphaT> *&var(state->cursor.variable);

                if(0 == var) {
                    binder->value = 0;
                    return false;
                }

                var = var->next;

                if(0 == state->variable) {
                    binder->value = 0;
                    return false;
                }

                // bind the variable
                binder->value = var->id;
                return true;
            }

            /// reset the terminal generator
            static void reset_next_terminal(Generator<AlphaT> *state) throw() {
                state->cursor.terminal_offset = 1U;
            }

            /// generate terminals
            static bool bind_next_terminal(Generator<AlphaT> *state) throw() {
                Symbol<AlphaT> *binder(
                    helper::unsafe_cast<Symbol<AlphaT> *>(state->binder)
                );

                unsigned &offset(state->cursor.terminal_offset);

                ++offset;

                if(offset > state->cfg->terminal_map.size()) {
                    binder->value = 0;
                    return false;
                }

                // bind the variable
                binder->value = -1 * static_cast<internal_sym_type>(
                    offset - 1
                );

                return true;
            }
        };

        /// template for complex patterns
        template <typename AlphaT, typename PatternBuilderT>
        class PatternGenerator {
        public:

            static bool bind_next_pattern(Generator<AlphaT> *state) throw() {
                Production<AlphaT> *&prod(state->cursor.production);
                Production<AlphaT> *bound_prod(0);

                if(0 == prod) {
                    return false;
                }

                do {
                    OpaqueProduction<AlphaT> opaque_prod(prod);

                    bound_prod = prod;
                    prod = SimpleGenerator<AlphaT>::find_next_production(prod);

                    // bind the pattern
                    if(PatternBuilderT::static_match(state->pattern, opaque_prod)) {
                        break;
                    } else {
                        bound_prod = 0;
                    }

                } while(0 != prod);

                // bind the production
                if(0 != state->binder) {
                    OpaqueProduction<AlphaT> opaque_prod(bound_prod);
                    OpaqueProduction<AlphaT> *binder(
                        helper::unsafe_cast<OpaqueProduction<AlphaT> *>(
                            state->binder
                        )
                    );

                    *binder = opaque_prod;
                }

                return 0 != bound_prod;
            }

            static void reset_next_pattern(Generator<AlphaT> *state) throw() {
                if(1 == PatternBuilderT::IS_BOUND_TO_VAR) {
                    state->cursor.production = state->cfg->variable_map.get(
                        static_cast<unsigned>(
                            helper::unsafe_cast<const VariableSymbol<AlphaT> *>(
                                state->pattern->get_var()
                            )->value
                        )
                    )->first_production;
                } else {
                    state->cursor.production = state->cfg->first_production;
                }
            }
        };
    }

    template <typename AlphaT>
    class Generator {
    private:

        friend class CFG<AlphaT>;
        friend class detail::SimpleGenerator<AlphaT>;

        template <typename, typename>
        friend class detail::PatternGenerator;

        typedef Generator<AlphaT> self_type;
        typedef typename CFG<AlphaT>::production_type production_type;

        typedef void (reset_gen_type)(self_type *);
        typedef bool (bind_next_type)(self_type *);
        typedef bool (pattern_func_type)(void *, const production_type &);

        /// CFG from which we are generating stuff
        CFG<AlphaT> *cfg;

        union {

            /// the last variable bound by the generator
            Variable<AlphaT> *variable;

            /// the last production bound by the generator
            Production<AlphaT> *production;

            /// offset into the terminals into which we're looking
            unsigned terminal_offset;

        } cursor;

        /// pointer to some sort of type to which we are binding results
        void *binder;
        PatternData<AlphaT> *pattern;

        /// the binder function, does the variable binding and tells us if
        /// we can keep going
        bind_next_type *binder_func;

        /// reset the generator
        reset_gen_type *reset_func;

        Generator(
            CFG<AlphaT> *_cfg,
            void *_binder,
            PatternData<AlphaT> *_pattern,
            bind_next_type *_binder_func,
            reset_gen_type *_reset_func
        ) throw()
            : cfg(_cfg)
            , binder(_binder)
            , pattern(_pattern)
            , binder_func(_binder_func)
            , reset_func(_reset_func)
        {
            memset(&cursor, 0, sizeof cursor);

            if(0 != pattern) {
                PatternData<AlphaT>::incref(pattern);
            }
        }

    public:

        Generator(void) throw()
            : cfg(0)
            , binder(0)
            , pattern(0)
            , binder_func(&detail::default_gen_next)
            , reset_func(&detail::default_gen_reset)
        {
            memset(&cursor, 0, sizeof cursor);
        }

        /// copy constructor for public use
        Generator(const self_type &that) throw()
            : cfg(that.cfg)
            , binder(that.binder)
            , pattern(that.pattern)
            , binder_func(that.binder_func)
            , reset_func(that.reset_func)
        {
            memcpy(&cursor, &(that.cursor), sizeof cursor);

            if(0 != pattern) {
                PatternData<AlphaT>::incref(pattern);
            }
        }

        ~Generator(void) throw() {
            if(0 != pattern) {
                PatternData<AlphaT>::decref(pattern);
            }
        }

        self_type &operator=(self_type &that) throw() {
            assert(
                0 == cfg &&
                "Illegal assignment to an initialized generator."
            );

            if(0 != pattern) {
                PatternData<AlphaT>::decref(pattern);
            }

            cfg = that.cfg;
            memcpy(&cursor, &(that.cursor), sizeof cursor);
            binder = that.binder;
            pattern = that.pattern;
            binder_func = that.binder_func;
            reset_func = that.reset_func;

            if(0 != pattern) {
                PatternData<AlphaT>::incref(pattern);
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
            return binder_func(this);
        }
    };
}}}

#endif /* FLTL_GENERATOR_HPP_ */
