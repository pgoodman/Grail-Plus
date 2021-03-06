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
    class Symbol : public Term<AlphaT> {
    private:

        friend class TDOP<AlphaT>;
        friend class Term<AlphaT>;
        friend class Operator<AlphaT>;
        friend class detail::SymbolGenerator<AlphaT>;

        FLTL_TDOP_USE_TYPES(TDOP<AlphaT>);

        Symbol(unsigned id) throw()
            : Term<AlphaT>(static_cast<int32_t>(id) * -1)
        { }

    public:

        typedef symbol_tag tag_type;

        /// constructors

        Symbol(void) throw()
            : Term<AlphaT>(Term<AlphaT>::DEFAULT_SYMBOL)
        { }

        Symbol(const symbol_type &that) throw()
            : Term<AlphaT>(that)
        { }

        Symbol(const term_type &that) throw() {
            assert(that.is_symbol());
            this->val = that.val;
        }

        /// assignment operators
        symbol_type operator=(const symbol_type &that) throw() {
            this->val = that.val;
            return *this;
        }

        symbol_type operator=(const term_type &that) throw() {
            assert(that.is_symbol());
            this->val = that.val;
            return *this;
        }

        /// operator, followed-by
        const operator_type
        operator*(void) const throw() {
            operator_type op(*this, true);
            return op;
        }

        /// pattern matching
        const Unbound<AlphaT, symbol_tag>
        operator~(void) const throw() {
            return Unbound<AlphaT, symbol_tag>(this);
        }

        /// properties
        unsigned number(void) const throw() {
            return static_cast<unsigned>(this->val * -1);
        }
    };

}}

#endif /* Grail_Plus_TDOP_SYMBOL_HPP_ */
