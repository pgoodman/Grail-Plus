/*
 * Query.hpp
 *
 *  Created on: Sep 19, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef CFTL_MPL_QUERY_HPP_
#define CFTL_MPL_QUERY_HPP_

#include "src/include/preprocessor/ENUMERATE_VALUE_PARAMS.hpp"
#include "src/include/preprocessor/REPEAT_LEFT.hpp"
#include "src/include/preprocessor/TEMPLATE_VARIABLE_LIMIT.hpp"

#include "src/include/mpl/Expr.hpp"
#include "src/include/mpl/Sequence.hpp"
#include "src/include/mpl/Unit.hpp"

#include "src/include/stl/Tuple.hpp"

#include "src/include/trait/StaticOnly.hpp"
#include "src/include/trait/Uncopyable.hpp"

#define CFTL_QUERY_TYPENAME_DEFAULT_LIST \
    typename T0 \
    CFTL_ENUMERATE_VALUE_PARAMS(\
        CFTL_TEMPLATE_VARIABLE_LIMIT,T,typename,= Unit \
    )

#define CFTL_QUERY_VAR_ARG(n, _) \
    , var_t v ## n=null_var

#define CFTL_QUERY_VAR_ARGS \
    var_t v0 \
    CFTL_REPEAT_LEFT(CFTL_TEMPLATE_VARIABLE_LIMIT, CFTL_QUERY_VAR_ARG, void)

#define CFTL_QUERY_MAKE_VAR(n, ret) \
    QueryVar<n> _ ## n;

#define CFTL_QUERY_USE_VAR(n, foo) (void) query::var::_ ## n;

//    ret _ ## n (QueryUnit) { }

namespace cftl { namespace mpl {

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
            CFTL_QUERY_MAKE_VAR(0, void)
            CFTL_REPEAT_LEFT(
                CFTL_TEMPLATE_VARIABLE_LIMIT,
                CFTL_QUERY_MAKE_VAR,
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
        template <CFTL_QUERY_TYPENAME_DEFAULT_LIST>
        class select : trait::StaticOnly {
        public:

            /// build up the predicates of this query. all we need is the
            /// type information stored inside of PredicateExprT.
            template <typename PredicateExprT>
            static void where(Expr<PredicateExprT>) throw() {
                CFTL_QUERY_USE_VAR(0, void)
                CFTL_REPEAT_LEFT(
                    CFTL_TEMPLATE_VARIABLE_LIMIT,
                    CFTL_QUERY_USE_VAR,
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

#endif /* CFTL_MPL_QUERY_HPP_ */
