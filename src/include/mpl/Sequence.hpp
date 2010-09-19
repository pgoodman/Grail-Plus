/*
 * Sequence.hpp
 *
 *  Created on: Sep 15, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef CFTL_MPL_SEQUENCE_HPP_
#define CFTL_MPL_SEQUENCE_HPP_

#include "src/include/preprocessor/TEMPLATE_VARIABLE_LIMIT.hpp"
#include "src/include/preprocessor/CATENATE.hpp"
#include "src/include/preprocessor/DECREMENT.hpp"
#include "src/include/preprocessor/ENUMERATE_PARAMS.hpp"
#include "src/include/preprocessor/ENUMERATE_VALUE_PARAMS.hpp"
#include "src/include/preprocessor/EVAL.hpp"
#include "src/include/preprocessor/INCREMENT.hpp"
#include "src/include/preprocessor/REPEAT_LEFT.hpp"
#include "src/include/preprocessor/UNPACK.hpp"

#include "src/include/trait/StaticOnly.hpp"

#include "src/include/mpl/Max.hpp"
#include "src/include/mpl/SizeOf.hpp"
#include "src/include/mpl/Unit.hpp"

#define CFTL_SEQUENCE_COMPARE_TYPE(n, t) \
    , SizeOf<typename SequenceTypeEq<t, T ## n >::type_t >::VALUE

#define CFTL_SEQUENCE_SPEC_TYPENAME_LIST \
    typename T0 \
    CFTL_ENUMERATE_PARAMS(CFTL_TEMPLATE_VARIABLE_LIMIT,typename T)

#define CFTL_SEQUENCE_TYPE_PARAM_LIST \
    T0 CFTL_ENUMERATE_PARAMS(CFTL_TEMPLATE_VARIABLE_LIMIT, T)

#define CFTL_SEQUENCE_AT_SPEC(n, _) \
    template <typename InvalidType> \
    class AtImpl<n, InvalidType> { \
    public: \
        typedef T ## n type_t; \
    };

#define CFTL_SEQUENCE_TYPE_LENGTH(n, _) \
    + SequenceTypeLength<T ## n>::VALUE

#define CFTL_SEQUENCE_INSERT_TYPE(n, _) \
    typedef typename CFTL_CATENATE(base_t, CFTL_DECREMENT(n))::\
    template Insert<T ## n>::type_t base_t ## n;

#define CFTL_SEQUENCE_TYPE_INDEX1(n, next_n) \
    VALUE ## next_n = mpl::SizeOf< \
        typename SequenceTypeEq<T ## n, T>::type_t \
    >::VALUE == 0 ? VALUE ## n : \
    (VALUE ## n > 0 ? VALUE ## n : next_n),

#define CFTL_SEQUENCE_TYPE_INDEX0(n, next_n) \
    CFTL_SEQUENCE_TYPE_INDEX1(n, next_n)

#define CFTL_SEQUENCE_TYPE_INDEX(n, _) \
    CFTL_SEQUENCE_TYPE_INDEX0(n, CFTL_INCREMENT(n))

namespace cftl { namespace mpl {

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

        /// if statement for computing a type given a boolean value
        template <const bool, typename IfTrue, typename IfFalse>
        class SequenceBoolIf;

        template <typename IfTrue, typename IfFalse>
        class SequenceBoolIf<true, IfTrue, IfFalse> {
        public:
            typedef IfTrue type_t;
        };

        template <typename IfTrue, typename IfFalse>
        class SequenceBoolIf<false, IfTrue, IfFalse> {
        public:
            typedef IfFalse type_t;
        };

        /// determine whether or not a type contributes toward the length
        /// of the sequence by looking at the size of the type.
        template <typename T>
        class SequenceTypeLength {
        public:
            enum {
                VALUE = (0 == SizeOf<T>::VALUE ? 0 : 1)
            };
        };

        /// if statement for computing a type given a test type.
        template <typename Test, typename IfNotUnit, typename IfUnit>
        class SequenceTypeIf {
        public:
            typedef IfNotUnit type_t;
        };

        template <typename IfNotUnit, typename IfUnit>
        class SequenceTypeIf<Unit, IfNotUnit, IfUnit> {
        public:
            typedef IfUnit type_t;
        };
    }

    /// represents a sequence of types and operations on the sequence.
    ///
    /// Examples:
    ///     Sequence<int, char, float>::Select<char>::type_t <==> char
    ///     Sequence<int, char, float>::Select<bool>::type_t <==> Unit
    ///
    ///     Sequence<int, char, float>::At<1>::type_t <==> char
    ///     Sequence<int, char, float>::At<3>::type_t <==> Unit
    ///
    ///     Sequence<int, char, float>::Length::VALUE <==> 3
    ///
    ///     Sequence<int, char, float>::Insert<double>::type_t
    ///     <==> Sequence<double, int, char, float>
    ///
    ///     Sequence<int, char, float>::Insert<char>::type_t
    ///     <==> Sequence<int, char, float>
    ///
    template<typename T0 CFTL_ENUMERATE_VALUE_PARAMS(
        CFTL_TEMPLATE_VARIABLE_LIMIT,
        T,
        typename,
        = Unit
    )>
    class Sequence : private trait::StaticOnly {
    public:

        /// easy way for nested classes to access the type of the sequence.
        typedef Sequence<
            T0
            CFTL_ENUMERATE_PARAMS(CFTL_TEMPLATE_VARIABLE_LIMIT, T)
        > self_t;

        /// the length of the type sequence. types can be ignored from the
        /// length count by specializing SizeOf to have VALUE = 0, i.e. this
        /// counts all types in the sequence that the programmer considers
        /// to be able to contain values.
        class Length : private trait::StaticOnly {
        public:
            enum {
                VALUE = (
                    SequenceTypeLength<T0>::VALUE
                    CFTL_REPEAT_LEFT(
                        CFTL_TEMPLATE_VARIABLE_LIMIT,
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
        class Select : private trait::StaticOnly {
        private:

            enum {
                MAX_TYPE_SIZE = Max<0
                    CFTL_SEQUENCE_COMPARE_TYPE(0, Q)
                    CFTL_REPEAT_LEFT(
                        CFTL_TEMPLATE_VARIABLE_LIMIT,
                        CFTL_SEQUENCE_COMPARE_TYPE,
                        Q
                    )
                >::VALUE
            };

        public:

            typedef typename SequenceBoolIf<
                (MAX_TYPE_SIZE > 0),
                Q,
                InvalidType
            >::type_t type_t;

        };

    private:

        template <const unsigned i, typename InvalidType>
        class AtImpl {
        public:
            typedef InvalidType type_t;
        };

        /// partial specializations of AtImpl
        CFTL_SEQUENCE_AT_SPEC(0, void)
        CFTL_REPEAT_LEFT(
            CFTL_TEMPLATE_VARIABLE_LIMIT,
            CFTL_SEQUENCE_AT_SPEC,
            void
        )

    public:

        /// get the type at a particular index. if the index provided is
        /// out of range then the Unit type is returned.
        template <const unsigned i, typename InvalidType=Unit>
        class At : private trait::StaticOnly {
        public:
            typedef typename AtImpl<i, InvalidType>::type_t type_t;
        };

        /// insert a type into the type sequence. if the type is in the
        /// sequence then the type of the inserted sequence remains the
        /// same.
        template <typename T, typename DontInsertType=Unit>
        class Insert : private trait::StaticOnly {
        public:

            typedef typename SequenceTypeIf<
                typename self_t::template Select<T>::type_t,
                self_t,
                Sequence<
                    T,
                    T0
                    CFTL_ENUMERATE_PARAMS(
                        CFTL_DECREMENT(CFTL_TEMPLATE_VARIABLE_LIMIT),
                        T
                    )
                >
            >::type_t type_t;
        };

        /// gets this sequence type as a sort of set.
        class UniqueTypes {
        private:

            typedef Sequence<T0> base_t0;
            CFTL_REPEAT_LEFT(
                CFTL_TEMPLATE_VARIABLE_LIMIT,
                CFTL_SEQUENCE_INSERT_TYPE,
                void
            )

        public:
            typedef CFTL_CATENATE(base_t, CFTL_TEMPLATE_VARIABLE_LIMIT)
                    type_t;
        };

        /// get the index of a particular type in the sequence. If the type
        /// is not in the sequence then the value is value_on_error (defaults
        /// to CFTL_TEMPLATE_VARIABLE_LIMIT+1).
        template <
            typename T,
            const unsigned value_on_error=CFTL_TEMPLATE_VARIABLE_LIMIT+1
        >
        class IndexOf {
        private:
            enum {
                VALUE0 = 0,
                CFTL_SEQUENCE_TYPE_INDEX(0, void)
                CFTL_REPEAT_LEFT(
                    CFTL_TEMPLATE_VARIABLE_LIMIT,
                    CFTL_SEQUENCE_TYPE_INDEX,
                    void
                )

                RESULT = CFTL_CATENATE(
                    VALUE,
                    CFTL_INCREMENT(CFTL_TEMPLATE_VARIABLE_LIMIT)
                )
            };
        public:

            enum {
                VALUE = (RESULT > 0 ? RESULT - 1 : value_on_error)
            };
        };
    };
}}


#endif /* CFTL_MPL_SEQUENCE_HPP_ */
