/*
 * Terminal.hpp
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

#ifndef FLTL_TERMINAL_HPP_
#define FLTL_TERMINAL_HPP_

#include <functional>

namespace fltl { namespace cfg {

    template <typename AlphaT>
    class TerminalSymbol : public Symbol<AlphaT> {
    private:

        friend class CFG<AlphaT>;
        friend class detail::PatternData<AlphaT>;
        friend struct std::less<TerminalSymbol<AlphaT> >;

        typedef TerminalSymbol<AlphaT> self_type;

        explicit TerminalSymbol(const internal_sym_type _value) throw()
            : cfg::Symbol<AlphaT>(_value)
        { }

    public:

        TerminalSymbol(void) throw()
            : Symbol<AlphaT>(-1)
        { }

        TerminalSymbol(const self_type &that) throw()
            : Symbol<AlphaT>(that)
        { }

        TerminalSymbol(const Symbol<AlphaT> &that) throw()
            : Symbol<AlphaT>(that)
        {
            assert(that.is_terminal());
        }

        /// return an "unbound" version of this symbol
        /// note: *not* const!!
        Unbound<AlphaT,terminal_tag> operator~(void) throw() {
            return Unbound<AlphaT,terminal_tag>(this);
        }

        self_type &operator=(const Symbol<AlphaT> &that) throw() {
            assert(that.is_terminal());
            this->value = that.value;
            return *this;
        }

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
            return static_cast<unsigned>(-1 * this->value);
        }
    };
}}

namespace std {

    template <typename AlphaT>
    struct less<fltl::cfg::TerminalSymbol<AlphaT> > : binary_function <fltl::cfg::TerminalSymbol<AlphaT>,fltl::cfg::TerminalSymbol<AlphaT>,bool> {
    public:
        bool operator() (const fltl::cfg::TerminalSymbol<AlphaT> &x, const fltl::cfg::TerminalSymbol<AlphaT> &y) const {
            return x.value < y.value;
        }
    };
}

#endif /* FLTL_TERMINAL_HPP_ */
