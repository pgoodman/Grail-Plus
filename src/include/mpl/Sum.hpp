/*
 * Sum.hpp
 *
 *  Created on: Sep 14, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef CFTL_MPL_SUM_HPP_
#define CFTL_MPL_SUM_HPP_

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
    , typename CFTL_CATENATE(p, n)=Unit

/// an expansion of a single value inside an enumerated type
#define CFTL_SUM_ENUMERATED_ENUM_PARAM(n, p) \
    , TYPE_ ## p ## n = n

/// macro for computing the size of a parameterized type by its index
#define CFTL_SUM_TYPE_SIZE(n, _) \
    , sizeof(T ## n)

namespace cftl {

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
    private:

        /// enumeration type defining valid accessors
        typedef enum {
            TYPE_T0 = 0
            CFTL_REPEAT_LEFT(
                CFTL_SUM_OVER_TYPES_LIMIT,
                CFTL_SUM_ENUMERATED_ENUM_PARAM,
                T
            )
        } type_t;

        enum {

            /// the maximum size of all object sizes in this sum type
            MAX_OBJECT_SIZE = Max<
                sizeof(T0)
                CFTL_REPEAT_LEFT(
                    CFTL_SUM_OVER_TYPES_LIMIT,
                    CFTL_SUM_TYPE_SIZE,
                    0
                )
            >
        };

        type_t type;

        unsigned storage[];

    public:

        Sum(void) { }

        template <typename T>
        bool hasType(void) const {

        }

        ///
        template <typename T>
        void visit(const T &visitor) {
            //visitor();
        }
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
