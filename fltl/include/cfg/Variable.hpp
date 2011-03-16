/*
 * Variable.hpp
 *
 *  Created on: Jan 20, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
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
        Variable<AlphaT> *prev;

        /// the first production related to this variable
        Production<AlphaT> *first_production;

        /// the number of productions
        unsigned num_productions;

        /// the name associated with this variable. if the name is 0 then
        /// an automatic name is generated when the CFG is printed. note:
        /// the name is *owned* by the variable
        const char *name;

    public:

        Variable(void) throw()
            : id(0)
            , next(0)
            , prev(0)
            , first_production(0)
            , num_productions(0)
            , name(0)
        { }

        ~Variable(void) throw() {

            // free each variable's first_production
            for(cfg::Production<AlphaT> *prod(first_production), *next_prod(0);
                0 != prod;
                prod = next_prod) {

                next_prod = prod->next;
                prod->var = 0;
                prod->prev = 0;
                prod->next = 0;

                if(!prod->is_deleted) {
                    cfg::Production<AlphaT>::release(prod);
                }
            }

            if(0 != name) {
                delete [] name;
            }

            name = 0;
            num_productions = 0;
        }
    };

}}

#endif /* FLTL_VARIABLE_HPP_ */
