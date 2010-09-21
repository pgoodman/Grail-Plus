/*
 * Expr.hpp
 *
 *  Created on: Sep 19, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef FLTL_MPL_EXPR_HPP_
#define FLTL_MPL_EXPR_HPP_

#include "fltl/include/trait/Uncopyable.hpp"

#define FLTL_BUILD_EXPR_TYPE_TPL_1(name) \
    template <typename T> \
    class name : public Expr<name<T> > { };

#define FLTL_BUILD_EXPR_TYPE_TPL_2(name) \
    template <typename L, typename R> \
    class name : public Expr<name<L, R> > { };

#define FLTL_BUILD_EXPR_TYPE_FUNC_1(c, f) \
    expr::c<L> f(void) const { \
        return Static<expr::c<L> >::VALUE; \
    }

#define FLTL_BUILD_EXPR_TYPE_FUNC_2(c, f) \
    template <typename R> \
    expr::c<L, R> f(R) const { \
        return Static<expr::c<L, R> >::VALUE; \
    }

namespace fltl { namespace mpl {

    template <typename T> class Expr;

    namespace {

        /// produces a value of a given type
        template <typename T>
        class Static {
        public:
            static const T VALUE;
        };
    }

    /// expression template classes for common operators
    namespace expr {

        FLTL_BUILD_EXPR_TYPE_TPL_2(Equal)
        FLTL_BUILD_EXPR_TYPE_TPL_2(NotEqual)
        FLTL_BUILD_EXPR_TYPE_TPL_2(LogicalAnd)
        FLTL_BUILD_EXPR_TYPE_TPL_2(LogicalOr)
        FLTL_BUILD_EXPR_TYPE_TPL_2(BitwiseAnd)
        FLTL_BUILD_EXPR_TYPE_TPL_2(BitwiseOr)

        FLTL_BUILD_EXPR_TYPE_TPL_1(LogicalNot)
        FLTL_BUILD_EXPR_TYPE_TPL_1(BitwiseNot)

        FLTL_BUILD_EXPR_TYPE_TPL_1(Ref)
        FLTL_BUILD_EXPR_TYPE_TPL_2(DerefByPtr)
    }

    /// Base expression template
    template <typename L>
    class Expr {
    public:

        FLTL_BUILD_EXPR_TYPE_FUNC_2(Equal, operator==)
        FLTL_BUILD_EXPR_TYPE_FUNC_2(NotEqual, operator!=)

        FLTL_BUILD_EXPR_TYPE_FUNC_2(LogicalAnd, operator&&)
        FLTL_BUILD_EXPR_TYPE_FUNC_2(LogicalOr, operator||)

        FLTL_BUILD_EXPR_TYPE_FUNC_2(BitwiseAnd, operator&)
        FLTL_BUILD_EXPR_TYPE_FUNC_2(BitwiseOr, operator|)

        FLTL_BUILD_EXPR_TYPE_FUNC_1(LogicalNot, operator!)
        FLTL_BUILD_EXPR_TYPE_FUNC_1(BitwiseNot, operator~)

        FLTL_BUILD_EXPR_TYPE_FUNC_1(Ref, operator&)
        FLTL_BUILD_EXPR_TYPE_FUNC_2(DerefByPtr, operator->*)
    };
}}

#endif /* FLTL_MPL_EXPR_HPP_ */
