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
        bool default_gen_next(Generator<AlphaT> *, void *) throw() {
            return false;
        }

        template <typename AlphaT>
        class SimpleGenerator {
        public:

            /// generate productions
            static bool find_next_production(
                Generator<AlphaT> *state,
                void *_binder
            ) throw() {

                if(0 == state->variable) {
                    return false;
                }

                Variable<AlphaT> *var(state->variable);
                Production<AlphaT> *prod(state->production);

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
                    state->variable = var;
                    state->production = prod;
                }

                // get the binder
                OpaqueProduction<AlphaT> *binder(
                    helper::unsafe_cast<OpaqueProduction<AlphaT> *>(_binder)
                );

                // bind the production
                OpaqueProduction<AlphaT> oprod(prod);
                *binder = oprod;

                return true;
            }

            /// generate variables
            static bool find_next_variable(
                Generator<AlphaT> *state,
                void *_binder
            ) throw() {
                if(0 == state->variable) {
                    return false;
                }

                state->variable = state->variable->next;

                if(0 == state->variable) {
                    return false;
                }

                Symbol<AlphaT> *binder(
                    helper::unsafe_cast<Symbol<AlphaT> *>(_binder)
                );

                // bind the variable
                binder->value = state->variable->id;
                return true;
            }

            /// generate terminals
            static bool find_next_terminal(
                Generator<AlphaT> *state,
                void *_binder
            ) throw() {
                ++(state->terminal_offset);

                if(state->terminal_offset > state->cfg->terminal_map.size()) {
                    return false;
                }

                Symbol<AlphaT> *binder(
                    helper::unsafe_cast<Symbol<AlphaT> *>(_binder)
                );

                // bind the variable
                binder->value = -1 * static_cast<internal_sym_type>(
                    state->terminal_offset - 1
                );

                return true;
            }
        };
    }

    template <typename AlphaT>
    class Generator {
    private:

        friend class CFG<AlphaT>;
        friend class detail::SimpleGenerator<AlphaT>;

        typedef Generator<AlphaT> self_type;
        typedef bool (bind_next_type)(self_type *, void *);

        /// CFG from which we are generating stuff
        CFG<AlphaT> *cfg;

        /// the last variable bound by the generator
        Variable<AlphaT> *variable;

        /// the last production bound by the generator
        Production<AlphaT> *production;

        /// offset into the terminals into which we're looking
        unsigned terminal_offset;

        /// pointer to some sort of type to which we are binding results
        void *binder;

        /// the binder function, does the variable binding and tells us if
        /// we can keep going
        bind_next_type *binder_func;

        /// private constructor for use by CFG
        Generator(
            CFG<AlphaT> *_cfg,
            Variable<AlphaT> *_variable,
            Production<AlphaT> *_production,
            const unsigned _terminal_offset,
            void *_binder,
            bind_next_type *_binder_func
        ) throw()
            : cfg(_cfg)
            , variable(_variable)
            , production(_production)
            , terminal_offset(_terminal_offset)
            , binder(_binder)
            , binder_func(_binder_func)
        {

        }

    public:

        Generator(void) throw()
            : cfg(0)
            , variable(0)
            , production(0)
            , terminal_offset(0)
            , binder(0)
            , binder_func(&detail::default_gen_next)
        { }

        /// copy constructor for public use
        Generator(const self_type &that) throw()
            : cfg(that.cfg)
            , variable(that.variable)
            , production(that.production)
            , terminal_offset(that.terminal_offset)
            , binder(that.binder)
            , binder_func(that.binder_func)
        { }

        self_type &operator=(self_type &that) throw() {
            cfg = that.cfg;
            variable = that.variable;
            production = that.production;
            terminal_offset = that.terminal_offset;
            binder = that.binder;
            binder_func = that.binder_func;
            return *this;
        }

        bool find_next(void) throw() {
            return binder_func(this, binder);
        }
    };
}}}

#endif /* FLTL_GENERATOR_HPP_ */
