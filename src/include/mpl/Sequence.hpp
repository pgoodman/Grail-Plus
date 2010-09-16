/*
 * Sequence.hpp
 *
 *  Created on: Sep 15, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef CFTL_MPL_SEQUENCE_HPP_
#define CFTL_MPL_SEQUENCE_HPP_

#include "src/include/preprocessor/ENUMERATE_VALUE_PARAMS.hpp"
#include "src/include/preprocessor/REPEAT_LEFT.hpp"

#include "src/include/mpl/Max.hpp"
#include "src/include/mpl/SizeOf.hpp"
#include "src/include/mpl/Unit.hpp"

/// default number of types that can be sequenced
#ifndef CFTL_SEQUENCE_OVER_TYPES_LIMIT
#define CFTL_SEQUENCE_OVER_TYPES_LIMIT 7
#endif

#define CFTL_SEQUENCE_COMPARE_TYPE(n, t) \
    , SizeOf<typename __Eq<t, T ## n >::type_t >::VALUE

namespace cftl {

    namespace {

        /// type equivalence, if two types are different then this yields the
        /// unit type
        template <typename A, typename B>
        class __Eq {
        public:
            typedef Unit type_t;
        };

        /// type equivalence, if two types are the same then this yields
        /// the type that was compared.
        template <typename A>
        class __Eq<A, A> {
        public:
            typedef A type_t;
        };

        /// if we're requesting a const and the stored type might be const
        /// then we return the type as const.
        template <typename A>
        class __Eq<const A, A> {
        public:
            typedef const A type_t;
        };

        /// const-correctness for type equivalence. if one of the types in
        /// sum is const, but we're requesting with a non-const, then we
        /// see that as a failure to match
        template <typename A>
        class __Eq<A, const A> {
        public:
            typedef Unit type_t;
        };

        /// typed if
        template <const bool, typename IfTrue, typename IfFalse>
        class __TypeIf;

        template <typename IfTrue, typename IfFalse>
        class __TypeIf<true, IfTrue, IfFalse> {
        public:
            typedef IfTrue type_t;
        };

        template <typename IfTrue, typename IfFalse>
        class __TypeIf<false, IfTrue, IfFalse> {
        public:
            typedef IfFalse type_t;
        };
    }

    /// represents a sequence of types and operations on the sequence.
    template<typename T0 CFTL_ENUMERATE_VALUE_PARAMS(
        CFTL_SEQUENCE_OVER_TYPES_LIMIT,
        T,
        typename,
        = Unit
    )>
    class Sequence {
    public:

        /// check to see if another type is contained within the type
        /// sequence. the yield of this is the type if it is contained
        /// or the unit type if it is not contained.
        template <typename Q>
        class Select {
        private:

            enum {
                MAX_TYPE_SIZE = Max<0
                    CFTL_SEQUENCE_COMPARE_TYPE(0, Q)
                    CFTL_REPEAT_LEFT(
                        CFTL_SEQUENCE_OVER_TYPES_LIMIT,
                        CFTL_SEQUENCE_COMPARE_TYPE,
                        Q
                    )
                >::VALUE
            };

        public:

            typedef typename __TypeIf<
                (MAX_TYPE_SIZE > 0),
                Q,
                Unit
            >::type_t type_t;

        };
    };
}


#endif /* CFTL_MPL_SEQUENCE_HPP_ */
