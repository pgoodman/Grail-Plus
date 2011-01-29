/*
 * Unbound.hpp
 *
 *  Created on: Jan 27, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_UNBOUNDSYMBOL_HPP_
#define FLTL_UNBOUNDSYMBOL_HPP_

namespace fltl { namespace lib { namespace cfg {

    /// represents an unbound symbol in a query; the key to how unbound
    /// symbols work is that any symbol producing functions return const
    /// references to their symbols, whereas an unbound symbol, by means
    /// of being instantiated with the ~ operator, must operator on a non-
    /// const symbol.

    template <typename AlphaT>
    class Unbound<AlphaT, typename CFG<AlphaT>::symbol_type> {

        friend class CFG<AlphaT>;
        friend class Symbol<AlphaT>;
        friend class CFG<AlphaT>::terminal_type;
        friend class CFG<AlphaT>::variable_type;
        friend class detail::SimpleGenerator<AlphaT>;

    protected:

        Symbol<AlphaT> *symbol;

        typedef Unbound<AlphaT, typename CFG<AlphaT>::symbol_type>
                self_type;

        Unbound(Symbol<AlphaT> *_symbol)
            : symbol(_symbol)
        { }
    };

    template <typename AlphaT>
    class Unbound<AlphaT, typename CFG<AlphaT>::terminal_type> {
    private:

        friend class CFG<AlphaT>;
        friend class Symbol<AlphaT>;
        friend class CFG<AlphaT>::terminal_type;
        friend class CFG<AlphaT>::variable_type;
        friend class detail::SimpleGenerator<AlphaT>;


        typename CFG<AlphaT>::terminal_type *term;

        Unbound(typename CFG<AlphaT>::terminal_type *_term)
            : term(_term)
        { }
    };

    template <typename AlphaT>
    class Unbound<AlphaT, typename CFG<AlphaT>::variable_type> {

        friend class CFG<AlphaT>;
        friend class Symbol<AlphaT>;
        friend class CFG<AlphaT>::terminal_type;
        friend class CFG<AlphaT>::variable_type;
        friend class detail::SimpleGenerator<AlphaT>;

    private:

        typedef Unbound<AlphaT, typename CFG<AlphaT>::variable_type>
                self_type;

        typedef typename CFG<AlphaT>::variable_type variable_type;
        typedef typename CFG<AlphaT>::terminal_type terminal_type;
        typedef typename CFG<AlphaT>::symbol_type symbol_type;
        typedef typename CFG<AlphaT>::symbol_string_type symbol_string_type;

        variable_type *var;

        Unbound(typename CFG<AlphaT>::variable_type *_var)
            : var(_var)
        { }

    public:

        /// making a query where the variable is (un)bound
        FLTL_CFG_PRODUCTION_PATTERN(variable_type, 0U)
        FLTL_CFG_UNBOUND_PRODUCTION_PATTERN(variable_type, 0U)

        /// making a query where the terminal is (un)bound
        FLTL_CFG_PRODUCTION_PATTERN(terminal_type, 0U)
        FLTL_CFG_UNBOUND_PRODUCTION_PATTERN(terminal_type, 0U)

        /// making a query where the symbol is bound
        FLTL_CFG_PRODUCTION_PATTERN(symbol_type, 0U)
        FLTL_CFG_UNBOUND_PRODUCTION_PATTERN(symbol_type, 0U)

        /// making a query where the symbol string is (un)bound
        FLTL_CFG_PRODUCTION_PATTERN(symbol_string_type, 0U)
        FLTL_CFG_UNBOUND_PRODUCTION_PATTERN(symbol_string_type, 1U)
    };

    /// an unbound production
    template <typename AlphaT>
    class Unbound<AlphaT, typename CFG<AlphaT>::production_type> {
    private:

        friend class CFG<AlphaT>;
        friend class OpaqueProduction<AlphaT>;
        friend class detail::SimpleGenerator<AlphaT>;

        OpaqueProduction<AlphaT> *prod;

        typedef Unbound<AlphaT, typename CFG<AlphaT>::production_type>
                self_type;

        Unbound(OpaqueProduction<AlphaT> *_prod)
            : prod(_prod)
        { }
    };
}}}

#endif /* FLTL_UNBOUNDSYMBOL_HPP_ */
