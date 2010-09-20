/*
 * INCREMENT.hpp
 *
 *  Created on: Sep 15, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef CFTL_INCREMENT_HPP_
#define CFTL_INCREMENT_HPP_

#define CFTL_INC_0 1
#define CFTL_INC_1 2
#define CFTL_INC_2 3
#define CFTL_INC_3 4
#define CFTL_INC_4 5
#define CFTL_INC_5 6
#define CFTL_INC_6 7
#define CFTL_INC_7 8
#define CFTL_INC_8 9
#define CFTL_INC_9 10
#define CFTL_INC_10 11
#define CFTL_INC_11 12
#define CFTL_INC_12 13

/// pre-processor increment function
#define CFTL_INCREMENT0(n) CFTL_INC_ ## n
#define CFTL_INCREMENT(n) CFTL_INCREMENT0(n)

#endif /* CFTL_INCREMENT_HPP_ */
