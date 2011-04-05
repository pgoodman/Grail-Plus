/*
 * VariableSymbol.hpp
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

#ifndef FLTL_VARIABLESYMBOL_HPP_
#define FLTL_VARIABLESYMBOL_HPP_

#include <functional>

namespace fltl { namespace cfg {

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

        VariableSymbol(const self_type &that) throw()
            : Symbol<AlphaT>(that)
        { }

        VariableSymbol(const Symbol<AlphaT> &that) throw()
            : Symbol<AlphaT>(that)
        {
            assert(that.is_variable());
        }

#if !FLTL_FEATURE_USER_DEFINED_OPERATORS
        /// return an "unbound" version of this symbol
        /// note: *not* const!!
        Unbound<AlphaT,variable_tag> operator~(void) throw() {
            return Unbound<AlphaT,variable_tag>(this);
        }
#endif

        self_type &operator=(const Symbol<AlphaT> &that) throw() {
            assert(that.is_variable());
            this->value = that.value;
            return *this;
        }

#if !FLTL_FEATURE_USER_DEFINED_OPERATORS


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
#endif

        unsigned number(void) const throw() {
            return static_cast<unsigned>(this->value);
        }

        FLTL_CFG_PRODUCTION_PATTERN(variable_tag)

#if FLTL_FEATURE_USER_DEFINED_OPERATORS
        FLTL_USER_BITWISE_OPERATORS_UNARY(AlphaT, variable_tag)
        FLTL_USER_LOGICAL_OPERATORS(AlphaT, variable_tag)
        FLTL_USER_ARITHMETIC_OPERATORS_BINARY(AlphaT, variable_tag)
        FLTL_USER_RELATION_OPERATORS(AlphaT, variable_tag)
#endif

    };

}}

#if FLTL_FEATURE_USER_DEFINED_OPERATORS
namespace fltl { namespace mpl {

    /// unbound symbol used in pattern matching
    template <typename AlphaT,typename ScopeT>
    class OpUnaryBitwiseNot<AlphaT, cfg::variable_tag, ScopeT> {
    public:
        typedef cfg::Unbound<AlphaT,cfg::variable_tag> return_type;

        inline static return_type
        run(cfg::VariableSymbol<AlphaT> *self) throw() {
            return cfg::Unbound<AlphaT,cfg::variable_tag>(self);
        }
    };

    /// are two symbols equivalent?
    template <typename AlphaT,typename ScopeT>
    class OpBinaryEq<AlphaT, cfg::variable_tag, ScopeT, cfg::SymbolString<AlphaT> >
     : public OpBinaryEq<AlphaT, cfg::symbol_tag, ScopeT, cfg::SymbolString<AlphaT> >
    { };

    template <typename AlphaT,typename ScopeT>
    class OpBinaryEq<AlphaT, cfg::variable_tag, ScopeT, cfg::Symbol<AlphaT> >
     : public OpBinaryEq<AlphaT, cfg::symbol_tag, ScopeT, cfg::Symbol<AlphaT> >
    { };

    template <typename AlphaT,typename ScopeT>
    class OpBinaryEq<AlphaT, cfg::variable_tag, ScopeT, cfg::VariableSymbol<AlphaT> >
     : public OpBinaryEq<AlphaT, cfg::symbol_tag, ScopeT, cfg::VariableSymbol<AlphaT> >
    { };

    template <typename AlphaT,typename ScopeT>
    class OpBinaryEq<AlphaT, cfg::variable_tag, ScopeT, cfg::TerminalSymbol<AlphaT> >
     : public OpBinaryEq<AlphaT, cfg::symbol_tag, ScopeT, cfg::TerminalSymbol<AlphaT> >
    { };

    template <typename AlphaT,typename ScopeT>
    class OpBinaryNotEq<AlphaT, cfg::variable_tag, ScopeT, cfg::SymbolString<AlphaT> >
     : public OpBinaryNotEq<AlphaT, cfg::symbol_tag, ScopeT, cfg::SymbolString<AlphaT> >
    { };

    /// are two symbols different?
    template <typename AlphaT,typename ScopeT>
    class OpBinaryNotEq<AlphaT, cfg::variable_tag, ScopeT, cfg::VariableSymbol<AlphaT> >
     : public OpBinaryNotEq<AlphaT, cfg::symbol_tag, ScopeT, cfg::VariableSymbol<AlphaT> >
    { };

    template <typename AlphaT,typename ScopeT>
    class OpBinaryNotEq<AlphaT, cfg::variable_tag, ScopeT, cfg::TerminalSymbol<AlphaT> >
     : public OpBinaryNotEq<AlphaT, cfg::symbol_tag, ScopeT, cfg::TerminalSymbol<AlphaT> >
    { };

    template <typename AlphaT,typename ScopeT>
    class OpBinaryNotEq<AlphaT, cfg::variable_tag, ScopeT, cfg::Symbol<AlphaT> >
     : public OpBinaryNotEq<AlphaT, cfg::symbol_tag, ScopeT, cfg::Symbol<AlphaT> >
    { };

    /// concatenate two symbols into a string
    template <typename AlphaT,typename ScopeT>
    class OpBinaryPlus<AlphaT, cfg::variable_tag, ScopeT, cfg::Symbol<AlphaT> >
     : public OpBinaryPlus<AlphaT, cfg::symbol_tag, ScopeT, cfg::Symbol<AlphaT> >
    { };

    template <typename AlphaT,typename ScopeT>
    class OpBinaryPlus<AlphaT, cfg::variable_tag, ScopeT, cfg::VariableSymbol<AlphaT> >
     : public OpBinaryPlus<AlphaT, cfg::symbol_tag, ScopeT, cfg::Symbol<AlphaT> >
    { };

    template <typename AlphaT,typename ScopeT>
    class OpBinaryPlus<AlphaT, cfg::variable_tag, ScopeT, cfg::TerminalSymbol<AlphaT> >
     : public OpBinaryPlus<AlphaT, cfg::symbol_tag, ScopeT, cfg::Symbol<AlphaT> >
    { };

    /// concatenate a variable with a symbol string
    template <typename AlphaT,typename ScopeT>
    class OpBinaryPlus<AlphaT, cfg::variable_tag, ScopeT, cfg::SymbolString<AlphaT> >
      : public OpBinaryPlus<AlphaT, cfg::symbol_tag, ScopeT, cfg::SymbolString<AlphaT> >
    { };

    template <typename AlphaT,typename ScopeT>
    class OpBinaryLt<AlphaT, cfg::variable_tag, ScopeT, cfg::VariableSymbol<AlphaT> > {
    public:
        typedef bool return_type;
        typedef const cfg::VariableSymbol<AlphaT> param_type;

        inline static return_type
        run(const cfg::VariableSymbol<AlphaT> *self, param_type &that) throw() {
            return self->number() < that.number();
        }
    };

    template <typename AlphaT,typename ScopeT>
    class OpBinaryLtEq<AlphaT, cfg::variable_tag, ScopeT, cfg::VariableSymbol<AlphaT> > {
    public:
        typedef bool return_type;
        typedef const cfg::VariableSymbol<AlphaT> param_type;

        inline static return_type
        run(const cfg::VariableSymbol<AlphaT> *self, param_type &that) throw() {
            return self->number() <= that.number();
        }
    };

    template <typename AlphaT,typename ScopeT>
    class OpBinaryGt<AlphaT, cfg::variable_tag, ScopeT, cfg::VariableSymbol<AlphaT> > {
    public:
        typedef bool return_type;
        typedef const cfg::VariableSymbol<AlphaT> param_type;

        inline static return_type
        run(const cfg::VariableSymbol<AlphaT> *self, param_type &that) throw() {
            return self->number() > that.number();
        }
    };

    template <typename AlphaT,typename ScopeT>
    class OpBinaryGtEq<AlphaT, cfg::variable_tag, ScopeT, cfg::VariableSymbol<AlphaT> > {
    public:
        typedef bool return_type;
        typedef const cfg::VariableSymbol<AlphaT> param_type;

        inline static return_type
        run(const cfg::VariableSymbol<AlphaT> *self, param_type &that) throw() {
            return self->number() >= that.number();
        }
    };
}}
#endif

namespace std {

    template <typename AlphaT>
    struct less<fltl::cfg::VariableSymbol<AlphaT> > : binary_function <fltl::cfg::VariableSymbol<AlphaT>,fltl::cfg::VariableSymbol<AlphaT>,bool> {
    public:
        bool operator() (const fltl::cfg::VariableSymbol<AlphaT> &x, const fltl::cfg::VariableSymbol<AlphaT> &y) const {
            return x.value < y.value;
        }
    };
}

#endif /* FLTL_VARIABLESYMBOL_HPP_ */
