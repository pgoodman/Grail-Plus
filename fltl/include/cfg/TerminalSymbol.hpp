/*
 * Terminal.hpp
 *
 *  Created on: Feb 2, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
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
