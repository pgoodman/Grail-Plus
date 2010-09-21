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

/// pre-processor increment function
#define FLTL_INCREMENT0(n) FLTL_INC_ ## n
#define FLTL_INCREMENT(n) FLTL_INCREMENT0(n)

#endif /* FLTL_INCREMENT_HPP_ */
