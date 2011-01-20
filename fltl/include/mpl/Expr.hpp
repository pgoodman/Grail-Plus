/*
 * Expr.hpp
 *
 *  Created on: Sep 19, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef FLTL_MPL_EXPR_HPP_
#define FLTL_MPL_EXPR_HPP_

#include "fltl/include/mpl/Static.hpp"
#include "fltl/include/mpl/Unit.hpp"

#define FLTL_EXPR_MAKE_UNARY_OPERATOR(func,cls) \
    inline Expr<expr::op::cls, expr::Unary<self_type> > \
    func(void) const throw() { \
        return mpl::Static< \
            Expr<expr::op::cls, expr::Unary<self_type> > \
        >::VALUE; \
    }

#define FLTL_EXPR_MAKE_BINARY_OPERATOR(func,cls) \
    template <typename T> \
    inline Expr<expr::op::cls, expr::Binary<self_type, T> > \
    func(const T &) const throw() { \
        return mpl::Static< \
            Expr<expr::op::cls, expr::Binary<self_type, T> > \
        >::VALUE; \
    }

namespace fltl { namespace mpl {

    namespace expr {

        /// meta-information for a unary operator
        template <typename T0>
        class Unary {
        public:

            template <
                typename TagT,
                template<class, class> class VisitorT,
                template<class, class, class> class AccumulatorT
            >
            class Visit {
            public:
                typedef typename VisitorT<TagT,T0>::type
                        visited_type;

                typedef typename AccumulatorT<TagT,visited_type,mpl::Unit>::type
                        type;
            };
        };

        /// meta-information for a binary operator
        template <typename T0, typename T1>
        class Binary {
        public:

            template <
                typename TagT,
                template<class, class> class VisitorT,
                template<class, class, class> class AccumulatorT
            >
            class Visit {
            public:
                typedef typename VisitorT<TagT,T0>::type
                        visited_type0;

                typedef typename VisitorT<TagT,T1>::type
                        visited_type1;

                typedef typename AccumulatorT<
                    TagT,
                    visited_type0,
                    visited_type1
                >::type type;
            };
        };

        /// expression operators
        namespace op {
            class assign { };

            class add { };
            class sub { };
            class pos { };
            class neg { };
            class mul { };
            class div { };
            class mod { };

            class pre_incr { };
            class post_incr { };
            class pre_decr { };
            class post_decr { };

            class eq { };
            class ne { };
            class gt { };
            class lt { };
            class ge { };
            class le { };

            class bool_not { };
            class bool_and { };
            class bool_or { };

            class bit_not { };
            class bit_and { };
            class bit_or { };
            class bit_xor { };
            class bit_shl { };
            class bit_shr { };

            class add_assign { };
            class sub_assign { };
            class mul_assign { };
            class div_assign { };
            class mod_assign { };
            class bit_and_assign { };
            class bit_or_assign { };
            class bit_xor_assign { };
            class bit_shl_assign { };
            class bit_shr_assign { };

            class subscript { };
            class deref { };
            class ref { };
            class follow { };
            class follow_deref { };

        }
    }

    /// expression type. expressions are "tagged" for easy dispatching.
    /// expression types store meta information as a type containing their
    /// operands.
    template <typename TagT, typename MetaT>
    class Expr {
    public:

        typedef TagT tag_type;
        typedef MetaT meta_type;
        typedef Expr<tag_type, meta_type> self_type;

        /// visit the nodes of an expression
        template <
            template<class, class> class VisitorT,
            template<class, class, class> class AccumulatorT
        >
        class Visit {
        public:
            typedef typename MetaT::template Visit<
                tag_type,
                VisitorT,
                AccumulatorT
            >::type type;
        };

        FLTL_EXPR_MAKE_BINARY_OPERATOR(operator=,assign)
        FLTL_EXPR_MAKE_BINARY_OPERATOR(operator+,add)
        FLTL_EXPR_MAKE_BINARY_OPERATOR(operator-,sub)
        FLTL_EXPR_MAKE_BINARY_OPERATOR(operator*,mul)
        FLTL_EXPR_MAKE_BINARY_OPERATOR(operator/,div)
        FLTL_EXPR_MAKE_BINARY_OPERATOR(operator%,mod)
        FLTL_EXPR_MAKE_UNARY_OPERATOR(operator+,pos)
        FLTL_EXPR_MAKE_UNARY_OPERATOR(operator-,neg)
        FLTL_EXPR_MAKE_UNARY_OPERATOR(operator++,pre_incr)

        /// post-increment operator
        inline Expr<expr::op::post_incr, expr::Unary<self_type> >
        operator++(int) const throw() {
            return mpl::Static<
                Expr<expr::op::post_incr, expr::Unary<self_type> >
            >::VALUE;
        }

        FLTL_EXPR_MAKE_UNARY_OPERATOR(operator--,pre_decr)

        /// post-decrement operator
        inline Expr<expr::op::post_decr, expr::Unary<self_type> >
        operator--(int) const throw() {
            return mpl::Static<
                Expr<expr::op::post_decr, expr::Unary<self_type> >
            >::VALUE;
        }

        FLTL_EXPR_MAKE_BINARY_OPERATOR(operator==,eq)
        FLTL_EXPR_MAKE_BINARY_OPERATOR(operator!=,ne)
        FLTL_EXPR_MAKE_BINARY_OPERATOR(operator>,gt)
        FLTL_EXPR_MAKE_BINARY_OPERATOR(operator<,lt)
        FLTL_EXPR_MAKE_BINARY_OPERATOR(operator>=,ge)
        FLTL_EXPR_MAKE_BINARY_OPERATOR(operator<=,le)
        FLTL_EXPR_MAKE_UNARY_OPERATOR(operator!,bool_not)
        FLTL_EXPR_MAKE_BINARY_OPERATOR(operator&&,bool_and)
        FLTL_EXPR_MAKE_BINARY_OPERATOR(operator||,bool_or)
        FLTL_EXPR_MAKE_UNARY_OPERATOR(operator~,bit_not)
        FLTL_EXPR_MAKE_BINARY_OPERATOR(operator&,bit_and)
        FLTL_EXPR_MAKE_BINARY_OPERATOR(operator|,bit_or)
        FLTL_EXPR_MAKE_BINARY_OPERATOR(operator^,bit_xor)
        FLTL_EXPR_MAKE_BINARY_OPERATOR(operator<<,bit_shl)
        FLTL_EXPR_MAKE_BINARY_OPERATOR(operator>>,bit_shr)
        FLTL_EXPR_MAKE_BINARY_OPERATOR(operator+=,add_assign)
        FLTL_EXPR_MAKE_BINARY_OPERATOR(operator-=,sub_assign)
        FLTL_EXPR_MAKE_BINARY_OPERATOR(operator*=,mul_assign)
        FLTL_EXPR_MAKE_BINARY_OPERATOR(operator/=,div_assign)
        FLTL_EXPR_MAKE_BINARY_OPERATOR(operator%=,mod_assign)
        FLTL_EXPR_MAKE_BINARY_OPERATOR(operator&=,bit_and_assign)
        FLTL_EXPR_MAKE_BINARY_OPERATOR(operator|=,bit_or_assign)
        FLTL_EXPR_MAKE_BINARY_OPERATOR(operator^=,bit_xor_assign)
        FLTL_EXPR_MAKE_BINARY_OPERATOR(operator<<=,bit_shl_assign)
        FLTL_EXPR_MAKE_BINARY_OPERATOR(operator>>=,bit_shr_assign)
        FLTL_EXPR_MAKE_BINARY_OPERATOR(operator[],subscript)
        FLTL_EXPR_MAKE_UNARY_OPERATOR(operator*,deref)
        FLTL_EXPR_MAKE_UNARY_OPERATOR(operator&,ref)
        FLTL_EXPR_MAKE_UNARY_OPERATOR(operator->,follow)
        FLTL_EXPR_MAKE_BINARY_OPERATOR(operator->*,follow_deref)
    };
}}

#endif /* FLTL_MPL_EXPR_HPP_ */
