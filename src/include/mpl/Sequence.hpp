/*
 * Sequence.hpp
 *
 *  Created on: Sep 15, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef CFTL_MPL_SEQUENCE_HPP_
#define CFTL_MPL_SEQUENCE_HPP_

#include "src/include/preprocessor/CATENATE.hpp"
#include "src/include/preprocessor/ENUMERATE_PARAMS.hpp"
#include "src/include/preprocessor/EVAL.hpp"
#include "src/include/preprocessor/INCREMENT.hpp"
#include "src/include/preprocessor/REPEAT_LEFT.hpp"
#include "src/include/preprocessor/UNPACK.hpp"

#include "src/include/mpl/Max.hpp"
#include "src/include/mpl/SizeOf.hpp"
#include "src/include/mpl/Unit.hpp"

/// default number of types that can be sequenced
#ifndef CFTL_SEQUENCE_OVER_TYPES_LIMIT
#define CFTL_SEQUENCE_OVER_TYPES_LIMIT 7
#endif

#define CFTL_SEQUENCE_COMPARE_TYPE(n, t) \
    , SizeOf<typename SequenceTypeEq<t, T ## n >::type_t >::VALUE

#define CFTL_SEQUENCE_SPEC_TYPENAME_LIST \
    typename T0 \
    CFTL_ENUMERATE_PARAMS(CFTL_SEQUENCE_OVER_TYPES_LIMIT,typename T)

#define CFTL_SEQUENCE_TYPE_PARAM_LIST \
    T0 CFTL_ENUMERATE_PARAMS(CFTL_SEQUENCE_OVER_TYPES_LIMIT, T)

#define CFTL_SEQUENCE_INDEX_SPEC(n, _) \
    template <typename InvalidType> \
    class IndexImpl<InvalidType, n> { \
    public: \
        typedef T ## n type_t; \
    };

#define CFTL_SEQUENCE_TYPE_LENGTH(n, _) \
    + SequenceTypeLength<T ## n>::VALUE

namespace cftl {

    namespace {

        /// type equivalence, if two types are different then this yields the
        /// unit type
        template <typename A, typename B>
        class SequenceTypeEq {
        public:
            typedef Unit type_t;
        };

        /// type equivalence, if two types are the same then this yields
        /// the type that was compared.
        template <typename A>
        class SequenceTypeEq<A, A> {
        public:
            typedef A type_t;
        };

        /// if we're requesting a const and the stored type might be const
        /// then we return the type as const.
        template <typename A>
        class SequenceTypeEq<const A, A> {
        public:
            typedef const A type_t;
        };

        /// const-correctness for type equivalence. if one of the types in
        /// sum is const, but we're requesting with a non-const, then we
        /// see that as a failure to match
        template <typename A>
        class SequenceTypeEq<A, const A> {
        public:
            typedef Unit type_t;
        };

        /// typed if
        template <const bool, typename IfTrue, typename IfFalse>
        class SequenceTypeIf;

        template <typename IfTrue, typename IfFalse>
        class SequenceTypeIf<true, IfTrue, IfFalse> {
        public:
            typedef IfTrue type_t;
        };

        template <typename IfTrue, typename IfFalse>
        class SequenceTypeIf<false, IfTrue, IfFalse> {
        public:
            typedef IfFalse type_t;
        };

        template <typename T>
        class SequenceTypeLength {
        public:
            enum {
                VALUE = (0 == SizeOf<T>::VALUE ? 0 : 1)
            };
        };
    }

    /// represents a sequence of types and operations on the sequence.
    ///
    /// Examples:
    ///     Sequence<int, char, float>::Select<char>::type_t <==> char
    ///     Sequence<int, char, float>::Select<bool>::type_t <==> Unit
    ///
    ///     Sequence<int, char, float>::Index<1>::type_t <==> char
    ///     Sequence<int, char, float>::Index<3>::type_t <==> Unit
    ///
    ///     Sequence<int, char, float>::Length::VALUE <==> 3
    ///
    template<typename T0 CFTL_ENUMERATE_VALUE_PARAMS(
        CFTL_SEQUENCE_OVER_TYPES_LIMIT,
        T,
        typename,
        = Unit
    )>
    class Sequence {
    public:

        /// the length of the type sequence. types can be ignored from the
        /// length count by specializing SizeOf to have VALUE = 0, i.e. this
        /// counts all types in the sequence that the programmer considers
        /// to be able to contain values.
        class Length {
        public:
            enum {
                VALUE = (
                    SequenceTypeLength<T0>::VALUE
                    CFTL_REPEAT_LEFT(
                        CFTL_SEQUENCE_OVER_TYPES_LIMIT,
                        CFTL_SEQUENCE_TYPE_LENGTH,
                        void
                    )
                )
            };
        };

        /// check to see if another type is contained within the type
        /// sequence. the yield of this is the type if it is contained
        /// or the unit type if it is not contained. The select operation
        /// takes constness into account.
        template <typename Q, typename InvalidType=Unit>
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

            typedef typename SequenceTypeIf<
                (MAX_TYPE_SIZE > 0),
                Q,
                InvalidType
            >::type_t type_t;

        };

    private:

        template <typename InvalidType, const unsigned i>
        class IndexImpl {
        public:
            typedef InvalidType type_t;
        };

        /// partial specializations of IndexImpl
        CFTL_SEQUENCE_INDEX_SPEC(0, void)
        CFTL_REPEAT_LEFT(
            CFTL_SEQUENCE_OVER_TYPES_LIMIT,
            CFTL_SEQUENCE_INDEX_SPEC,
            void
        )

    public:

        /// get the type at a particular index. if the index provided is
        /// out of range then the Unit type is returned.
        template <const unsigned i, typename InvalidType=Unit>
        class Index {
        public:
            typedef typename IndexImpl<InvalidType, i>::type_t type_t;
        };
    };
}


#endif /* CFTL_MPL_SEQUENCE_HPP_ */
