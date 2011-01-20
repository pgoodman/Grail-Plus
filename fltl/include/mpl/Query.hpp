/*
 * Query.hpp
 *
 *  Created on: Sep 19, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef FLTL_MPL_QUERY_HPP_
#define FLTL_MPL_QUERY_HPP_

#include <cstdlib>

#include "fltl/include/mpl/Expr.hpp"
#include "fltl/include/mpl/Static.hpp"

#include "fltl/include/trait/Uncopyable.hpp"

namespace fltl { namespace mpl {

    /// forward-declaration
    template <typename DataStructureT>
    class Query;

    template <typename DataStructureT, typename BuilderT>
    class QueryBuilder;

    namespace detail {

        template <typename DataStructureT>
        class QueryHandler {
        private:

            typedef typename DataStructureT::query_builder_tag builder_tag;

            DataStructureT *data_structure;

        public:

            QueryHandler(DataStructureT *ds) throw()
                : data_structure(ds)
            { }

            template <typename ExprT>
            void operator[](const ExprT) const throw() {

            }
        };
    }

    /// make a new data structure query.
    template <typename DataStructureT>
    detail::QueryHandler<DataStructureT>
    query(DataStructureT &ds) throw() {
        return detail::QueryHandler<DataStructureT>(&ds);
    }

}}

#if 0

#include "fltl/include/preprocessor/CATENATE.hpp"
#include "fltl/include/preprocessor/ENUMERATE_VALUE_PARAMS.hpp"
#include "fltl/include/preprocessor/REPEAT_LEFT.hpp"
#include "fltl/include/preprocessor/STATIC_ASSERT.hpp"
#include "fltl/include/preprocessor/TEMPLATE_VARIABLE_LIMIT.hpp"

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

namespace {

    class QueryUnit { };

    /// template class for extracting a proper expression type
    template <typename ExprT>
    class QueryExtractExpr : public ExprT {
    public:
        typedef ExprT type;
    };

    template <>
    class QueryExtractExpr<fltl::mpl::Unit> : public fltl::mpl::expr::Expr<
        fltl::mpl::Unit,
        fltl::mpl::Unit,
        fltl::mpl::Unit
    > { };
}

namespace fltl { namespace mpl {

    /// variables available for use in queries. Variables are tagged
    /// expressions where the id of the variable is contained in its type.
    namespace query {

        enum {
            NUM_VARIABLES = 32
        };

        namespace {

            /// private place to store the names of variables. this exists
            /// so that we can name variables; makes debugging queries
            /// simpler.
            const char *VARIABLE_NAME[NUM_VARIABLES] = {""};
        }

        namespace var {

            /// expression tag for variables.
            class Var { };

            /// variable binding for use in a query. the integer template
            /// parameter specifies the id of the variable. all variables
            /// for a particular query should have different ids, else they
            /// will conflict
            template <typename T, const int n>
            class Let : public mpl::expr::Expr<
                Var,
                T,
                mpl::expr::Integer<n>
            > {
            public:
                explicit Let(const char *name) throw() {
                    VARIABLE_NAME[n] = name;
                }
            };

            /// binding environment type for query variables. This contains
            /// the set of bound variables.
            template <const int bound_vars=0>
            class BindingEnvironment {
            public:

                enum {
                    BOUND_VARS = bound_vars
                };

                /// bind a variable, resulting in a new binding environment
                template <const int var_id>
                class Bind {
                public:
                    typedef BindingEnvironment<
                        bound_vars | (1 << var_id)
                    > type;
                };

                /// intersection of two binding environments
                template <typename OtherEnvT>
                class Intersect {
                public:
                    typedef BindingEnvironment<
                        bound_vars & OtherEnvT::BOUND_VARS
                    > type;
                };

                /// union of two binding environments
                template <typename OtherEnvT>
                class Union {
                public:
                    typedef BindingEnvironment<
                        bound_vars | OtherEnvT::BOUND_VARS
                    > type;
                };

                /// check if a variable is bound
                template <const int var_id>
                class Contains {
                public:
                    enum {
                        RESULT = (0 == ((1 << var_id) & bound_vars)) ? 0 : 1
                    };
                };

                /// stream the variables
                inline static void stream(std::ostream &os) throw() {
                    const char *sep(", $");
                    for(int i = 0, j = 2; i < NUM_VARIABLES; ++i) {
                        if(0 != ((1 << i) & bound_vars)) {
                            os << &(sep[j]) << VARIABLE_NAME[i];
                            j = 0;
                        }
                    }
                }
            };
        }

        /// an index data structure for query results
        template <typename DiscriminatorT, typename DataStructureT>
        class Index;

        /// operation for meeting results. the meet operation is expected to
        /// have a public static void inline method "visit" to perform the
        /// meet operation on two indexes, returning a 3rd through "visit"'s
        /// third argument.
        template <
            typename DiscriminatorT,
            typename DataStructureT,
            typename ExprTagT
        >
        class Meet;

        /// operation for unpacking a query into its component parts. the
        /// filter operation is expected to have one inline static method
        /// called "visit" that accepts a const index reference for the
        /// index to search and a reference to an index to fill with results.
        template <
            typename DiscriminatorT,
            typename DataStructureT,
            typename ExprT,
            typename ExprTagT
        >
        class Filter {
        public:
            inline static void visit(
                const Index<DiscriminatorT, DataStructureT> &,
                Index<DiscriminatorT, DataStructureT> &
            ) throw() {
                std::cout << "[ERROR] No filter exists for sub-expression:\n\t";
                expr::Printer<ExprT>::print();
                std::cout << "\n\n";
            }
        };

        /// filter operation for a Unit expression
        template <typename DiscriminatorT, typename DataStructureT>
        class Filter<DiscriminatorT, DataStructureT, Unit, Unit> {
        public:
            inline static void visit(
                const Index<DiscriminatorT, DataStructureT> &,
                Index<DiscriminatorT, DataStructureT> &
            ) throw() { }
        };

        /// filter operation for an expression of unknown tag, unpack the
        /// tag. This simplifies the usage of filter somewhat.
        template <
            typename DiscriminatorT,
            typename DataStructureT,
            typename ExprT
        >
        class Filter<DiscriminatorT, DataStructureT, ExprT, Unit> {
        public:
            inline static void visit(
                const Index<DiscriminatorT, DataStructureT> &index_in,
                Index<DiscriminatorT, DataStructureT> &index_out
            ) throw() {
                Filter<
                    DiscriminatorT,
                    DataStructureT,
                    ExprT,
                    typename ExprT::tag_t
                >::visit(index_in, index_out);
            }
        };

        namespace {

            /// generic filter and meet operation for a binary operator
            /// that performs two separate filters and then meets their
            /// results.
            template <
                typename DiscriminatorT,
                typename DataStructureT,
                typename ExprT,
                typename ExprTagT
            >
            class FilterMeet {
            public:
                inline static void visit(
                    const Index<DiscriminatorT, DataStructureT> &index_in,
                    Index<DiscriminatorT, DataStructureT> &index_out
                ) throw() {
                    Index<DiscriminatorT, DataStructureT> index_out_0;
                    Index<DiscriminatorT, DataStructureT> index_out_1;

                    Filter<
                        DiscriminatorT,
                        DataStructureT,
                        typename ExprT::first_t,
                        Unit
                    >::visit(index_in, index_out_0);

                    Filter<
                        DiscriminatorT,
                        DataStructureT,
                        typename ExprT::second_t,
                        Unit
                    >::visit(index_in, index_out_1);

                    Meet<DiscriminatorT, DataStructureT, ExprTagT>::visit(
                        index_out_0,
                        index_out_1,
                        index_out
                    );
                }
            };
        }

        /// unpack for disjunction. this runs the two conditions and then
        /// meets their results.
        template <typename D0, typename D1, typename E>
        class Filter<D0, D1, E, mpl::expr::LogicalOr> : public FilterMeet<
            D0, D1, E, mpl::expr::LogicalOr
        > { };

        /// unpack for sequencing. this runs the first condition, then the
        /// second one given the results of the first one.
        template <
            typename DiscriminatorT,
            typename DataStructureT,
            typename ExprT
        >
        class Filter<
            DiscriminatorT,
            DataStructureT,
            ExprT,
            mpl::expr::LogicalAnd
        > {
        public:
            inline static void visit(
                const Index<DiscriminatorT, DataStructureT> &index_in,
                Index<DiscriminatorT, DataStructureT> &index_out
            ) throw() {
                Index<DiscriminatorT, DataStructureT> index_out_0;

                Filter<
                    DiscriminatorT,
                    DataStructureT,
                    typename ExprT::first_t,
                    Unit
                >::visit(index_in, index_out_0);

                Filter<
                    DiscriminatorT,
                    DataStructureT,
                    typename ExprT::second_t,
                    Unit
                >::visit(index_out_0, index_out);
            }
        };

        /// syntax checker.
        template <
            typename DiscriminatorT,
            typename DataStructureT,
            typename InEnvT,
            const int state,
            typename ExprT,
            typename ExprTagT=Unit
        >
        class SyntaxChecker;

        /// default case for syntax checker, the expression is
        /// invalid
        template <
            typename D0, typename D1, typename B,
            const int s, typename E, typename ET
        >
        class SyntaxChecker {
        public:

            typedef B out_env_t;

            inline static bool visit(void) throw() {
                std::cout << "[ERROR] Unsupported query operator "
                          << ET::PRINT_NAME << " in state " << s
                          << " with sub-expression:\n\t";
                expr::Printer<E>::print();
                std::cout << "\n\n";
                return false;
            }
        };

        /// base case for syntax checker.
        template <typename D0, typename D1, typename IE, const int s>
        class SyntaxChecker<D0, D1, IE, s, Unit, Unit> {
        public:
            typedef IE out_env_t;

            inline static bool visit(void) throw() {
                return true;
            }
        };

        /// expansion case for syntax checker, dispatch on the tag
        template <
            typename D0, typename D1, typename IE,
            const int s, typename E
        >
        class SyntaxChecker<D0, D1, IE, s, E, Unit> {
        public:

            typedef typename SyntaxChecker<
                D0, D1, IE, s, E, typename E::tag_t
            >::out_env_t out_env_t;

            inline static bool visit(void) throw() {
                return SyntaxChecker<
                    D0, D1, IE, s, E, typename E::tag_t
                >::visit();
            }
        };
    }

    /// base query type for an arbitrary data stucture. it is the programmers
    /// responsibility to expose certain information to the Query type in
    /// the form of type traits for queries.
    template <

        // data structure being selected from
        typename DataStructureT
    >
    class Query : private trait::StaticOnly {
    public:

        /// build up the predicates of this query. all we need is the
        /// type information stored inside of PredicateExprT.
        template <typename ExprT>
        static void where(ExprT) throw() {

            typedef DataStructureT ds_t;
            typedef typename ds_t::query_discriminator_t ds_id_t;

            // check syntax
            const bool has_error = !mpl::query::SyntaxChecker<
                ds_id_t,
                ds_t,
                mpl::query::var::BindingEnvironment<>,
                0,
                ExprT,
                Unit
            >::visit();

            // print out the expression tree if a syntax error was detected
            if(has_error) {
                std::cout << "[NOTE] Expression tree for query is:\n\t";
                expr::Printer<ExprT>::print();
                std::cout << "\n\n";
            } else {

                // go figure out what variables are used


                // filter results
                mpl::query::Index<ds_id_t, ds_t> index_in;
                mpl::query::Index<ds_id_t, ds_t> results;
                mpl::query::Filter<ds_id_t, ds_t, ExprT, Unit>::visit(
                    index_in,
                    results
                );
            }
        }
    };

    namespace expr {

        /// for printing query variables
        template <typename L, typename IntT>
        class Printer<Expr<mpl::query::var::Var, L, IntT> > {
        public:
            inline static void print(void) {
                std::cout << '$' << mpl::query::VARIABLE_NAME[IntT::VALUE];
            }
        };
    }

    namespace query {

        /// nice way to access the
        template <typename DataStructureT>
        class from : public Query<DataStructureT> { };
    }
}}

#if 0

/// unpack for conjunction. this runs the two conditions and then
/// meets their results.
/*template <typename D0, typename D1, typename E>
class Filter<D0, D1, E, mpl::expr::LogicalAnd> : public FilterMeet<
    D0, D1, E, mpl::expr::LogicalAnd
> { };*/

#if 0
        namespace {
            template <typename D0, typename D1, const int s, typename E, typename ET>
            class BSC {
            public:
                inline static bool visit(void) throw() {
                    return (
                        SyntaxChecker<D0, D1, s, typename E::first_t>::visit()
                     && SyntaxChecker<D0, D1, s, typename E::second_t>::visit()
                    );
                }
            };
        }

        /// initial case for default query operators
        template <typename D0, typename D1, typename E>
        class SyntaxChecker<D0, D1, 0, E, mpl::expr::LogicalAnd>
         : public BSC<D0,D1,0,E,mpl::expr::LogicalAnd>{ };

        template <typename D0, typename D1, typename E>
        class SyntaxChecker<D0, D1, 0, E, mpl::expr::LogicalOr>
         : public BSC<D0,D1,0,E,mpl::expr::LogicalOr>{ };

        template <typename D0, typename D1, typename E>
        class SyntaxChecker<D0, D1, 0, E, mpl::expr::ShiftRight>
         : public BSC<D0,D1,0,E,mpl::expr::ShiftRight>{ };
#endif

#if 0
#define FLTL_QUERY_MAKE_VAR(n, ret) \
    class __var_ ## n : public __var_t { \
    public: \
        enum { \
            ID = (1 << n), \
            NAME = n \
        }; \
        static const char *PRINT_NAME; \
    }; \
    mpl::expr::Expr<Variable, __var_ ## n> _ ## n; \
    const char *__var_ ## n::PRINT_NAME("_" #n);


#define FLTL_QUERY_USE_VAR(n, foo) (void) query::var::_ ## n;
#endif


/*
#define FLTL_QUERY_SELECT_NTH_PARAM(n,prefix) \
    prefix(n) const query::var::Let<DataStructureT,N ## n>

#define FLTL_QUERY_SELECT_NTH_TYPE(n,prefix) \
    prefix(n) const int N ## n

        template <
            FLTL_QUERY_SELECT_NTH_TYPE(0, FLTL_NOTHING)
            FLTL_REPEAT_LEFT(
                FLTL_TEMPLATE_VARIABLE_LIMIT,
                FLTL_QUERY_SELECT_NTH_TYPE,
                FLTL_COMMA
            )
        >
        inline static void select(
            FLTL_QUERY_SELECT_NTH_PARAM(0, FLTL_NOTHING)
            FLTL_REPEAT_LEFT(
                FLTL_TEMPLATE_VARIABLE_LIMIT,
                FLTL_QUERY_SELECT_NTH_PARAM,
                FLTL_COMMA
            )
        ) throw() {

        }
*/


/*
 typedef typename DataStructureT::query_discriminator_t
         query_discriminator_t;

 typedef query::Builder<query_discriminator_t>
         query_trait_t;

 typedef typename query_trait_t::template BindingEnvironment<>
         empty_binding_env_t;
 */
 // check to make sure that the input expression validates
 // against our query expression grammar
 // TODO

 // check to make sure no variables are used before they
 // are bound. if they are then we end up with a failed
 // compile-time assertion.
 /*FLTL_STATIC_ASSERT(
     (0 == query_trait_t::template CheckSubExpr<
         ExprT,
         empty_binding_env_t
     >::NUM_ERRORS)
 );*/

        /// the idea of the query builder is that it provides all the machinery
        /// for the query classes to build the query without having to be
        /// "owned" by a specific data structure. i.e. a class has this trait
        /// if there is a specialization of this trait for that class.
        /// Specializations of this trait can optionally take advantage of all
        /// the useful templates of this trait by inheriting from this trait
        /// class and omitting the data structure to be specialized
        template <typename DiscriminatorT>
        class Builder {
        public:



            template <typename T>
            class AsVariable { };

            /// forward-declaration
            template <typename ExprT, typename BoundVarsEnvT>
            class CheckSubExpr;

            /// visit an expression and check for errors
            template <
                typename TagT, typename L, typename R, typename BoundVarsEnvT
            >
            class CheckExpr {
            private:

                typedef CheckSubExpr<L, BoundVarsEnvT>
                        checked_left_t;

                typedef CheckSubExpr<R, typename checked_left_t::bound_vars_t>
                        checked_right_t;

            public:

                typedef typename checked_right_t::bound_vars_t
                        bound_vars_t;

                /// use this counter to track:
                ///     - uses of variables before they have been bound
                ///     - unsupported expression types
                ///
                /// by default, we see all typed operations (except bound
                /// variables, as specialized in fltl/include/mpl/Query.hpp),
                /// as being erroneous.
                enum {
                    NUM_ERRORS = checked_left_t::NUM_ERRORS
                               + checked_right_t::NUM_ERRORS
                };
            };

            /// partial specialization for unit type.
            template <
                typename ExprLT,
                typename ExprRT,
                typename BoundVarsEnvT
            >
            class CheckExpr<mpl::Unit,ExprLT,ExprRT,BoundVarsEnvT> {
            public:
                typedef BoundVarsEnvT bound_vars_t;
                enum {
                    NUM_ERRORS = 0
                };
            };

            /// simpler way to access the above for unknown, possibly unit,
            /// expression types
            template <typename ExprT, typename BoundVarsEnvT>
            class CheckSubExpr : public CheckExpr<
                typename ExprT::tag_t,
                typename ExprT::first_t,
                typename ExprT::second_t,
                BoundVarsEnvT
            > { };

            template <typename BoundVarsEnvT>
            class CheckSubExpr<mpl::Unit, BoundVarsEnvT> : public CheckExpr<
                mpl::Unit,
                mpl::Unit,
                mpl::Unit,
                BoundVarsEnvT
            > { };
        };

        /// allow for extraction of a variable where type L is a variable.
        template <typename DiscriminatorT>
        template <typename L, typename R>
        class Builder<DiscriminatorT>::AsVariable<
            mpl::expr::Expr<mpl::query::Variable, L, R
        > > : public L {};

        /// check to make sure a variable is bound
        template <typename DiscriminatorT>
        template <typename VarT, typename R, typename BoundVarsEnvT>
        class Builder<DiscriminatorT>::CheckExpr<
            mpl::query::Variable, VarT, R, BoundVarsEnvT
        > {
        public:
            typedef BoundVarsEnvT bound_vars_t;
            enum {
                NUM_ERRORS = (
                    1 - BoundVarsEnvT::template Contains<VarT>::VALUE
                )
            };
        };
#endif
#endif

#endif /* FLTL_MPL_QUERY_HPP_ */
