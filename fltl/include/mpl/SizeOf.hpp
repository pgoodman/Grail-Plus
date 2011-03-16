/*
 * SizeOf.hpp
 *
 *  Created on: Sep 15, 2010
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

#ifndef FLTL_MPL_SIZEOF_HPP_
#define FLTL_MPL_SIZEOF_HPP_

#include "fltl/include/mpl/If.hpp"
#include "fltl/include/mpl/Unit.hpp"

namespace fltl { namespace mpl {

    namespace detail {
        class sizeof_centinel { };
    }

    /// Get the byte size for a particular type. this allows us to say that
    /// specific types have zero size, which happens to be useful for other
    /// mpl classes.
    template <typename T>
    class SizeOf {
    public:
        enum {
            VALUE = sizeof(T)
        };
    };

    template <typename T>
    class SizeOf<const T> {
    public:
        enum {
            VALUE = SizeOf<T>::VALUE
        };
    };

    template <>
    class SizeOf<void> {
    public:
        enum {
            VALUE = 0
        };
    };

    // set the size of the Unit type, assuming it's distinct from void.
    template <>
    class SizeOf<IfEqual<void,Unit,detail::sizeof_centinel,Unit>::type> {
    public:
        enum {
            VALUE = 0
        };
    };
}}

#endif /* FLTL_MPL_SIZEOF_HPP_ */
