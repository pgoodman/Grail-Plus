/*
 * INCREMENT.hpp
 *
 *  Created on: Sep 15, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
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
