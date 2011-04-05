/*
 * Symbol.hpp
 *
 *  Created on: Jan 20, 2011
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

#ifndef FLTL_SYMBOL_HPP_
#define FLTL_SYMBOL_HPP_

namespace fltl { namespace cfg {

    /// grammar symbol (non-terminal or terminal)
    template <typename AlphaT>
    class Symbol {
    protected:

        mutable internal_sym_type value;

        friend class CFG<AlphaT>;
        friend class Variable<AlphaT>;
        friend class Production<AlphaT>;
        friend class OpaqueProduction<AlphaT>;
        friend class ProductionBuilder<AlphaT>;
        friend class SymbolString<AlphaT>;
        friend class VariableSymbol<AlphaT>;
        friend class TerminalSymbol<AlphaT>;
        friend class detail::SimpleGenerator<AlphaT>;
        friend class detail::PatternData<AlphaT>;

        template <typename, typename>
        friend class detail::PatternGenerator;

        template <typename, typename, typename, typename>
        friend class mpl::OpBinaryEq;

        template <typename, typename, typename, typename>
        friend class mpl::OpBinaryNotEq;

        template <typename, typename, typename, typename>
        friend class mpl::OpBinaryPlus;

        typedef SymbolString<AlphaT> symbol_string_type;

        /// adapted from fmix32, from Murmurhash3, by Austin Appleby.
        // this function is adapted from code that is licensed under the
        // MIT license
        inline static internal_sym_type
        mix32(const internal_sym_type _value) throw() {
            uint32_t h(static_cast<uint32_t>(_value));
            h ^= h >> 16;
            h *= 0x85ebca6b;
            h ^= h >> 13;
            h *= 0xc2b2ae35;
            h ^= h >> 16;
            return static_cast<internal_sym_type>(h);
        }

        FLTL_FORCE_INLINE internal_sym_type
        hash(void) const throw() {
            return mix32(value);
        }

        typedef Symbol<AlphaT> self_type;

        explicit Symbol(const internal_sym_type _value) throw()
            : value(_value)
        { }

    public:

        Symbol(void) throw()
            : value(1)
        { }

        Symbol(const self_type &that) throw()
            : value(that.value)
        { }

        ~Symbol(void) { }

        inline self_type &operator=(const self_type &that) throw() {
            value = that.value;
            return *this;
        }

#if !FLTL_FEATURE_USER_DEFINED_OPERATORS

        FLTL_FORCE_INLINE bool
        operator==(const self_type &that) const throw() {
            return value == that.value;
        }

        FLTL_FORCE_INLINE bool
        operator!=(const self_type &that) const throw() {
            return value != that.value;
        }

        inline bool operator==(const symbol_string_type &that) const throw() {
            return (
                length() == that.length() && hash() == that.get_hash()
            );
        }

        inline bool operator!=(const symbol_string_type &that) const throw() {
            return (
                length() != that.length() || hash() != that.get_hash()
            );
        }
#endif

        inline unsigned length(void) const throw() {
            return 1U;
        }

#if !FLTL_FEATURE_USER_DEFINED_OPERATORS

        /// concatenate this symbol with a symbol string, i.e. prepend this
        /// symbol onto the beginning of the other symbol string and return
        /// the result
        FLTL_FORCE_INLINE const symbol_string_type
        operator+(symbol_string_type that) const throw() {
            return that.prepend_symbol(this);
        }

        /// concatenate this symbol with another symbol and return the symbol
        /// string.
        const symbol_string_type
        operator+(const self_type that) const throw() {
            symbol_string_type ret;
            const unsigned this_len = length();
            const unsigned that_len = that.length();
            const unsigned total_len(this_len + that_len);

            if(0 == total_len) {
                return ret;
            }

            ret.symbols = symbol_string_type::allocate(total_len);
            ret.symbols[str::FIRST_SYMBOL].value = value;
            ret.symbols[
                str::FIRST_SYMBOL + this_len
            ].value = that.value;

            if(0 == this_len) {
                ret.symbols[str::HASH].value = that.hash();
            } else if(0 == that_len) {
                ret.symbols[str::HASH].value = hash();
            } else {
                ret.symbols[str::HASH].value = (
                    symbol_string_type::hash(
                        hash(),
                        that.hash()
                    )
                );
            }

            return ret;
        }
#endif

        /// is this a variable?
        FLTL_FORCE_INLINE bool is_variable(void) const throw() {
            return value > 0;
        }

        /// is this a terminal?
        FLTL_FORCE_INLINE bool is_terminal(void) const throw() {
            return value < 0;
        }

        /// return a numeric value for this symbol. if this symbol is a
        /// variable then this is equivalent to variable_type(sym).number().
        /// If this symbol is a terminal then this is equivalent to
        /// terminal_type(sym).number()
        unsigned number(void) const throw() {
            if(value < 0) {
                return static_cast<unsigned>(value * -1);
            } else {
                return static_cast<unsigned>(value);
            }
        }

#if !FLTL_FEATURE_USER_DEFINED_OPERATORS

        /// return an "unbound" version of this symbol
        /// note: *not* const!!
        Unbound<AlphaT,symbol_tag> operator~(void) throw() {
            return Unbound<AlphaT,symbol_tag>(this);
        }
#endif

#if FLTL_FEATURE_USER_DEFINED_OPERATORS
        FLTL_USER_BITWISE_OPERATORS_UNARY(AlphaT, symbol_tag)
        FLTL_USER_LOGICAL_OPERATORS(AlphaT, symbol_tag)
        FLTL_USER_ARITHMETIC_OPERATORS_BINARY(AlphaT, symbol_tag)
        FLTL_USER_RELATION_OPERATORS(AlphaT, symbol_tag)
#endif

    };
}}

#if FLTL_FEATURE_USER_DEFINED_OPERATORS
namespace fltl { namespace mpl {

    /// unbound symbol used in pattern matching
    template <typename AlphaT,typename ScopeT>
    class OpUnaryBitwiseNot<AlphaT, cfg::symbol_tag, ScopeT> {
    public:
        typedef cfg::Unbound<AlphaT,cfg::symbol_tag> return_type;

        inline static return_type
        run(cfg::Symbol<AlphaT> *self) throw() {
            return cfg::Unbound<AlphaT,cfg::symbol_tag>(self);
        }
    };

    template <typename AlphaT,typename ScopeT>
    class OpBinaryEq<AlphaT, cfg::symbol_tag, ScopeT, cfg::SymbolString<AlphaT> > {
    public:
        typedef bool return_type;
        typedef const cfg::SymbolString<AlphaT> param_type;

        inline static return_type
        run(const cfg::Symbol<AlphaT> *self, param_type &that) throw() {
            return self->length() == that.length() && self->hash() == that.get_hash();
        }
    };

    /// are two symbols equivalent?
    template <typename AlphaT,typename ScopeT>
    class OpBinaryEq<AlphaT, cfg::symbol_tag, ScopeT, cfg::Symbol<AlphaT> > {
    public:
        typedef bool return_type;
        typedef const cfg::Symbol<AlphaT> param_type;

        inline static return_type
        run(const cfg::Symbol<AlphaT> *self, param_type &that) throw() {
            return self->value == that.value;
        }
    };

    template <typename AlphaT,typename ScopeT>
    class OpBinaryEq<AlphaT, cfg::symbol_tag, ScopeT, cfg::VariableSymbol<AlphaT> >
     : public OpBinaryEq<AlphaT, cfg::symbol_tag, ScopeT, cfg::Symbol<AlphaT> >
    { };

    template <typename AlphaT,typename ScopeT>
    class OpBinaryEq<AlphaT, cfg::symbol_tag, ScopeT, cfg::TerminalSymbol<AlphaT> >
     : public OpBinaryEq<AlphaT, cfg::symbol_tag, ScopeT, cfg::Symbol<AlphaT> >
    { };

    template <typename AlphaT,typename ScopeT>
    class OpBinaryNotEq<AlphaT, cfg::symbol_tag, ScopeT, cfg::SymbolString<AlphaT> > {
    public:
        typedef bool return_type;
        typedef const cfg::SymbolString<AlphaT> param_type;

        inline static return_type
        run(const cfg::Symbol<AlphaT> *self, param_type &that) throw() {
            return self->length() != that.length() || self->hash() != that.get_hash();
        }
    };

    /// are two symbols different?
    template <typename AlphaT,typename ScopeT>
    class OpBinaryNotEq<AlphaT, cfg::symbol_tag, ScopeT, cfg::Symbol<AlphaT> > {
    public:
        typedef bool return_type;
        typedef const cfg::Symbol<AlphaT> param_type;

        inline static return_type
        run(const cfg::Symbol<AlphaT> *self, param_type &that) throw() {
            return self->value != that.value;
        }
    };

    template <typename AlphaT,typename ScopeT>
    class OpBinaryNotEq<AlphaT, cfg::symbol_tag, ScopeT, cfg::VariableSymbol<AlphaT> >
     : public OpBinaryNotEq<AlphaT, cfg::symbol_tag, ScopeT, cfg::Symbol<AlphaT> >
    { };

    template <typename AlphaT,typename ScopeT>
    class OpBinaryNotEq<AlphaT, cfg::symbol_tag, ScopeT, cfg::TerminalSymbol<AlphaT> >
     : public OpBinaryNotEq<AlphaT, cfg::symbol_tag, ScopeT, cfg::Symbol<AlphaT> >
    { };

    /// concatenate two symbols into a string
    template <typename AlphaT,typename ScopeT>
    class OpBinaryPlus<AlphaT, cfg::symbol_tag, ScopeT, cfg::Symbol<AlphaT> > {
    public:
        typedef cfg::SymbolString<AlphaT> return_type;
        typedef const cfg::Symbol<AlphaT> param_type;

        inline static return_type
        run(const cfg::Symbol<AlphaT> *self, param_type &that) throw() {
            return_type ret;
            const unsigned this_len = self->length();
            const unsigned that_len = that.length();
            const unsigned total_len(this_len + that_len);

            if(0 == total_len) {
                return ret;
            }

            ret.symbols = return_type::allocate(total_len);
            ret.symbols[cfg::str::FIRST_SYMBOL].value = self->value;
            ret.symbols[
                cfg::str::FIRST_SYMBOL + this_len
            ].value = that.value;

            if(0 == this_len) {
                ret.symbols[cfg::str::HASH].value = that.hash();
            } else if(0 == that_len) {
                ret.symbols[cfg::str::HASH].value = self->hash();
            } else {
                ret.symbols[cfg::str::HASH].value = (
                    return_type::hash(
                        self->hash(),
                        that.hash()
                    )
                );
            }

            return ret;
        }
    };

    template <typename AlphaT,typename ScopeT>
    class OpBinaryPlus<AlphaT, cfg::symbol_tag, ScopeT, cfg::VariableSymbol<AlphaT> >
     : public OpBinaryPlus<AlphaT, cfg::symbol_tag, ScopeT, cfg::Symbol<AlphaT> >
    { };

    template <typename AlphaT,typename ScopeT>
    class OpBinaryPlus<AlphaT, cfg::symbol_tag, ScopeT, cfg::TerminalSymbol<AlphaT> >
     : public OpBinaryPlus<AlphaT, cfg::symbol_tag, ScopeT, cfg::Symbol<AlphaT> >
    { };

    /// concatenate a symbol with a symbol string
    template <typename AlphaT,typename ScopeT>
    class OpBinaryPlus<AlphaT, cfg::symbol_tag, ScopeT, cfg::SymbolString<AlphaT> > {
    public:
        typedef cfg::SymbolString<AlphaT> return_type;
        typedef const cfg::SymbolString<AlphaT> param_type;

        inline static return_type
        run(const cfg::Symbol<AlphaT> *self, param_type &that) throw() {
            return that.prepend_symbol(self);
        }
    };
}}
#endif

#endif /* FLTL_SYMBOL_HPP_ */
