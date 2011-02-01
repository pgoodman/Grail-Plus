/*
 * Variable.hpp
 *
 *  Created on: Jan 20, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_VARIABLE_HPP_
#define FLTL_VARIABLE_HPP_

namespace fltl { namespace lib { namespace cfg {

    /// grammar variable
    template <typename AlphaT>
    class Variable {
    private:

        friend class CFG<AlphaT>;
        friend class detail::SimpleGenerator<AlphaT>;
        friend class Production<AlphaT>;
        friend class OpaqueProduction<AlphaT>;

        template <typename, typename>
        friend class detail::PatternGenerator;

        /// id of this variable
        cfg::internal_sym_type id;

        /// the next variable of this grammar
        Variable<AlphaT> *next;

        /// the first production related to this variable
        Production<AlphaT> *first_production;

        /// initialize the variable
        void init(
            const cfg::internal_sym_type _id,
            Variable<AlphaT> *prev
        ) throw() {
            id = _id;
            if(0 != prev) {
                next = prev->next;
                prev->next = this;
            } else {
                next = 0;
            }
        }

        /// add a production to this variable
        void add_production(cfg::Production<AlphaT> *prod) throw() {

            if(0 != first_production) {
                prod->prev = first_production->prev;
                first_production->prev = prod;
            } else {
                prod->prev = 0;
            }

            prod->next = first_production;
            first_production = prod;
        }

        inline void remove_production(cfg::Production<AlphaT> *prod) throw() {

            if(0 == prod->prev) {
                first_production = prod->next;
            } else {
                prod->prev->next = prod->next;
            }

            if(0 != prod->next) {
                prod->next->prev = prod->prev;
            }
        }

    public:

        Variable(void) throw()
            : id(0)
            , next(0)
            , first_production(0)
        { }

        Variable(const Variable<AlphaT> &) throw()
            : id(0)
            , next(0)
            , first_production(0)
        {
            assert(false);
        }

        ~Variable(void) throw() {

            // free each variable's first_production
            for(cfg::Production<AlphaT> *prod(first_production), *next_prod(0);
                0 != prod;
                prod = next_prod) {

                next_prod = prod->next;
                cfg::Production<AlphaT>::release(prod);
            }
        }

        Variable<AlphaT> &operator=(const Variable<AlphaT> &) throw() {
            assert(false);
            return *this;
        }
    };

}}}

#endif /* FLTL_VARIABLE_HPP_ */
