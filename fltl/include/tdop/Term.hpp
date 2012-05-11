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
    private:

        friend class Symbol<AlphaT>;

        FLTL_TDOP_USE_TYPES(TDOP<AlphaT>);

        union {
            Category<AlphaT> *as_category;
            Symbol<AlphaT> as_symbol;
        } u;

        enum {
            CATEGORY,
            SYMBOL,
            UNBOUND
        } kind;

    public:

        /// defualt constructor
        Term(void) throw()
            : kind(UNBOUND)
        { }

        /// copy constructor, another term.
        Term(const term_type &term) throw()
            : kind(term.kind)
        {
            memcpy(&u, &(term.u), sizeof u);
            if(CATEGORY == kind) {
                u.as_category->incref();
            }
        }

        /// copy constructor, a parser category. the parser category is allowed
        /// to be none in this case, but if so then the term will be unbound.
        Term(const category_type cat) throw()
            : kind(CATEGORY)
        {
            if(0 == cat.category) {
                kind = UNBOUND;
            } else {
                u.as_category = cat.category;
                u.as_category->incref();
            }
        }

        /// copy constructor, symbol type. the symbol type is allowed to be
        /// be undefined, but if so, the term will be unbound.
        Term(const symbol_type sym) throw()
            : kind(SYMBOL)
        {
            if(symbol_type::UNDEFINED_SYMBOL == sym) {
                kind = UNBOUND;
            } else {
                u.as_symbol = sym;
            }
        }

        /// destructor.
        ~Term(void) throw() {
            if(CATEGORY == kind) {
                assert(0 != u.as_category);
                u.as_category->decref();
            }

            kind = UNBOUND;
            memset(&u, 0, sizeof u);
        }

        /// copy constructor; this is a bit evil
        template <typename T>
        term_type &operator=(const T &that) throw() {
            if(reinterpret_cast<const void *>(&that) \
            == reinterpret_cast<const void *>(this)) {
                return *this;
            }
            this->~Term();
            new (this) term_type(that);
            return *this;
        }

        /// type checking
        bool is_category(void) const throw() {
            return CATEGORY == kind;
        }

        bool is_symbol(void) const throw() {
            return SYMBOL == kind;
        }

        bool is_unbound(void) const throw() {
            return UNBOUND == kind;
        }

        bool operator!(void) const throw() {
            return UNBOUND == kind;
        }

        /// comparison operators

        bool operator<(const term_type &that) const throw() {
            if(kind < that.kind) {
                return true;
            } else if(kind > that.kind) {
                return false;
            } else if(UNBOUND == kind) {
                return false;
            } else if(SYMBOL == kind) {
                return u.as_symbol < that.u.as_symbol;
            } else {
                return u.as_category < that.u.as_category;
            }
        }

        bool operator==(const term_type &that) const throw() {
            if(kind != that.kind) {
                return false;
            } else if(UNBOUND == kind) {
                return true;
            } else if(SYMBOL == kind) {
                return u.as_symbol == that.u.as_symbol;
            } else {
                return u.as_category == that.u.as_category;
            }
        }

        bool operator!=(const term_type &that) const throw() {
            return !(operator==(that));
        }

        /// pattern matching

        const Unbound<term_type> operator~(void) const throw() {
            // TODO
        }
    };

}}


#endif /* Grail_Plus_TDOP_TERM_HPP_ */
