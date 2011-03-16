/*
 * REPEAT_LEFT.hpp
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

#ifndef FLTL_REPEAT_LEFT_HPP_
#define FLTL_REPEAT_LEFT_HPP_

#define FLTL_COMMA(_) ,
#define FLTL_NOTHING(_)

/// pre-processor macro for repeating the application of another macro
/// a certain number of times. the application happens after the repetition
/// happens. the macro
#define FLTL_REPEAT_LEFT0(nt, m, d) \
    FLTL_REPEATL_ ## nt(m, d)

#define FLTL_REPEAT_LEFT(num_times, macro, data) \
    FLTL_REPEAT_LEFT0(num_times, macro, data)

#define FLTL_REPEATL_0(m,d)
#define FLTL_REPEATL_1(m,d) m(1, d)
#define FLTL_REPEATL_2(m,d) FLTL_REPEATL_1(m, d) m(2, d)
#define FLTL_REPEATL_3(m,d) FLTL_REPEATL_2(m, d) m(3, d)
#define FLTL_REPEATL_4(m,d) FLTL_REPEATL_3(m, d) m(4, d)
#define FLTL_REPEATL_5(m,d) FLTL_REPEATL_4(m, d) m(5, d)
#define FLTL_REPEATL_6(m,d) FLTL_REPEATL_5(m, d) m(6, d)
#define FLTL_REPEATL_7(m,d) FLTL_REPEATL_6(m, d) m(7, d)
#define FLTL_REPEATL_8(m,d) FLTL_REPEATL_7(m, d) m(8, d)
#define FLTL_REPEATL_9(m,d) FLTL_REPEATL_8(m, d) m(9, d)
#define FLTL_REPEATL_10(m,d) FLTL_REPEATL_9(m, d) m(10, d)
#define FLTL_REPEATL_11(m,d) FLTL_REPEATL_10(m, d) m(11, d)
#define FLTL_REPEATL_12(m,d) FLTL_REPEATL_11(m, d) m(12, d)
#define FLTL_REPEATL_13(m,d) FLTL_REPEATL_12(m, d) m(13, d)
#define FLTL_REPEATL_14(m,d) FLTL_REPEATL_13(m, d) m(14, d)
#define FLTL_REPEATL_15(m,d) FLTL_REPEATL_14(m, d) m(15, d)
#define FLTL_REPEATL_16(m,d) FLTL_REPEATL_15(m, d) m(16, d)
#define FLTL_REPEATL_17(m,d) FLTL_REPEATL_16(m, d) m(17, d)
#define FLTL_REPEATL_18(m,d) FLTL_REPEATL_17(m, d) m(18, d)
#define FLTL_REPEATL_19(m,d) FLTL_REPEATL_18(m, d) m(19, d)
#define FLTL_REPEATL_20(m,d) FLTL_REPEATL_19(m, d) m(20, d)
#define FLTL_REPEATL_21(m,d) FLTL_REPEATL_20(m, d) m(21, d)
#define FLTL_REPEATL_22(m,d) FLTL_REPEATL_21(m, d) m(22, d)
#define FLTL_REPEATL_23(m,d) FLTL_REPEATL_22(m, d) m(23, d)
#define FLTL_REPEATL_24(m,d) FLTL_REPEATL_23(m, d) m(24, d)
#define FLTL_REPEATL_25(m,d) FLTL_REPEATL_24(m, d) m(25, d)
#define FLTL_REPEATL_26(m,d) FLTL_REPEATL_25(m, d) m(26, d)
#define FLTL_REPEATL_27(m,d) FLTL_REPEATL_26(m, d) m(27, d)
#define FLTL_REPEATL_28(m,d) FLTL_REPEATL_27(m, d) m(28, d)
#define FLTL_REPEATL_29(m,d) FLTL_REPEATL_28(m, d) m(29, d)
#define FLTL_REPEATL_30(m,d) FLTL_REPEATL_29(m, d) m(30, d)
#define FLTL_REPEATL_31(m,d) FLTL_REPEATL_30(m, d) m(31, d)
#define FLTL_REPEATL_32(m,d) FLTL_REPEATL_31(m, d) m(32, d)
#define FLTL_REPEATL_33(m,d) FLTL_REPEATL_32(m, d) m(33, d)

#endif /* FLTL_REPEAT_LEFT_HPP_ */
