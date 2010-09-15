/*
 * UNPACK.hpp
 *
 *  Created on: Sep 14, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef CFTL_UNPACK_HPP_
#define CFTL_UNPACK_HPP_

#include "src/include/preprocessor/EVAL.hpp"

/// pre-processor macros for unpacking parenthesized comma-separated lists
/// of values into un-parenthesized comma-separated lists of those values.
#define CFTL_I_UNPACK_0(d0) void
#define CFTL_I_UNPACK_1(d0) d0
#define CFTL_I_UNPACK_2(d0, d1) d0, d1
#define CFTL_I_UNPACK_3(d0, d1, d2) d0, d1, d2
#define CFTL_I_UNPACK_4(d0, d1, d2, d3) d0, d1, d2, d3
#define CFTL_I_UNPACK_5(d0, d1, d2, d3, d4) d0, d1, d2, d3, d4

#define CFTL_UNPACK_0(d) void
#define CFTL_UNPACK_1(d) CFTL_I_UNPACK_1 d
#define CFTL_UNPACK_2(d) CFTL_I_UNPACK_2 d
#define CFTL_UNPACK_3(d) CFTL_I_UNPACK_3 d
#define CFTL_UNPACK_4(d) CFTL_I_UNPACK_4 d
#define CFTL_UNPACK_5(d) CFTL_I_UNPACK_5 d

/// variadic unpack where the data tuple is stored inside a 2-tuple with
/// first value being the number of elements in the data tuple
#define CFTL_UNPACK_VAR(num, d) \
    CFTL_I_UNPACK_ ## num d

#define CFTL_UNPACK(d) \
    CFTL_EVAL(CFTL_UNPACK_VAR, d)

#endif /* CFTL_UNPACK_HPP_ */
