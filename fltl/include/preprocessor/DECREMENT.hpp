/*
 * DECREMENT.hpp
 *
 *  Created on: Sep 14, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef CFTL_PREPROCESSOR_DECREMENT_HPP_
#define CFTL_PREPROCESSOR_DECREMENT_HPP_

#define CFTL_DEC_0 0
#define CFTL_DEC_1 0
#define CFTL_DEC_2 1
#define CFTL_DEC_3 2
#define CFTL_DEC_4 3
#define CFTL_DEC_5 4
#define CFTL_DEC_6 5
#define CFTL_DEC_7 6
#define CFTL_DEC_8 7
#define CFTL_DEC_9 8
#define CFTL_DEC_10 9
#define CFTL_DEC_11 10
#define CFTL_DEC_12 11

/// pre-processor decrement function
#define CFTL_DECREMENT0(n) CFTL_DEC_ ## n
#define CFTL_DECREMENT(n) CFTL_DECREMENT0(n)

#endif /* CFTL_PREPROCESSOR_DECREMENT_HPP_ */
