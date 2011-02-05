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

#include <functional>

namespace fltl { namespace lib { namespace cfg {

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
    };

}}}

namespace std {

    template <typename AlphaT>
    struct less<fltl::lib::cfg::VariableSymbol<AlphaT> > : binary_function <fltl::lib::cfg::VariableSymbol<AlphaT>,fltl::lib::cfg::VariableSymbol<AlphaT>,bool> {
    public:
        bool operator() (const fltl::lib::cfg::VariableSymbol<AlphaT> &x, const fltl::lib::cfg::VariableSymbol<AlphaT> &y) const {
            return x.value < y.value;
        }
    };
}

#endif /* FLTL_VARIABLESYMBOL_HPP_ */
