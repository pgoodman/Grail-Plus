/*
 * REPEAT_LEFT.hpp
 *
 *  Created on: Sep 14, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef CFTL_REPEAT_LEFT_HPP_
#define CFTL_REPEAT_LEFT_HPP_

/// pre-processor macro for repeating the application of another macro
/// a certain number of times. the application happens after the repetition
/// happens. the macro
#define CFTL_REPEAT_LEFT0(nt, m, d) \
    CFTL_REPEATL_ ## nt(m, d)

#define CFTL_REPEAT_LEFT(num_times, macro, data) \
    CFTL_REPEAT_LEFT0(num_times, macro, data)

#define CFTL_REPEATL_0(m,d)
#define CFTL_REPEATL_1(m,d) m(1, d)
#define CFTL_REPEATL_2(m,d) CFTL_REPEATL_1(m, d) m(2, d)
#define CFTL_REPEATL_3(m,d) CFTL_REPEATL_2(m, d) m(3, d)
#define CFTL_REPEATL_4(m,d) CFTL_REPEATL_3(m, d) m(4, d)
#define CFTL_REPEATL_5(m,d) CFTL_REPEATL_4(m, d) m(5, d)
#define CFTL_REPEATL_6(m,d) CFTL_REPEATL_5(m, d) m(6, d)
#define CFTL_REPEATL_7(m,d) CFTL_REPEATL_6(m, d) m(7, d)
#define CFTL_REPEATL_8(m,d) CFTL_REPEATL_7(m, d) m(8, d)
#define CFTL_REPEATL_9(m,d) CFTL_REPEATL_8(m, d) m(9, d)
#define CFTL_REPEATL_10(m,d) CFTL_REPEATL_9(m, d) m(10, d)
#define CFTL_REPEATL_11(m,d) CFTL_REPEATL_10(m, d) m(11, d)
#define CFTL_REPEATL_12(m,d) CFTL_REPEATL_11(m, d) m(12, d)

#endif /* CFTL_REPEAT_LEFT_HPP_ */
