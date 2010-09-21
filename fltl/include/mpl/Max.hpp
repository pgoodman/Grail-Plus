/*
 * Max.hpp
 *
 *  Created on: Sep 14, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef FLTL_MPL_MAX_HPP_
#define FLTL_MPL_MAX_HPP_

#include <cstddef>

#include "fltl/include/preprocessor/TEMPLATE_VARIABLE_LIMIT.hpp"
#include "fltl/include/preprocessor/ENUMERATE_VALUE_PARAMS.hpp"
#include "fltl/include/preprocessor/FOLD_LEFT.hpp"
#include "fltl/include/preprocessor/PACK.hpp"

#include "fltl/include/trait/StaticOnly.hpp"

#define FLTL_MAX_TEMPLATE_VARIABLE_LIMIT 10

#if FLTL_TEMPLATE_VARIABLE_LIMIT > (FLTL_MAX_TEMPLATE_VARIABLE_LIMIT - 3)
#error "The Max template must accept more template arguments."
#endif

/// the fold function for computing the max value of N numbers by computing
/// the value one pair at a time.
#define FLTL_MAX_OF_2(n, _, rest) (Max2<v ## n, rest>::VALUE)

namespace fltl { namespace mpl {

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

    /// Compute the maximum of 1 to FLTL_MAX_TEMPLATE_VARIABLE_LIMIT values at
    /// compile time.
    template <
        const std::size_t v0
        FLTL_ENUMERATE_VALUE_PARAMS(
            FLTL_MAX_TEMPLATE_VARIABLE_LIMIT,
            v,
            const std::size_t,
            = 0
        )
    >
    class Max : private trait::StaticOnly {
    public:
        enum {
            VALUE = FLTL_FOLD_LEFT(
                FLTL_MAX_TEMPLATE_VARIABLE_LIMIT,
                FLTL_MAX_OF_2,
                FLTL_PACK_0,
                v0
            )
        };
    };
}}

#endif /* FLTL_MPL_MAX_HPP_ */
