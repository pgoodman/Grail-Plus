/*
 * Query.hpp
 *
 *  Created on: Sep 19, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef FLTL_MPL_QUERY_HPP_
#define FLTL_MPL_QUERY_HPP_

#include "fltl/include/preprocessor/ENUMERATE_VALUE_PARAMS.hpp"
#include "fltl/include/preprocessor/REPEAT_LEFT.hpp"
#include "fltl/include/preprocessor/TEMPLATE_VARIABLE_LIMIT.hpp"

#include "fltl/include/mpl/Expr.hpp"
#include "fltl/include/mpl/Sequence.hpp"
#include "fltl/include/mpl/Unit.hpp"

#include "fltl/include/stl/Tuple.hpp"

#include "fltl/include/trait/StaticOnly.hpp"
#include "fltl/include/trait/Uncopyable.hpp"

#define FLTL_QUERY_TYPENAME_DEFAULT_LIST \
    typename T0 \
    FLTL_ENUMERATE_VALUE_PARAMS(\
        FLTL_TEMPLATE_VARIABLE_LIMIT,T,typename,= Unit \
    )

#define FLTL_QUERY_VAR_ARG(n, _) \
    , var_t v ## n=null_var

#define FLTL_QUERY_VAR_ARGS \
    var_t v0 \
    FLTL_REPEAT_LEFT(FLTL_TEMPLATE_VARIABLE_LIMIT, FLTL_QUERY_VAR_ARG, void)

#define FLTL_QUERY_MAKE_VAR(n, ret) \
    QueryVar<n> _ ## n;

#define FLTL_QUERY_USE_VAR(n, foo) (void) query::var::_ ## n;

//    ret _ ## n (QueryUnit) { }

namespace fltl { namespace mpl {

    namespace {
        class QueryUnit { };

        template <const unsigned x>
        class QueryVar : public Expr<QueryVar<x> > { };

        /// the type of a query variable
        //typedef void (*useless_func_t)(QueryUnit);

        //typedef expr::Variable<unsigned> var_t;

        /// default null variable
        //void null_var(QueryUnit) { }
    }

    /// variables available for use in queries. Note: the types of the
    /// variables are opaque to the outside world.
    namespace query {
        namespace var {
            FLTL_QUERY_MAKE_VAR(0, void)
            FLTL_REPEAT_LEFT(
                FLTL_TEMPLATE_VARIABLE_LIMIT,
                FLTL_QUERY_MAKE_VAR,
                void
            )
        }
    }

    /// base query type for an arbitrary data stucture. it is the programmers
    /// responsibility to expose certain information to the Query type in
    /// the form of type traits for queries.
    template <

        // data structure being selected from
        typename DataStructureT,

        // sequence of types of return values, default is the empty sequence.
        typename ReturnSeqT=Sequence<Unit>
    >
    class Query : private trait::StaticOnly {
    public:

        /// determine which variables to select
        template <FLTL_QUERY_TYPENAME_DEFAULT_LIST>
        class with : trait::StaticOnly {
        public:

            /// build up the predicates of this query. all we need is the
            /// type information stored inside of PredicateExprT.
            template <typename PredicateExprT>
            static void where(Expr<PredicateExprT>) throw() {

                // mark the variables as being "used" so that we don't get
                // compiler warnings about them being unused.
                FLTL_QUERY_USE_VAR(0, void)
                FLTL_REPEAT_LEFT(
                    FLTL_TEMPLATE_VARIABLE_LIMIT,
                    FLTL_QUERY_USE_VAR,
                    void
                )
            }
        };

    };

    namespace query {

        /// nice way to access the
        template <typename DataStructureT>
        class from : public Query<DataStructureT> { };
    }
}}

#endif /* FLTL_MPL_QUERY_HPP_ */
