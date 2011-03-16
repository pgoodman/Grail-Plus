/*
 * VariableSymbol.hpp
 *
 *  Created on: Feb 2, 2011
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

#ifndef FLTL_VARIABLESYMBOL_HPP_
#define FLTL_VARIABLESYMBOL_HPP_

#include <functional>

namespace fltl { namespace cfg {

    /// represents a non-terminal of a grammar
    template <typename AlphaT>
    class VariableSymbol : public Symbol<AlphaT> {
    private:

        friend class CFG<AlphaT>;
        friend class OpaqueProduction<AlphaT>;
        friend class detail::PatternData<AlphaT>;
        friend struct std::less<VariableSymbol<AlphaT> >;

        typedef VariableSymbol<AlphaT> self_type;

        explicit VariableSymbol(const internal_sym_type _value) throw()
            : Symbol<AlphaT>(_value)
        { }

    public:

        VariableSymbol(void) throw()
            : Symbol<AlphaT>(1)
        { }

        VariableSymbol(const self_type &that) throw()
            : Symbol<AlphaT>(that)
        { }

        VariableSymbol(const Symbol<AlphaT> &that) throw()
            : Symbol<AlphaT>(that)
        {
            assert(that.is_variable());
        }

        /// return an "unbound" version of this symbol
        /// note: *not* const!!
        Unbound<AlphaT,variable_tag> operator~(void) throw() {
            return Unbound<AlphaT,variable_tag>(this);
        }

        self_type &operator=(const Symbol<AlphaT> &that) throw() {
            assert(that.is_variable());
            this->value = that.value;
            return *this;
        }

        FLTL_CFG_PRODUCTION_PATTERN(variable_tag)

        bool operator<(const self_type &that) const throw() {
            return this->value < that.value;
        }

        bool operator<=(const self_type &that) const throw() {
            return this->value <= that.value;
        }

        bool operator>(const self_type &that) const throw() {
            return this->value > that.value;
        }

        bool operator>=(const self_type &that) const throw() {
            return this->value >= that.value;
        }

        unsigned number(void) const throw() {
            return static_cast<unsigned>(this->value);
        }
    };

}}

namespace std {

    template <typename AlphaT>
    struct less<fltl::cfg::VariableSymbol<AlphaT> > : binary_function <fltl::cfg::VariableSymbol<AlphaT>,fltl::cfg::VariableSymbol<AlphaT>,bool> {
    public:
        bool operator() (const fltl::cfg::VariableSymbol<AlphaT> &x, const fltl::cfg::VariableSymbol<AlphaT> &y) const {
            return x.value < y.value;
        }
    };
}

#endif /* FLTL_VARIABLESYMBOL_HPP_ */
