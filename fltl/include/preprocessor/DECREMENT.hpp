/*
 * DECREMENT.hpp
 *
 *  Created on: Sep 14, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
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

/// pre-processor decrement function
#define FLTL_DECREMENT0(n) FLTL_DEC_ ## n
#define FLTL_DECREMENT(n) FLTL_DECREMENT0(n)

#endif /* FLTL_PREPROCESSOR_DECREMENT_HPP_ */
