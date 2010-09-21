/*
 * UNPACK.hpp
 *
 *  Created on: Sep 14, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef FLTL_UNPACK_HPP_
#define FLTL_UNPACK_HPP_

#include "fltl/include/preprocessor/EVAL.hpp"

/// pre-processor macros for unpacking parenthesized comma-separated lists
/// of values into un-parenthesized comma-separated lists of those values.
#define FLTL_UNPACK_IMM_0(d0) void
#define FLTL_UNPACK_IMM_1(d0) d0
#define FLTL_UNPACK_IMM_2(d0, d1) d0, d1
#define FLTL_UNPACK_IMM_3(d0, d1, d2) d0, d1, d2
#define FLTL_UNPACK_IMM_4(d0, d1, d2, d3) d0, d1, d2, d3
#define FLTL_UNPACK_IMM_5(d0, d1, d2, d3, d4) d0, d1, d2, d3, d4
#define FLTL_UNPACK_IMM_6(d0, d1, d2, d3, d4, d5) d0, d1, d2, d3, d4, d5
#define FLTL_UNPACK_IMM_7(d0, d1, d2, d3, d4, d5, d6) d0, d1, d2, d3, d4, d5, d6
#define FLTL_UNPACK_IMM_8(d0, d1, d2, d3, d4, d5, d6, d7) d0, d1, d2, d3, d4, d5, d6, d7
#define FLTL_UNPACK_IMM_9(d0, d1, d2, d3, d4, d5, d6, d7, d8) d0, d1, d2, d3, d4, d5, d6, d7, d8

#define FLTL_UNPACK_0(d) void
#define FLTL_UNPACK_1(d) FLTL_UNPACK_IMM_1 d
#define FLTL_UNPACK_2(d) FLTL_UNPACK_IMM_2 d
#define FLTL_UNPACK_3(d) FLTL_UNPACK_IMM_3 d
#define FLTL_UNPACK_4(d) FLTL_UNPACK_IMM_4 d
#define FLTL_UNPACK_5(d) FLTL_UNPACK_IMM_5 d
#define FLTL_UNPACK_6(d) FLTL_UNPACK_IMM_6 d
#define FLTL_UNPACK_7(d) FLTL_UNPACK_IMM_7 d
#define FLTL_UNPACK_8(d) FLTL_UNPACK_IMM_8 d
#define FLTL_UNPACK_9(d) FLTL_UNPACK_IMM_9 d

/// variadic unpack where the data tuple is stored inside a 2-tuple with
/// first value being the number of elements in the data tuple
#define FLTL_UNPACK_VAR(num, d) \
    FLTL_I_UNPACK_ ## num d

#define FLTL_UNPACK(d) \
    FLTL_EVAL(FLTL_UNPACK_VAR, d)

#endif /* FLTL_UNPACK_HPP_ */
