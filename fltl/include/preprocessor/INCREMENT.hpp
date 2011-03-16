/*
 * INCREMENT.hpp
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

#ifndef FLTL_INCREMENT_HPP_
#define FLTL_INCREMENT_HPP_

#define FLTL_INC_0 1
#define FLTL_INC_1 2
#define FLTL_INC_2 3
#define FLTL_INC_3 4
#define FLTL_INC_4 5
#define FLTL_INC_5 6
#define FLTL_INC_6 7
#define FLTL_INC_7 8
#define FLTL_INC_8 9
#define FLTL_INC_9 10
#define FLTL_INC_10 11
#define FLTL_INC_11 12
#define FLTL_INC_12 13
#define FLTL_INC_13 14
#define FLTL_INC_14 15
#define FLTL_INC_15 16
#define FLTL_INC_16 17
#define FLTL_INC_17 18
#define FLTL_INC_18 19
#define FLTL_INC_19 20
#define FLTL_INC_20 21
#define FLTL_INC_21 22
#define FLTL_INC_22 23
#define FLTL_INC_23 24
#define FLTL_INC_24 25
#define FLTL_INC_25 26
#define FLTL_INC_26 27
#define FLTL_INC_27 28
#define FLTL_INC_28 29
#define FLTL_INC_29 30
#define FLTL_INC_30 31
#define FLTL_INC_31 32
#define FLTL_INC_32 33

/// pre-processor increment function
#define FLTL_INCREMENT0(n) FLTL_INC_ ## n
#define FLTL_INCREMENT(n) FLTL_INCREMENT0(n)

#endif /* FLTL_INCREMENT_HPP_ */
