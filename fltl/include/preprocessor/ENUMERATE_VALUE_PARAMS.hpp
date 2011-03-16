/*
 * ENUMERATE_VALUE_PARAMS.hpp
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

#ifndef FLTL_ENUMERATE_VALUE_PARAMS_HPP_
#define FLTL_ENUMERATE_VALUE_PARAMS_HPP_

#include "fltl/include/preprocessor/CATENATE.hpp"
#include "fltl/include/preprocessor/EVAL.hpp"
#include "fltl/include/preprocessor/REPEAT_LEFT.hpp"
#include "fltl/include/preprocessor/UNPACK.hpp"

/// the definition of a value param.
#define FLTL_VALUE_PARAM(type, param, default_assign, num) \
    type FLTL_CATENATE(param, num) default_assign

/// unboxes the data into a call to the VALUE_PARAM macro
#define FLTL_ENUMERATED_VALUE_PARAM(n, d) \
    , FLTL_EVAL(FLTL_VALUE_PARAM, (FLTL_UNPACK_3(d), n))

/// pre-processor comma-separated list maker. the elements of the list are
/// prefixed with commas. the parameter name p is joined with n. E.g.
/// FLTL_ENUMERATE_TYPE_PARAMS(3, T) ==> , T2, T1, T0
#define FLTL_ENUMERATE_VALUE_PARAMS0(n, p, type, default_assign) \
    FLTL_REPEAT_LEFT(n, FLTL_ENUMERATED_VALUE_PARAM, (type, p, default_assign))

#define FLTL_ENUMERATE_VALUE_PARAMS(n, p, type, default_assign) \
    FLTL_ENUMERATE_VALUE_PARAMS0(n, p, type, default_assign)

#endif /* FLTL_ENUMERATE_VALUE_PARAMS_HPP_ */
