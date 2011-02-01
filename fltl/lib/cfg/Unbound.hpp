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

    template <typename AlphaT>
    class AnySymbol {
    public:
        typedef AnySymbol self_type;
        FLTL_CFG_PRODUCTION_PATTERN
    };

    template <typename AlphaT>
    class AnySymbolString { };

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
        template <typename, typename> friend class Pattern;

        template <typename, typename, const unsigned, typename, typename>
        friend class detail::Match2;

        template <typename, typename, typename, typename>
        friend class detail::DestructuringBind;

    protected:

        Symbol<AlphaT> *symbol;

        typedef Unbound<AlphaT, typename CFG<AlphaT>::symbol_type>
                self_type;

        Unbound(Symbol<AlphaT> *_symbol)
            : symbol(_symbol)
        { }

    public:

        bool operator==(const self_type &that) const throw() {
            return symbol == that.symbol;
        }
    };

    template <typename AlphaT>
    class Unbound<AlphaT, typename CFG<AlphaT>::terminal_type> {
    private:

        friend class CFG<AlphaT>;
        friend class Symbol<AlphaT>;
        friend class CFG<AlphaT>::terminal_type;
        friend class CFG<AlphaT>::variable_type;
        friend class detail::SimpleGenerator<AlphaT>;
        template <typename, typename> friend class Pattern;

        template <typename, typename, const unsigned, typename, typename>
        friend class detail::Match2;

        template <typename, typename, typename, typename>
        friend class detail::DestructuringBind;

        typedef Unbound<AlphaT, typename CFG<AlphaT>::terminal_type> self_type;

        typename CFG<AlphaT>::terminal_type *symbol;

        Unbound(typename CFG<AlphaT>::terminal_type *_term)
            : symbol(_term)
        { }

    public:

        bool operator==(const self_type &that) const throw() {
            return symbol == that.symbol;
        }
    };

    template <typename AlphaT>
    class Unbound<AlphaT, typename CFG<AlphaT>::variable_type> {

        friend class CFG<AlphaT>;
        friend class Symbol<AlphaT>;
        friend class CFG<AlphaT>::terminal_type;
        friend class CFG<AlphaT>::variable_type;
        friend class detail::SimpleGenerator<AlphaT>;
        template <typename, typename> friend class Pattern;

        template <typename, typename, const unsigned, typename, typename>
        friend class detail::Match2;

        template <typename, typename, typename, typename>
        friend class detail::DestructuringBind;

    private:

        typedef Unbound<AlphaT, typename CFG<AlphaT>::variable_type>
                self_type;

        typedef typename CFG<AlphaT>::variable_type variable_type;
        typedef typename CFG<AlphaT>::terminal_type terminal_type;
        typedef typename CFG<AlphaT>::symbol_type symbol_type;
        typedef typename CFG<AlphaT>::symbol_string_type symbol_string_type;

        variable_type *symbol;

        Unbound(typename CFG<AlphaT>::variable_type *_var)
            : symbol(_var)
        { }

    public:
        /*
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

        public:
        */

        FLTL_CFG_PRODUCTION_PATTERN

        bool operator==(const self_type &that) const throw() {
            return symbol == that.symbol;
        }
    };

    /// an unbound production
    template <typename AlphaT>
    class Unbound<AlphaT, OpaqueProduction<AlphaT> > {
    private:

        friend class CFG<AlphaT>;
        friend class OpaqueProduction<AlphaT>;
        friend class detail::SimpleGenerator<AlphaT>;

        template <typename, typename, const unsigned, typename, typename>
        friend class detail::Match2;

        template <typename, typename, typename, typename>
        friend class detail::DestructuringBind;

        typedef Unbound<AlphaT, typename CFG<AlphaT>::production_type>
                self_type;

        OpaqueProduction<AlphaT> *prod;

        Unbound(OpaqueProduction<AlphaT> *_prod)
            : prod(_prod)
        { }
    };

    /// an unbound symbol string
    template <typename AlphaT>
    class Unbound<AlphaT, SymbolString<AlphaT> > {
    private:

        friend class CFG<AlphaT>;
        friend class OpaqueProduction<AlphaT>;
        friend class detail::SimpleGenerator<AlphaT>;
        friend class SymbolString<AlphaT>;

        template <typename, typename, const unsigned, typename, typename>
        friend class detail::Match2;

        template <typename, typename, typename, typename>
        friend class detail::DestructuringBind;

        template <typename, typename, typename, const unsigned>
        friend class detail::ResetPattern;

        typedef Unbound<AlphaT, SymbolString<AlphaT> > self_type;

        SymbolString<AlphaT> *string;

        Unbound(SymbolString<AlphaT> *_string)
            : string(_string)
        { }
    };
}}}

#endif /* FLTL_UNBOUNDSYMBOL_HPP_ */
