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
        FLTL_CFG_PRODUCTION_PATTERN(any_symbol_tag)
    };

    template <typename AlphaT>
    class AnySymbolString { };

    /// represents an unbound symbol in a query; the key to how unbound
    /// symbols work is that any symbol producing functions return const
    /// references to their symbols, whereas an unbound symbol, by means
    /// of being instantiated with the ~ operator, must operator on a non-
    /// const symbol.

    template <typename AlphaT>
    class Unbound<AlphaT, symbol_tag> {

        friend class CFG<AlphaT>;
        friend class Symbol<AlphaT>;
        friend class TerminalSymbol<AlphaT>;
        friend class VariableSymbol<AlphaT>;
        friend class detail::SimpleGenerator<AlphaT>;
        template <typename, typename> friend class Pattern;
        friend class PatternData<AlphaT>;

        template <typename, typename, const unsigned, typename, typename>
        friend class detail::Match2;

        template <typename, typename, typename>
        friend class detail::DestructuringBind;

    protected:

        Symbol<AlphaT> *symbol;

        typedef Unbound<AlphaT, symbol_tag> self_type;

        Unbound(Symbol<AlphaT> *_symbol)
            : symbol(_symbol)
        { }

    public:

        bool operator==(const self_type &that) const throw() {
            return symbol == that.symbol;
        }
    };

    template <typename AlphaT>
    class Unbound<AlphaT, terminal_tag> {
    private:

        friend class CFG<AlphaT>;
        friend class Symbol<AlphaT>;
        friend class TerminalSymbol<AlphaT>;
        friend class VariableSymbol<AlphaT>;
        friend class detail::SimpleGenerator<AlphaT>;
        template <typename, typename> friend class Pattern;
        friend class PatternData<AlphaT>;

        template <typename, typename, const unsigned, typename, typename>
        friend class detail::Match2;

        template <typename, typename, typename>
        friend class detail::DestructuringBind;

        typedef Unbound<AlphaT, terminal_tag> self_type;

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
    class Unbound<AlphaT, variable_tag> {

        friend class CFG<AlphaT>;
        friend class Symbol<AlphaT>;
        friend class TerminalSymbol<AlphaT>;
        friend class VariableSymbol<AlphaT>;
        friend class detail::SimpleGenerator<AlphaT>;
        template <typename, typename> friend class Pattern;
        friend class PatternData<AlphaT>;

        template <typename, typename, const unsigned, typename, typename>
        friend class detail::Match2;

        template <typename, typename, typename>
        friend class detail::DestructuringBind;

    private:

        typedef Unbound<AlphaT, variable_tag> self_type;

        typedef typename CFG<AlphaT>::variable_type variable_type;

        variable_type *symbol;

        Unbound(typename CFG<AlphaT>::variable_type *_var)
            : symbol(_var)
        { }

    public:

        FLTL_CFG_PRODUCTION_PATTERN(unbound_variable_tag)

        bool operator==(const self_type &that) const throw() {
            return symbol == that.symbol;
        }
    };

    /// an unbound production
    template <typename AlphaT>
    class Unbound<AlphaT, production_tag> {
    private:

        friend class CFG<AlphaT>;
        friend class OpaqueProduction<AlphaT>;
        friend class detail::SimpleGenerator<AlphaT>;

        template <typename, typename, const unsigned, typename, typename>
        friend class detail::Match2;

        template <typename, typename, typename>
        friend class detail::DestructuringBind;

        friend class PatternData<AlphaT>;

        typedef Unbound<AlphaT, production_tag> self_type;

        OpaqueProduction<AlphaT> *prod;

        Unbound(OpaqueProduction<AlphaT> *_prod)
            : prod(_prod)
        { }
    };

    /// an unbound symbol string
    template <typename AlphaT>
    class Unbound<AlphaT, symbol_string_tag> {
    private:

        friend class CFG<AlphaT>;
        friend class OpaqueProduction<AlphaT>;
        friend class detail::SimpleGenerator<AlphaT>;
        friend class SymbolString<AlphaT>;

        template <typename, typename> friend class Pattern;
        friend class PatternData<AlphaT>;

        template <typename, typename, const unsigned, typename, typename>
        friend class detail::Match2;

        template <typename, typename, typename>
        friend class detail::DestructuringBind;

        template <typename, typename, typename, const unsigned>
        friend class detail::ResetPattern;

        typedef Unbound<AlphaT, symbol_string_tag> self_type;

        SymbolString<AlphaT> *string;

        Unbound(SymbolString<AlphaT> *_string)
            : string(_string)
        { }
    };
}}}

#endif /* FLTL_UNBOUNDSYMBOL_HPP_ */
