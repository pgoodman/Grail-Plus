/*
 * Symbol.hpp
 *
 *  Created on: Feb 26, 2011
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

#ifndef FLTL_PDA_SYMBOL_HPP_
#define FLTL_PDA_SYMBOL_HPP_

namespace fltl { namespace pda {

    /// represents a symbol from the alphabet
    template <typename AlphaT>
    class Symbol {
    private:

        typedef Symbol<AlphaT> self_type;

        friend class PDA<AlphaT>;
        friend class SymbolBuffer<AlphaT>;
        friend class SymbolGenerator<AlphaT>;
        friend struct std::less<self_type>;

        template <
            typename, typename,
            typename, typename,
            typename, typename
        > friend class PatternGenerator;

        template <typename, typename, typename>
        friend class detail::FindNextTransition;

        unsigned id;

        Symbol(const unsigned _id) throw()
            : id(_id)
        { }

    public:

        typedef symbol_tag tag_type;

        Symbol(void) throw()
            : id(0)
        { }

        Symbol(const self_type &that) throw()
            : id(that.id)
        { }

        ~Symbol(void) throw() {
            id = 0U;
        }

        self_type &operator=(const self_type &that) throw() {
            id = that.id;
            return *this;
        }

        bool operator==(const self_type &that) const throw() {
            return id == that.id;
        }

        bool operator!=(const self_type &that) const throw() {
            return id != that.id;
        }

        bool operator<(const self_type &that) const throw() {
            return id < that.id;
        }

        bool operator<=(const self_type &that) const throw() {
            return id <= that.id;
        }

        bool operator>(const self_type &that) const throw() {
            return id > that.id;
        }

        bool operator>=(const self_type &that) const throw() {
            return id >= that.id;
        }

        /// note: not const!
        Unbound<AlphaT,symbol_tag> operator~(void) throw() {
            return Unbound<AlphaT,symbol_tag>(this);
        }

        unsigned number(void) const throw() {
            return id;
        }
    };
}}


namespace std {

    template <typename AlphaT>
    struct less<fltl::pda::Symbol<AlphaT> > : binary_function <fltl::pda::Symbol<AlphaT>,fltl::pda::Symbol<AlphaT>,bool> {
    public:
        bool operator() (const fltl::pda::Symbol<AlphaT> &x, const fltl::pda::Symbol<AlphaT> &y) const {
            return x.id < y.id;
        }
    };
}

#endif /* FLTL_PDA_SYMBOL_HPP_ */
