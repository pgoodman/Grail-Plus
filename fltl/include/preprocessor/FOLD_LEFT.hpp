/*
 * FOLD_LEFT.hpp
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

#ifndef FLTL_FOLD_LEFT_HPP_
#define FLTL_FOLD_LEFT_HPP_

#include "fltl/include/preprocessor/EVAL.hpp"
#include "fltl/include/preprocessor/UNPACK.hpp"

#define FLTL_FOLD_LEFT_0

#define FLTL_FOLD_LEFT_1(m, pd, cp) \
    FLTL_EVAL(m, (1, FLTL_UNPACK(pd), cp))

#define FLTL_FOLD_LEFT_2(m, pd, cp) \
    FLTL_FOLD_LEFT_1(m, pd, FLTL_EVAL(m, (2, FLTL_UNPACK(pd), cp)))

#define FLTL_FOLD_LEFT_3(m, pd, cp) \
    FLTL_FOLD_LEFT_2(m, pd, FLTL_EVAL(m, (3, FLTL_UNPACK(pd), cp)))

#define FLTL_FOLD_LEFT_4(m, pd, cp) \
    FLTL_FOLD_LEFT_3(m, pd, FLTL_EVAL(m, (4, FLTL_UNPACK(pd), cp)))

#define FLTL_FOLD_LEFT_5(m, pd, cp) \
    FLTL_FOLD_LEFT_4(m, pd, FLTL_EVAL(m, (5, FLTL_UNPACK(pd), cp)))

#define FLTL_FOLD_LEFT_6(m, pd, cp) \
    FLTL_FOLD_LEFT_5(m, pd, FLTL_EVAL(m, (6, FLTL_UNPACK(pd), cp)))

#define FLTL_FOLD_LEFT_7(m, pd, cp) \
    FLTL_FOLD_LEFT_6(m, pd, FLTL_EVAL(m, (7, FLTL_UNPACK(pd), cp)))

#define FLTL_FOLD_LEFT_8(m, pd, cp) \
    FLTL_FOLD_LEFT_7(m, pd, FLTL_EVAL(m, (8, FLTL_UNPACK(pd), cp)))

#define FLTL_FOLD_LEFT_9(m, pd, cp) \
    FLTL_FOLD_LEFT_8(m, pd, FLTL_EVAL(m, (9, FLTL_UNPACK(pd), cp)))

#define FLTL_FOLD_LEFT_10(m, pd, cp) \
    FLTL_FOLD_LEFT_9(m, pd, FLTL_EVAL(m, (10, FLTL_UNPACK(pd), cp)))

#define FLTL_FOLD_LEFT_11(m, pd, cp) \
    FLTL_FOLD_LEFT_10(m, pd, FLTL_EVAL(m, (11, FLTL_UNPACK(pd), cp)))

#define FLTL_FOLD_LEFT0(num_times, macro, packed_data, term_expr) \
    FLTL_FOLD_LEFT_ ## num_times(macro, packed_data, term_expr)

#define FLTL_FOLD_LEFT(num_times, macro, packed_data, term_expr) \
    FLTL_FOLD_LEFT0(num_times, macro, packed_data, term_expr)

#endif /* FLTL_FOLD_LEFT_HPP_ */
