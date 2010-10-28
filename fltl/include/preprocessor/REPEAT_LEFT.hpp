/*
 * REPEAT_LEFT.hpp
 *
 *  Created on: Sep 14, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
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

#endif /* FLTL_REPEAT_LEFT_HPP_ */
