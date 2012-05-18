/*
 * Unbound.hpp
 *
 *  Created on: May 11, 2012
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef Grail_Plus_TDOP_UNBOUND_HPP_
#define Grail_Plus_TDOP_UNBOUND_HPP_

namespace fltl { namespace tdop {

    template <typename, typename> class Unbound;
    template <typename, typename> class Bound;

    /// unbound symbol
    template <typename AlphaT>
    class Unbound<AlphaT, symbol_tag> {
    private:

        friend class detail::PatternData<AlphaT>;
        friend class Symbol<AlphaT>;

        Symbol<AlphaT> *expr;

        Unbound(const Symbol<AlphaT> *expr_) throw()
            : expr(const_cast<Symbol<AlphaT> *>(expr_))
        { }

    public:

        typedef Unbound<AlphaT, symbol_tag> self_type;
        typedef unbound_symbol_tag tag_type;

        /// unbound predicate symbol, e.g. *(~s), where 's' is a symbol.
        const Unbound<AlphaT, ubound_symbol_predicate_tag>
        operator*(void) const throw() {
            return Unbound<AlphaT, ubound_symbol_predicate_tag>(expr);
        }

        // TODO
    };

    /// unbound predicate symbol
    template <typename AlphaT>
    class Unbound<AlphaT, ubound_symbol_predicate_tag> {
    private:

        friend class detail::PatternData<AlphaT>;
        friend class Unbound<AlphaT, symbol_tag>;

        Symbol<AlphaT> *expr;

        Unbound(const Symbol<AlphaT> *expr_) throw()
            : expr(const_cast<Symbol<AlphaT> *>(expr_))
        { }

    public:

        typedef Unbound<AlphaT, ubound_symbol_predicate_tag> self_type;
        typedef ubound_symbol_predicate_tag tag_type;

        // TODO
    };

    /// unbound operator
    template <typename AlphaT>
    class Unbound<AlphaT, operator_tag> {
    private:

        friend class detail::PatternData<AlphaT>;
        friend class Operator<AlphaT>;

        Operator<AlphaT> *expr;

        Unbound(const Operator<AlphaT> *expr_) throw()
            : expr(const_cast<Operator<AlphaT> *>(expr_))
        { }

    public:

        typedef Unbound<AlphaT, operator_tag> self_type;
        typedef unbound_operator_tag tag_type;

        // TODO
    };

    /// unbound operator string
    template <typename AlphaT>
    class Unbound<AlphaT, operator_string_tag> {
    private:

        friend class detail::PatternData<AlphaT>;
        friend class OperatorString<AlphaT>;

        OperatorString<AlphaT> *expr;

        Unbound(const OperatorString<AlphaT> *expr_) throw()
            : expr(const_cast<OperatorString<AlphaT> *>(expr_))
        { }

    public:

        typedef Unbound<AlphaT, operator_string_tag> self_type;
        typedef unbound_operator_string_tag tag_type;

        // TODO
    };

    /// unbound term
    template <typename AlphaT>
    class Unbound<AlphaT, term_tag> {
    private:

        friend class detail::PatternData<AlphaT>;
        friend class Term<AlphaT>;

        Term<AlphaT> *expr;

        Unbound(const Term<AlphaT> *expr_) throw()
            : expr(const_cast<Term<AlphaT> *>(expr_))
        { }

    public:

        typedef Unbound<AlphaT, term_tag> self_type;
        typedef unbound_term_tag tag_type;

        // TODO
    };

    /// unbound category
    template <typename AlphaT>
    class Unbound<AlphaT, category_tag> {
    private:

        friend class detail::PatternData<AlphaT>;
        friend class OpaqueCategory<AlphaT>;

        OpaqueCategory<AlphaT> *expr;

        Unbound(const OpaqueCategory<AlphaT> *expr_) throw()
            : expr(const_cast<OpaqueCategory<AlphaT> *>(expr_))
        { }

    public:

        typedef Unbound<AlphaT, category_tag> self_type;
        typedef unbound_category_tag tag_type;

        /// (~c)[n], unbound category, unbound lower bound
        const Unbound<AlphaT, category_lb_tag>
        operator[](unsigned &lower_bound) const throw() {
            return Unbound<AlphaT, category_lb_tag>(expr, &lower_bound);
        }

        // an initial rule, with an unbound category
        FLTL_TDOP_RULE_PATTERN(unbound_category_tag)
    };

    /// unbound category, unbound lower bound
    template <typename AlphaT>
    class Unbound<AlphaT, category_lb_tag> {
    private:

        friend class detail::PatternData<AlphaT>;
        friend class Unbound<AlphaT, category_tag>;

        OpaqueCategory<AlphaT> *expr;
        unsigned *lower_bound;

        Unbound(const OpaqueCategory<AlphaT> *expr_, unsigned *lb) throw()
            : expr(const_cast<OpaqueCategory<AlphaT> *>(expr_))
            , lower_bound(lb)
        { }

    public:

        typedef Unbound<AlphaT, category_lb_tag> self_type;
        typedef unbound_category_lb_tag tag_type;

        // an extension rule, with an unbound category and upper bound
        FLTL_TDOP_RULE_PATTERN(unbound_category_lb_tag)
    };

    /// bound category, unbound lower bound
    template <typename AlphaT>
    class Bound<AlphaT, category_lb_tag> {
    private:

        friend class detail::PatternData<AlphaT>;
        friend class OpaqueCategory<AlphaT>;

        OpaqueCategory<AlphaT> *expr;
        unsigned *lower_bound;

        Bound(const OpaqueCategory<AlphaT> *expr_, unsigned *lb) throw()
            : expr(const_cast<OpaqueCategory<AlphaT> *>(expr_))
            , lower_bound(lb)
        { }

    public:

        typedef Bound<AlphaT, category_lb_tag> self_type;
        typedef category_lb_tag tag_type;

        // an extension rule, with an unbound upper bound, and a definite
        // category
        FLTL_TDOP_RULE_PATTERN(category_lb_tag)
    };
}}

#endif /* Grail_Plus_TDOP_UNBOUND_HPP_ */
