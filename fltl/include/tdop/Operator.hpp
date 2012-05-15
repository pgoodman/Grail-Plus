/*
 * Operator.hpp
 *
 *  Created on: May 11, 2012
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef Grail_Plus_TDOP_OPERATOR_HPP_
#define Grail_Plus_TDOP_OPERATOR_HPP_

namespace fltl { namespace tdop {

    /// represents an operator within a rule
    template <typename AlphaT>
    class Operator {
    private:

        template <typename, typename, const unsigned, typename, typename>
        friend class Match;

        friend class OpaqueCategory<AlphaT>;
        friend class Symbol<AlphaT>;

        FLTL_TDOP_USE_TYPES(TDOP<AlphaT>);

        term_type term;
        int32_t lower_bound;

        enum {
            UNINITIALIZED = -1,
            IS_UNBOUND = -2,
            IS_PREDICATE = -3
        };

    public:

        typedef operator_tag tag_type;

        /// constructors

        Operator(void) throw()
            : term()
            , lower_bound(UNINITIALIZED)
        { }

        Operator(const operator_type that) throw()
            : term(that.term)
            , lower_bound(that.lower_bound)
        { }

        Operator(const term_type that) throw()
            : term(that)
            , lower_bound(UNINITIALIZED)
        {
            if(term.is_category()) {
                lower_bound = IS_UNBOUND;
            }
        }

        Operator(const symbol_type that) throw()
            : term(that)
            , lower_bound(UNINITIALIZED)
        { }

        Operator(const symbol_type that, bool) throw()
            : term(that)
            , lower_bound(IS_PREDICATE)
        { }

        Operator(const category_type that) throw()
            : term(that)
            , lower_bound(IS_UNBOUND)
        { }

        Operator(const category_type that, unsigned lb) throw()
            : term(that)
            , lower_bound(static_cast<int32_t>(lb))
        { }

        /// assignment

        operator_type &operator=(const operator_type that) throw() {
            term = that.term;
            lower_bound = that.lower_bound;
            return *this;
        }

        operator_type &operator=(const term_type that) throw() {
            term = that;
            if(term.is_category()) {
                lower_bound = IS_UNBOUND;
            } else {
                lower_bound = UNINITIALIZED;
            }
            return *this;
        }

        operator_type &operator=(const symbol_type that) throw() {
            term = that;
            lower_bound = UNINITIALIZED;
            return *this;
        }

        operator_type &operator=(const category_type that) throw() {
            term = that;
            lower_bound = IS_UNBOUND;
            return *this;
        }

        /// destructuring

        bool match(symbol_type &sym) const throw() {
            if(!term.is_symbol()) {
                return false;
            }

            sym.val = term.val;

            return true;
        }

        bool match(category_type &cat) const throw() {
            if(!term.is_category()) {
                return false;
            }

            cat = term.val;

            return true;
        }

        bool match(category_type &cat, unsigned &bound) const throw() {
            if(!term.is_category()) {
                return false;
            }

            if(IS_UNBOUND == lower_bound) {
                return false;
            }

            cat.val = term.val;
            bound = static_cast<unsigned>(lower_bound);

            return true;
        }

        /// type/state checking

        bool is_category(void) const throw() {
            return term.is_category();
        }

        bool is_symbol(void) const throw() {
            return term.is_symbol();
        }

        bool is_valid(void) const throw() {
            return term.is_valid();
        }

        bool is_symbol_predicate(void) const throw() {
            return term.is_symbol() && IS_PREDICATE == lower_bound;
        }

        bool is_bound_category(void) const throw() {
            return term.is_category() && IS_UNBOUND != lower_bound;
        }

        /// comparison

        bool operator==(const operator_type &that) const throw() {
            return term == that.term && lower_bound == that.lower_bound;
        }
        bool operator!=(const operator_type &that) const throw() {
            return term != that.term || lower_bound != that.lower_bound;
        }

#define FLTL_TDOP_OPERATOR_CAT(type) \
    const operator_string_type \
    operator+(const type that) throw() { \
        operator_type arr[2] = {*this, operator_type(that)}; \
        operator_string_type str(&(arr[0]), 2); \
        return str; \
    }

        /// extension into a string
        FLTL_TDOP_OPERATOR_CAT(operator_type)
        FLTL_TDOP_OPERATOR_CAT(term_type)
        FLTL_TDOP_OPERATOR_CAT(symbol_type)
        FLTL_TDOP_OPERATOR_CAT(category_type)

        const operator_string_type
        operator+(const operator_string_type &that) throw() {
            operator_type str(1 + that.length());
            operator_type::append(str.arr, *this, 0);
            operator_type::append(str.arr, that.arr, 1);
            return str;
        }

#undef FLTL_TDOP_OPERATOR_APPEND

        /// unbound operator
        const Unbound<AlphaT,operator_tag>
        operator~(void) const throw() {
            return Unbound<AlphaT,operator_tag>(this);
        }
    };

}}

#endif /* Grail_Plus_TDOP_OPERATOR_HPP_ */
