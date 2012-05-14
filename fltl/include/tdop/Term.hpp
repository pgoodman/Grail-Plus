/*
 * Action.hpp
 *
 *  Created on: May 10, 2012
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef Grail_Plus_TDOP_TERM_HPP_
#define Grail_Plus_TDOP_TERM_HPP_

namespace fltl { namespace tdop {

    /// represents either a symbol or a parser in a TDOP machine.
    template <typename AlphaT>
    class Term {
    protected:

        friend class Symbol<AlphaT>;
        friend class OpaqueCategory<AlphaT>;
        friend class Operator<AlphaT>;

        FLTL_TDOP_USE_TYPES(TDOP<AlphaT>);

        enum {
            UNBOUND = 0,
            DEFAULT_CATEGORY = 1,
            DEFAULT_SYMBOL = -1
        };

        int32_t val;

        Term(int32_t val_) throw()
            : val(val_)
        { }

    public:

        /// defualt constructor
        Term(void) throw()
            : val(UNBOUND)
        { }

        /// copy constructor, another term.
        Term(const term_type &term) throw()
            : val(term.val)
        { }

        /// copy constructor, a parser category. the parser category is allowed
        /// to be none in this case, but if so then the term will be unbound.
        Term(const category_type cat) throw()
            : val(cat.val)
        { }

        /// copy constructor, symbol type. the symbol type is allowed to be
        /// be undefined, but if so, the term will be unbound.
        Term(const symbol_type sym) throw()
            : val(sym.val)
        { }

        /// destructor.
        ~Term(void) throw() {
            val = UNBOUND;
        }

        /// copy constructor; this is a bit evil
        template <typename T>
        term_type &operator=(const T &that) throw() {
            if(reinterpret_cast<const void *>(&that) \
            == reinterpret_cast<const void *>(this)) {
                return *this;
            }
            val = that.val;
            return *this;
        }

        /// type checking
        bool is_category(void) const throw() {
            return 0 < val;
        }

        bool is_symbol(void) const throw() {
            return 0 > val;
        }

        bool is_valid(void) const throw() {
            return UNBOUND != val;
        }

        bool operator!(void) const throw() {
            return UNBOUND == val;
        }

        /// comparison operators

        bool operator<(const term_type &that) const throw() {
            return val < that.val;
        }

        bool operator==(const term_type &that) const throw() {
            return val == that.val;
        }

        bool operator!=(const term_type &that) const throw() {
            return val != that.val;
        }

        bool operator==(const category_type &that) const throw() {
            return val == that.val;
        }

        bool operator!=(const category_type &that) const throw() {
            return val != that.val;
        }

        bool operator==(const symbol_type &that) const throw() {
            return val == that.val;
        }

        bool operator!=(const symbol_type &that) const throw() {
            return val != that.val;
        }

        /// pattern matching

        const Unbound<AlphaT, term_tag> operator~(void) const throw() {
            // TODO
        }
    };

}}


#endif /* Grail_Plus_TDOP_TERM_HPP_ */
