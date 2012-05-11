/*
 * Terminal.hpp
 *
 *  Created on: May 10, 2012
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef Grail_Plus_TDOP_SYMBOL_HPP_
#define Grail_Plus_TDOP_SYMBOL_HPP_

namespace fltl { namespace tdop {

    /// represents a symbol/terminal/letter of a string in the language
    /// accepted by the TDOP machine.
    template <typename AlphaT>
    class Symbol {
    private:

        friend class TDOP<AlphaT>;
        friend class Term<AlphaT>;

        enum {
            UNDEFINED_SYMBOL = 0
        };

        uint32_t number;

        FLTL_TDOP_USE_TYPES(TDOP<AlphaT>);

    public:

        /// constructors

        Symbol(void) throw()
            : number(UNDEFINED_SYMBOL)
        { }

        Symbol(const symbol_type &that) throw()
            : number(that.number)
        { }

        Symbol(const term_type &that) throw() {
            assert(that.is_symbol());
            number = that.u.as_symbol.number;
        }

        /// assignment operators
        symbol_type operator=(const symbol_type &that) throw() {
            number = that.number;
            return *this;
        }

        symbol_type operator=(const term_type &that) throw() {
            assert(that.is_symbol());
            number = that.u.as_symbol.number;
            return *this;
        }

        /// type checking

        bool operator!(void) const throw() {
            return UNDEFINED_SYMBOL == number;
        }

        /// comparison operators

        bool operator==(const symbol_type &that) const throw() {
            return number == that.number;
        }

        bool operator!=(const symbol_type &that) const throw() {
            return number != that.number;
        }

        bool operator<(const symbol_type &that) const throw() {
            return number < that.number;
        }

        /// operator, followed-by
        const operator_type operator&(void) const throw() {
            // TODO
        }

        /// pattern matching

        const Unbound<symbol_type> operator~(void) const throw() {
            // TODO
        }
    };

}}

#endif /* Grail_Plus_TDOP_SYMBOL_HPP_ */
