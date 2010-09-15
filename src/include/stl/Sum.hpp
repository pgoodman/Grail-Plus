/*
 * Sum.hpp
 *
 *  Created on: Sep 14, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef CFTL_MPL_SUM_HPP_
#define CFTL_MPL_SUM_HPP_

#include <cstddef>

#include "src/include/preprocessor/CATENATE.hpp"
#include "src/include/preprocessor/REPEAT_LEFT.hpp"

#include "src/include/mpl/Max.hpp"
#include "src/include/mpl/Unit.hpp"

/// the default number of types that can be summed over
#ifndef CFTL_SUM_OVER_TYPES_LIMIT
#define CFTL_SUM_OVER_TYPES_LIMIT 7
#endif

/// an expansion of a single parameterized type with a default type of Unit
#define CFTL_SUM_ENUMERATED_TPL_PARAM(n, p) \
    , typename CFTL_CATENATE(p, n)=SumUnit ## n

/// an expansion of a single value inside an enumerated type
#define CFTL_SUM_ENUMERATED_ENUM_PARAM(n, p) \
    , TYPE_ ## p ## n = n

/// macro for computing the size of a parameterized type by its index
#define CFTL_SUM_TYPE_SIZE(n, _) \
    , sizeof(T ## n)

#define CFTL_MAX_SUM_UNIT(n, _) \
    class SumUnit ## n { };

namespace cftl {

    namespace {

        /// type used for calculating how much extra space is needed to add
        /// on to the end of the storage so as to be able to hold a value of
        /// any of the types in the Sum

        template <typename T, const std::size_t k>
        class Padding {
        public:
            enum { VALUE = sizeof(T) - k };
        };

        template <typename T>
        class Padding<T, 0UL> {
        public:
            enum { VALUE = 0 };
        };

        /// make the unit classes. these classes are meant to be distinct.
        /// also, they are in the anonymous namespace and so they cannot be
        /// used by someone outside of this file, i.e. no one can ever
        /// make a sum that explicity uses one of the SumUnit classes.
        CFTL_REPEAT_LEFT(CFTL_SUM_OVER_TYPES_LIMIT, CFTL_MAX_SUM_UNIT, void)
    }

    /// Default definition for a tagged variant/union/sum type.
    ///
    /// Note: the sum type expects all non-Unit types to be
    ///       distinct.
    template <typename T0 \
              CFTL_REPEAT_LEFT(
                  CFTL_SUM_OVER_TYPES_LIMIT,
                  CFTL_SUM_ENUMERATED_TPL_PARAM,
                  T
              )>
    class Sum {
    public:

        /// enumeration type defining valid accessors
        typedef enum {
            TYPE_T0 = 0
            CFTL_REPEAT_LEFT(
                CFTL_SUM_OVER_TYPES_LIMIT,
                CFTL_SUM_ENUMERATED_ENUM_PARAM,
                T
            )
        } tag_t;

        typedef unsigned storage_t;

        enum {

            STORAGE_SIZE = sizeof(storage_t),

            /// the maximum size of all object sizes in this sum type
            MAX_OBJECT_SIZE = Max<
                sizeof(T0)
                CFTL_REPEAT_LEFT(
                    CFTL_SUM_OVER_TYPES_LIMIT,
                    CFTL_SUM_TYPE_SIZE,
                    STORAGE_SIZE
                )
            >::VALUE
        };

        enum {
            MOS_DIV_0 = MAX_OBJECT_SIZE / STORAGE_SIZE,
            MOS_DIV = (0 == MOS_DIV_0 ? 1 : MOS_DIV_0),
            MOS_MOD = MAX_OBJECT_SIZE % STORAGE_SIZE,

            PADDING = Padding<storage_t, MOS_MOD>::VALUE,
            MEMORY_SIZE = MOS_DIV + PADDING
        };

        /// type tag
        tag_t type;

        /// memory in which a value of any of the types Ti will be stored
        storage_t storage[MEMORY_SIZE];

    public:

        Sum(void) : type(TYPE_T0) { }
    };



#if 0
    template <typename T0>
    class Sum<T0,Unit,Unit,Unit,Unit,Unit,Unit,Unit,Unit> {

    };

    template <typename T0, typename T1>
    class Sum<T0,T1,Unit,Unit,Unit,Unit,Unit,Unit,Unit> {

    };

    template <typename T0, typename T1, typename T2>
    class Sum<T0,T1,T2,Unit,Unit,Unit,Unit,Unit,Unit> {

    };

    template <typename T0, typename T1, typename T2, typename T3>
    class Sum<T0,T1,T2,T3,Unit,Unit,Unit,Unit,Unit> {

    };

    template <typename T0, typename T1, typename T2, typename T3,
              typename T4>
    class Sum<T0,T1,T2,T3,T4,Unit,Unit,Unit,Unit> {

    };

    template <typename T0, typename T1, typename T2, typename T3,
              typename T4, typename T5>
    class Sum<T0,T1,T2,T3,T4,T5,Unit,Unit,Unit> {

    };

    template <typename T0, typename T1, typename T2, typename T3,
              typename T4, typename T5, typename T6>
    class Sum<T0,T1,T2,T3,T4,T5,T6,Unit,Unit> {

    };

    template <typename T0, typename T1, typename T2, typename T3,
              typename T4, typename T5, typename T6, typename T7>
    class Sum<T0,T1,T2,T3,T4,T5,T6,T7,Unit> {

    };
#endif
}

#endif /* CFTL_MPL_SUM_HPP_ */
