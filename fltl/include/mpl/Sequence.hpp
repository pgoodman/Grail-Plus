/*
 * Sequence.hpp
 *
 *  Created on: Sep 15, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef FLTL_MPL_SEQUENCE_HPP_
#define FLTL_MPL_SEQUENCE_HPP_

#include "fltl/include/preprocessor/TEMPLATE_VARIABLE_LIMIT.hpp"
#include "fltl/include/preprocessor/CATENATE.hpp"
#include "fltl/include/preprocessor/DECREMENT.hpp"
#include "fltl/include/preprocessor/ENUMERATE_PARAMS.hpp"
#include "fltl/include/preprocessor/ENUMERATE_VALUE_PARAMS.hpp"
#include "fltl/include/preprocessor/EVAL.hpp"
#include "fltl/include/preprocessor/INCREMENT.hpp"
#include "fltl/include/preprocessor/REPEAT_LEFT.hpp"
#include "fltl/include/preprocessor/UNPACK.hpp"

#include "fltl/include/mpl/Max.hpp"
#include "fltl/include/mpl/SizeOf.hpp"
#include "fltl/include/mpl/Unit.hpp"

#define FLTL_SEQUENCE_COMPARE_TYPE(n, t) \
    , SizeOf<typename detail::SequenceTypeEq<t, T ## n >::type >::VALUE

#define FLTL_SEQUENCE_SPEC_TYPENAME_LIST \
    typename T0 \
    FLTL_ENUMERATE_PARAMS(FLTL_TEMPLATE_VARIABLE_LIMIT,typename T)

#define FLTL_SEQUENCE_TYPE_PARAM_LIST \
    T0 FLTL_ENUMERATE_PARAMS(FLTL_TEMPLATE_VARIABLE_LIMIT, T)

#define FLTL_SEQUENCE_AT_SPEC(n, _) \
    template <typename InvalidType> \
    class AtImpl<n, InvalidType> { \
    public: \
        typedef T ## n type; \
    };

#define FLTL_SEQUENCE_TYPE_LENGTH(n, _) \
    + detail::SequenceTypeLength<T ## n>::VALUE

#define FLTL_SEQUENCE_INSERT_TYPE(n, _) \
    typedef typename FLTL_CATENATE(base_t, FLTL_DECREMENT(n))::\
    template Insert<T ## n>::type base_t ## n;

#define FLTL_SEQUENCE_TYPE_INDEX1(n, next_n) \
    VALUE ## next_n = mpl::SizeOf< \
        typename detail::SequenceTypeEq<T ## n, T>::type \
    >::VALUE == 0 ? VALUE ## n : \
    (VALUE ## n > 0 ? VALUE ## n : next_n),

#define FLTL_SEQUENCE_TYPE_INDEX0(n, next_n) \
    FLTL_SEQUENCE_TYPE_INDEX1(n, next_n)

#define FLTL_SEQUENCE_TYPE_INDEX(n, _) \
    FLTL_SEQUENCE_TYPE_INDEX0(n, FLTL_INCREMENT(n))

namespace fltl { namespace mpl {

    namespace detail {

        /// type equivalence, if two types are different then this yields the
        /// unit type
        template <typename A, typename B>
        class SequenceTypeEq {
        public:
            typedef Unit type;
        };

        /// type equivalence, if two types are the same then this yields
        /// the type that was compared.
        template <typename A>
        class SequenceTypeEq<A, A> {
        public:
            typedef A type;
        };

        /// if we're requesting a const and the stored type might be const
        /// then we return the type as const.
        template <typename A>
        class SequenceTypeEq<const A, A> {
        public:
            typedef const A type;
        };

        /// const-correctness for type equivalence. if one of the types in
        /// sum is const, but we're requesting with a non-const, then we
        /// see that as a failure to match
        template <typename A>
        class SequenceTypeEq<A, const A> {
        public:
            typedef Unit type;
        };

        /// if statement for computing a type given a boolean value
        template <const bool, typename IfTrue, typename IfFalse>
        class SequenceBoolIf;

        template <typename IfTrue, typename IfFalse>
        class SequenceBoolIf<true, IfTrue, IfFalse> {
        public:
            typedef IfTrue type;
        };

        template <typename IfTrue, typename IfFalse>
        class SequenceBoolIf<false, IfTrue, IfFalse> {
        public:
            typedef IfFalse type;
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
            typedef IfNotUnit type;
        };

        template <typename IfNotUnit, typename IfUnit>
        class SequenceTypeIf<Unit, IfNotUnit, IfUnit> {
        public:
            typedef IfUnit type;
        };
    }

    /// represents a sequence of types and operations on the sequence.
    ///
    /// Examples:
    ///     Sequence<int, char, float>::Select<char>::type <==> char
    ///     Sequence<int, char, float>::Select<bool>::type <==> Unit
    ///
    ///     Sequence<int, char, float>::At<1>::type <==> char
    ///     Sequence<int, char, float>::At<3>::type <==> Unit
    ///
    ///     Sequence<int, char, float>::Length::VALUE <==> 3
    ///
    ///     Sequence<int, char, float>::Insert<double>::type
    ///     <==> Sequence<double, int, char, float>
    ///
    ///     Sequence<int, char, float>::Insert<char>::type
    ///     <==> Sequence<int, char, float>
    ///
    template<typename T0 FLTL_ENUMERATE_VALUE_PARAMS(
        FLTL_TEMPLATE_VARIABLE_LIMIT,
        T,
        typename,
        = Unit
    )>
    class Sequence {
    public:

        /// easy way for nested classes to access the type of the sequence.
        typedef Sequence<
            T0
            FLTL_ENUMERATE_PARAMS(FLTL_TEMPLATE_VARIABLE_LIMIT, T)
        > self_t;

        /// the length of the type sequence. types can be ignored from the
        /// length count by specializing SizeOf to have VALUE = 0, i.e. this
        /// counts all types in the sequence that the programmer considers
        /// to be able to contain values.
        class Length {
        public:
            enum {
                VALUE = (
                    detail::SequenceTypeLength<T0>::VALUE
                    FLTL_REPEAT_LEFT(
                        FLTL_TEMPLATE_VARIABLE_LIMIT,
                        FLTL_SEQUENCE_TYPE_LENGTH,
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
                    FLTL_SEQUENCE_COMPARE_TYPE(0, Q)
                    FLTL_REPEAT_LEFT(
                        FLTL_TEMPLATE_VARIABLE_LIMIT,
                        FLTL_SEQUENCE_COMPARE_TYPE,
                        Q
                    )
                >::VALUE
            };

        public:

            typedef typename detail::SequenceBoolIf<
                (MAX_TYPE_SIZE > 0),
                Q,
                InvalidType
            >::type type;

        };

    private:

        template <const unsigned i, typename InvalidType>
        class AtImpl {
        public:
            typedef InvalidType type;
        };

        /// partial specializations of AtImpl
        FLTL_SEQUENCE_AT_SPEC(0, void)
        FLTL_REPEAT_LEFT(
            FLTL_TEMPLATE_VARIABLE_LIMIT,
            FLTL_SEQUENCE_AT_SPEC,
            void
        )

    public:

        /// get the type at a particular index. if the index provided is
        /// out of range then the Unit type is returned.
        template <const unsigned i, typename InvalidType=Unit>
        class At {
        public:
            typedef typename AtImpl<i, InvalidType>::type type;
        };

        /// insert a type into the type sequence. if the type is in the
        /// sequence then the type of the inserted sequence remains the
        /// same.
        template <typename T, typename DontInsertType=Unit>
        class Insert {
        public:

            typedef typename detail::SequenceTypeIf<
                typename self_t::template Select<T>::type,
                self_t,
                Sequence<
                    T,
                    T0
                    FLTL_ENUMERATE_PARAMS(
                        FLTL_DECREMENT(FLTL_TEMPLATE_VARIABLE_LIMIT),
                        T
                    )
                >
            >::type type;
        };

        /// gets this sequence type as a sort of set.
        class UniqueTypes {
        private:

            typedef Sequence<T0> base_t0;
            FLTL_REPEAT_LEFT(
                FLTL_TEMPLATE_VARIABLE_LIMIT,
                FLTL_SEQUENCE_INSERT_TYPE,
                void
            )

        public:
            typedef FLTL_CATENATE(base_t, FLTL_TEMPLATE_VARIABLE_LIMIT)
                    type;
        };

        /// get the index of a particular type in the sequence. If the type
        /// is not in the sequence then the value is value_on_error (defaults
        /// to FLTL_TEMPLATE_VARIABLE_LIMIT+1).
        template <
            typename T,
            const unsigned value_on_error=FLTL_TEMPLATE_VARIABLE_LIMIT+1
        >
        class IndexOf {
        private:
            enum {
                VALUE0 = 0,
                FLTL_SEQUENCE_TYPE_INDEX(0, void)
                FLTL_REPEAT_LEFT(
                    FLTL_TEMPLATE_VARIABLE_LIMIT,
                    FLTL_SEQUENCE_TYPE_INDEX,
                    void
                )

                RESULT = FLTL_CATENATE(
                    VALUE,
                    FLTL_INCREMENT(FLTL_TEMPLATE_VARIABLE_LIMIT)
                )
            };
        public:

            enum {
                VALUE = (RESULT > 0 ? RESULT - 1 : value_on_error)
            };
        };
    };
}}


#endif /* FLTL_MPL_SEQUENCE_HPP_ */
