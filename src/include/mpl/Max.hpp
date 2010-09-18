/*
 * Max.hpp
 *
 *  Created on: Sep 14, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef CFTL_MPL_MAX_HPP_
#define CFTL_MPL_MAX_HPP_

#include <cstddef>

#include "src/include/preprocessor/ENUMERATE_VALUE_PARAMS.hpp"
#include "src/include/preprocessor/FOLD_LEFT.hpp"
#include "src/include/preprocessor/PACK.hpp"

#include "src/include/trait/StaticOnly.hpp"

/// the default number of types that can be summed over
#ifndef CFTL_MAX_OVER_VALUES_LIMIT
#define CFTL_MAX_OVER_VALUES_LIMIT 10
#endif

/// the fold function for computing the max value of N numbers by computing
/// the value one pair at a time.
#define CFTL_MAX_OF_2(n, _, rest) (Max2<v ## n, rest>::VALUE)

namespace cftl { namespace mpl {

    namespace {

        /// compute the maximum of two values at compile time.
        template <const std::size_t v0, const std::size_t v1>
        class Max2 {
        public:
            enum {
                VALUE = (v0 > v1 ? v0 : v1)
            };
        };
    }

    /// Compute the maximum of 1 to CFTL_MAX_OVER_VALUES_LIMIT values at
    /// compile time.
    template <const std::size_t v0
              CFTL_ENUMERATE_VALUE_PARAMS(
                  CFTL_MAX_OVER_VALUES_LIMIT, v, const std::size_t, = 0
              ) >
    class Max : private trait::StaticOnly {
    public:
        enum {
            VALUE = CFTL_FOLD_LEFT(
                CFTL_MAX_OVER_VALUES_LIMIT,
                CFTL_MAX_OF_2,
                CFTL_PACK_0,
                v0
            )
        };
    };
}}

#endif /* CFTL_MPL_MAX_HPP_ */
