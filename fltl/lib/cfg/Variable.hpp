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

        /// id of this variable
        cfg::internal_sym_type id;

        /// the next variable of this grammar
        Variable<AlphaT> *next;

        /// the first production related to this variable
        Production<AlphaT> *productions;

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
            cfg::Production<AlphaT>::hold(prod);
            if(0 != productions) {
                productions->prev = prod;
            }
            prod->prev = 0;
            prod->next = productions;
            productions = prod;
        }

        inline void remove_production(cfg::Production<AlphaT> *prod) throw() {

            if(0 == prod->prev) {
                productions = prod->next;
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
            , productions(0)
        { }

        ~Variable(void) throw() {

            // free each variable's productions
            for(cfg::Production<AlphaT> *prod(productions), *next_prod(0);
                0 != prod;
                prod = next_prod) {

                next_prod = prod->next;
                cfg::Production<AlphaT>::release(prod);
            }
        }

        /// get a pointer to the next pointer of this variable. used by
        /// the list allocator for variables in the grammar.
        inline static Variable<AlphaT> **
        get_next_pointer(Variable<AlphaT> *var) {
            return &(var->next);
        }
    };

}}}

#endif /* FLTL_VARIABLE_HPP_ */
