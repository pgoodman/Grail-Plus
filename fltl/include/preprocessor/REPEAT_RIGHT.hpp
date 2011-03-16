/*
 * REPEAT_RIGHT.hpp
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

#ifndef FLTL_REPEAT_RIGHT_HPP_
#define FLTL_REPEAT_RIGHT_HPP_

/// pre-processor macro for repeating the application of another macro
/// a certain number of times. the application happens before the repetition
/// happens.
#define FLTL_REPEAT_RIGHT(num_times, macro, data) \
    FLTL_REPEATR_ ## num_times(macro, data)

#define FLTL_REPEATR_0(m,d)
#define FLTL_REPEATR_1(m,d) m(1, d)
#define FLTL_REPEATR_2(m,d) m(2, d) FLTL_REPEATR_1(m, d)
#define FLTL_REPEATR_3(m,d) m(3, d) FLTL_REPEATR_2(m, d)
#define FLTL_REPEATR_4(m,d) m(4, d) FLTL_REPEATR_3(m, d)
#define FLTL_REPEATR_5(m,d) m(5, d) FLTL_REPEATR_4(m, d)
#define FLTL_REPEATR_6(m,d) m(6, d) FLTL_REPEATR_5(m, d)
#define FLTL_REPEATR_7(m,d) m(7, d) FLTL_REPEATR_6(m, d)
#define FLTL_REPEATR_8(m,d) m(8, d) FLTL_REPEATR_7(m, d)
#define FLTL_REPEATR_9(m,d) m(9, d) FLTL_REPEATR_8(m, d)
#define FLTL_REPEATR_10(m,d) m(10, d) FLTL_REPEATR_9(m, d)
#define FLTL_REPEATR_11(m,d) m(11, d) FLTL_REPEATR_10(m, d)
#define FLTL_REPEATR_10(m,d) m(12, d) FLTL_REPEATR_11(m, d)

#endif /* FLTL_REPEAT_RIGHT_HPP_ */
