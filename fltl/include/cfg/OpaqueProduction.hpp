/*
 * OpaqueProduction.hpp
 *
 *  Created on: Jan 21, 2011
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

#ifndef FLTL_OPAQUEPRODUCTION_HPP_
#define FLTL_OPAQUEPRODUCTION_HPP_

#include <functional>

namespace fltl { namespace cfg {

    /// the production type that is exposed to users
    template <typename AlphaT>
    class OpaqueProduction {
    private:

        friend class CFG<AlphaT>;
        friend class detail::SimpleGenerator<AlphaT>;

        template <typename, typename>
        friend class detail::PatternGenerator;

        typedef OpaqueProduction<AlphaT> self_type;
        typedef typename CFG<AlphaT>::variable_type variable_type;
        typedef typename CFG<AlphaT>::symbol_type symbol_type;
        typedef typename CFG<AlphaT>::symbol_string_type symbol_string_type;

        /// the internal production that this opaque type references
        Production<AlphaT> *production;

        /// proper constructor for productions.
        explicit OpaqueProduction(Production<AlphaT> *prod) throw()
            : production(prod)
        {
            if(0 != prod) {
                Production<AlphaT>::hold(production);
            }
        }

        void assign(Production<AlphaT> *prod) throw() {
            if(production != prod) {

                if(0 != production) {
                    Production<AlphaT>::release(production);
                }

                production = prod;

                if(0 != production) {
                    Production<AlphaT>::hold(production);
                }
            }
        }

    public:

        OpaqueProduction(void) throw()
            : production(0)
        { }

        OpaqueProduction(const self_type &that) throw()
            : production(that.production)
        {
            if(0 != production) {
                Production<AlphaT>::hold(production);
            }
        }

        ~OpaqueProduction(void) throw() {
            if(0 != production) {
                Production<AlphaT>::release(production);
                production = 0;
            }
        }

        inline self_type &operator=(const self_type &that) throw() {
            assign(that.production);
            return *this;
        }

        /// is this production valid?
        inline bool is_valid(void) const throw() {
            return 0 != production;
        }

        /// access the variable of this production
        inline variable_type
        variable(void) const throw() {
            assert(
                0 != production &&
                "Unable to access variable of non-existent production."
            );
            return variable_type(production->var->id);
            //return *helper::unsafe_cast<variable_type *>(&(production->var));
        }

        /// return the length of this production
        inline unsigned length(void) const throw() {
            assert(
                0 != production &&
                "Unable to access length of non-existent production."
            );
            return production->length();
        }

        /// was this production automatically added to its variable? this
        /// happens in the event that we remove all productions from a
        /// variable.
        inline bool was_automatically_made(void) const throw() {
            assert(
                0 != production &&
                "Unable to access state of non-existent production."
            );
            return production->var->null_production == production;
        }

        /// equivalence of productions; to check that symbols are equivalent,
        /// use a.symbols() == b.symbols().
        inline bool operator==(const self_type &that) const throw() {
            return production == that.production;
        }
        inline bool operator!=(const self_type &that) const throw() {
            return production != that.production;
        }

        /// access a symbol from the production. uses 0 indexing, i.e. if we
        /// have a production of the form prod = A -> B C D then:
        ///             prod[0] <==> prod.symbol(0) <==> B.
        inline const symbol_type &
        operator[](const unsigned offset) const throw() {
            return symbol_at(offset);
        }

        inline const symbol_type &
        symbol_at(const unsigned offset) const throw() {
            assert(
                0 != production &&
                "Unable to access symbol of non-existent production."
            );
            return production->symbols.at(offset);
        }

        /// get a slice of symbols
        inline const symbol_string_type &
        symbols(void) const throw() {
            return production->symbols;
        }

        Unbound<AlphaT, production_tag> operator~(void) throw() {
            return Unbound<AlphaT, production_tag>(this);
        }

        bool operator<(const self_type &that) const throw() {
            if(0 == production && 0 == that.production) {
                return false;
            } else if(0 == production) {
                return true;
            } else if(0 == that.production) {
                return false;
            } else {
                return production->is_less_than(*(that.production));
            }
        }

        uint64_t number(void) const throw() {
            if(0 == production) {
                return 0UL;
            }

            uint64_t number(0UL);
            number |= production->var->id;
            number <<= 16U;
            number <<= 16U;

            if(0 != production->symbols.symbols) {
                number |= production->symbols.symbols[str::HASH].value;
            }

            return number;
        }
    };

}}

namespace std {
    template <typename  AlphaT>
    struct less<fltl::cfg::OpaqueProduction<AlphaT> > : binary_function <fltl::cfg::OpaqueProduction<AlphaT>, fltl::cfg::OpaqueProduction<AlphaT>, bool> {
        bool operator() (const fltl::cfg::OpaqueProduction<AlphaT> &x, const fltl::cfg::OpaqueProduction<AlphaT> &y) const {
            return x.number() < y.number();
        }
    };
}

#endif /* FLTL_OPAQUEPRODUCTION_HPP_ */
