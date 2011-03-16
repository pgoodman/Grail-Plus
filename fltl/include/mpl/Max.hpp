/*
 * Max.hpp
 *
 *  Created on: Sep 14, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
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
#define FLTL_MAX_OF_2(n, _, rest) (detail::Max2<v ## n, rest>::VALUE)

namespace fltl { namespace mpl {

    namespace detail {

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
