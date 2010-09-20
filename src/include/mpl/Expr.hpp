/*
 * Expr.hpp
 *
 *  Created on: Sep 19, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef CFTL_MPL_EXPR_HPP_
#define CFTL_MPL_EXPR_HPP_

#include "src/include/trait/Uncopyable.hpp"

namespace cftl { namespace mpl {

    template <typename T> class Expr;

    namespace {

        /// produces a value of a given type
        template <typename T>
        class Static {
        public:
            static const T VALUE;
        };
    }

    /// expression predicates
    namespace expr {

        template <typename L, typename R>
        class Equal : public Expr<Equal<L, R> >{ };

        template <typename L, typename R>
        class NotEqual : public Expr<NotEqual<L, R> >{ };

        template <typename L, typename R>
        class And : public Expr<And<L, R> >{ };

        template <typename L, typename R>
        class Or : public Expr<Or<L, R> >{ };

        template <typename R>
        class Not : public Expr<Not<R> >{ };

        template <typename T>
        class Variable : public Expr<Variable<T> > { };
    }

    /// Base expression template
    template <typename L>
    class Expr {
    public:

        template <typename R>
        expr::Equal<L, R> operator==(R) const {
            return Static<expr::Equal<L, R> >::VALUE;
        }

        template <typename R>
        expr::NotEqual<L, R> operator!=(R) const {
            return Static<expr::NotEqual<L, R> >::VALUE;
        }

        template <typename R>
        expr::And<L, R> operator&&(R) const {
            return Static<expr::And<L, R> >::VALUE;
        }

        template <typename R>
        expr::Or<L, R> operator||(R) const {
            return Static<expr::Or<L, R> >::VALUE;
        }

        expr::Not<L> operator!(void) const {
            return Static<expr::Not<L> >::VALUE;
        }
    };
}}

#endif /* CFTL_MPL_EXPR_HPP_ */
