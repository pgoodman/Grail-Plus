/*
 * REPEAT_RIGHT.hpp
 *
 *  Created on: Sep 14, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
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
