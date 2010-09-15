/*
 * REPEAT_RIGHT.hpp
 *
 *  Created on: Sep 14, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef CFTL_REPEAT_RIGHT_HPP_
#define CFTL_REPEAT_RIGHT_HPP_

/// pre-processor macro for repeating the application of another macro
/// a certain number of times. the application happens before the repetition
/// happens.
#define CFTL_REPEAT_RIGHT(num_times, macro, data) \
    CFTL_REPEATR_ ## num_times(macro, data)

#define CFTL_REPEATR_0(m,d)
#define CFTL_REPEATR_1(m,d) m(1, d)
#define CFTL_REPEATR_2(m,d) m(2, d) CFTL_REPEATR_1(m, d)
#define CFTL_REPEATR_3(m,d) m(3, d) CFTL_REPEATR_2(m, d)
#define CFTL_REPEATR_4(m,d) m(4, d) CFTL_REPEATR_3(m, d)
#define CFTL_REPEATR_5(m,d) m(5, d) CFTL_REPEATR_4(m, d)
#define CFTL_REPEATR_6(m,d) m(6, d) CFTL_REPEATR_5(m, d)
#define CFTL_REPEATR_7(m,d) m(7, d) CFTL_REPEATR_6(m, d)
#define CFTL_REPEATR_8(m,d) m(8, d) CFTL_REPEATR_7(m, d)
#define CFTL_REPEATR_9(m,d) m(9, d) CFTL_REPEATR_8(m, d)

#endif /* CFTL_REPEAT_RIGHT_HPP_ */
