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

namespace fltl { namespace cfg {

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

        /// the default empty production for this variable
        Production<AlphaT> *null_production;

        /// the name associated with this variable. if the name is 0 then
        /// an automatic name is generated when the CFG is printed. note:
        /// the name is *owned* by the variable
        const char *name;

        /// initialize the variable
        void init(
            const cfg::internal_sym_type _id,
            Variable<AlphaT> *prev,
            const char *_name
        ) throw() {
            id = _id;
            name = _name;
            if(0 != prev) {
                next = prev->next;
                prev->next = this;
            } else {
                next = 0;
            }

            make_null_production();
        }

        void make_null_production(void) {
            null_production = CFG<AlphaT>::production_allocator->allocate();
            null_production->var = this;
            first_production = null_production;
            Production<AlphaT>::hold(null_production);
        }

    public:

        Variable(void) throw()
            : id(0)
            , next(0)
            , first_production(0)
            , null_production(0)
            , name(0)
        { }

        Variable(const Variable<AlphaT> &) throw()
            : id(0)
            , next(0)
            , first_production(0)
            , null_production(0)
            , name(0)
        {
            assert(false);
        }

        ~Variable(void) throw() {

            // free each variable's first_production
            for(cfg::Production<AlphaT> *prod(first_production), *next_prod(0);
                0 != prod;
                prod = next_prod) {

                next_prod = prod->next;
                prod->var = 0;

                if(!prod->is_deleted) {
                    cfg::Production<AlphaT>::release(prod);
                }
            }

            if(0 != name) {
                delete [] name;
            }

            null_production = 0;
            name = 0;
        }

        Variable<AlphaT> &operator=(const Variable<AlphaT> &) throw() {
            assert(false);
            return *this;
        }
    };

}}

#endif /* FLTL_VARIABLE_HPP_ */
