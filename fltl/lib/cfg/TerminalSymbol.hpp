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

namespace fltl { namespace lib { namespace cfg {

    template <typename AlphaT>
    class TerminalSymbol : public Symbol<AlphaT> {
    private:

        friend class CFG<AlphaT>;
        friend class detail::PatternData<AlphaT>;
        friend struct std::less<TerminalSymbol<AlphaT> >;

        explicit TerminalSymbol(const internal_sym_type _value) throw()
            : cfg::Symbol<AlphaT>(_value)
        { }

    public:

        TerminalSymbol(void) throw()
            : Symbol<AlphaT>(-1)
        { }

        /// return an "unbound" version of this symbol
        /// note: *not* const!!
        Unbound<AlphaT,terminal_tag> operator~(void) throw() {
            return Unbound<AlphaT,terminal_tag>(this);
        }
    };
}}}

namespace std {

    template <typename AlphaT>
    struct less<fltl::lib::cfg::TerminalSymbol<AlphaT> > : binary_function <fltl::lib::cfg::TerminalSymbol<AlphaT>,fltl::lib::cfg::TerminalSymbol<AlphaT>,bool> {
    public:
        bool operator() (const fltl::lib::cfg::TerminalSymbol<AlphaT> &x, const fltl::lib::cfg::TerminalSymbol<AlphaT> &y) const {
            return x.value < y.value;
        }
    };
}

#endif /* FLTL_TERMINAL_HPP_ */
