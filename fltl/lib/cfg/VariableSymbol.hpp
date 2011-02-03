/*
 * VariableSymbol.hpp
 *
 *  Created on: Feb 2, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_VARIABLESYMBOL_HPP_
#define FLTL_VARIABLESYMBOL_HPP_

namespace fltl { namespace lib { namespace cfg {

    /// represents a non-terminal of a grammar
    template <typename AlphaT>
    class VariableSymbol : public Symbol<AlphaT> {
    private:

        friend class CFG<AlphaT>;
        friend class OpaqueProduction<AlphaT>;
        friend class PatternData<AlphaT>;

        typedef VariableSymbol<AlphaT> self_type;

        explicit VariableSymbol(const internal_sym_type _value) throw()
            : Symbol<AlphaT>(_value)
        { }

    public:

        VariableSymbol(void) throw()
            : Symbol<AlphaT>(1)
        { }

        /// return an "unbound" version of this symbol
        /// note: *not* const!!
        Unbound<AlphaT,VariableSymbol<AlphaT> > operator~(void) throw() {
            return Unbound<AlphaT,VariableSymbol<AlphaT> >(this);
        }

        FLTL_CFG_PRODUCTION_PATTERN(variable_tag)
    };

}}}

#endif /* FLTL_VARIABLESYMBOL_HPP_ */
