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

namespace fltl { namespace lib { namespace cfg {

    template <typename AlphaT>
    class TerminalSymbol : public Symbol<AlphaT> {
    private:

        friend class CFG<AlphaT>;

        explicit TerminalSymbol(const internal_sym_type _value) throw()
            : cfg::Symbol<AlphaT>(_value)
        { }

    public:

        TerminalSymbol(void) throw()
            : Symbol<AlphaT>(-1)
        { }

        /// return an "unbound" version of this symbol
        /// note: *not* const!!
        Unbound<AlphaT,TerminalSymbol<AlphaT> > operator~(void) throw() {
            return Unbound<AlphaT,TerminalSymbol<AlphaT> >(this);
        }
    };
}}}

#endif /* FLTL_TERMINAL_HPP_ */
