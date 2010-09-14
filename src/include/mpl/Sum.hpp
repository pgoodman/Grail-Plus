/*
 * Sum.hpp
 *
 *  Created on: Sep 14, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef CFTL_MPL_SUM_HPP_
#define CFTL_MPL_SUM_HPP_

#include "src/include/mpl/Unit.hpp"

namespace cftl {

    /// Default definition for a tagged variant/union/sum type.
    ///
    /// Note: the sum type expects all non-Unit types to be
    ///       distinct.
    template <typename T0=Unit,
              typename T1=Unit,
              typename T2=Unit,
              typename T3=Unit,
              typename T4=Unit,
              typename T5=Unit,
              typename T6=Unit,
              typename T7=Unit,
              typename T8=Unit>
    class Sum { };

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
}

#endif /* CFTL_MPL_SUM_HPP_ */
