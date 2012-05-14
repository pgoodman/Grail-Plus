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

        FLTL_TDOP_USE_TYPES(TDOP<AlphaT>);

    public:

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
