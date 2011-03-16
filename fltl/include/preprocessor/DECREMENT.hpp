/*
 * DECREMENT.hpp
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

#ifndef FLTL_PREPROCESSOR_DECREMENT_HPP_
#define FLTL_PREPROCESSOR_DECREMENT_HPP_

#define FLTL_DEC_0 0
#define FLTL_DEC_1 0
#define FLTL_DEC_2 1
#define FLTL_DEC_3 2
#define FLTL_DEC_4 3
#define FLTL_DEC_5 4
#define FLTL_DEC_6 5
#define FLTL_DEC_7 6
#define FLTL_DEC_8 7
#define FLTL_DEC_9 8
#define FLTL_DEC_10 9
#define FLTL_DEC_11 10
#define FLTL_DEC_12 11
#define FLTL_DEC_13 12
#define FLTL_DEC_14 13
#define FLTL_DEC_15 14
#define FLTL_DEC_16 15
#define FLTL_DEC_17 16
#define FLTL_DEC_18 17
#define FLTL_DEC_19 18
#define FLTL_DEC_20 19
#define FLTL_DEC_21 20
#define FLTL_DEC_22 21
#define FLTL_DEC_23 22
#define FLTL_DEC_24 23
#define FLTL_DEC_25 24
#define FLTL_DEC_26 25
#define FLTL_DEC_27 26
#define FLTL_DEC_28 27
#define FLTL_DEC_29 28
#define FLTL_DEC_30 29
#define FLTL_DEC_31 30
#define FLTL_DEC_32 31
#define FLTL_DEC_33 32

/// pre-processor decrement function
#define FLTL_DECREMENT0(n) FLTL_DEC_ ## n
#define FLTL_DECREMENT(n) FLTL_DECREMENT0(n)

#endif /* FLTL_PREPROCESSOR_DECREMENT_HPP_ */
