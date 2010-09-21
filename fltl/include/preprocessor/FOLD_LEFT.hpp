/*
 * FOLD_LEFT.hpp
 *
 *  Created on: Sep 14, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef FLTL_FOLD_LEFT_HPP_
#define FLTL_FOLD_LEFT_HPP_

#include "fltl/include/preprocessor/EVAL.hpp"
#include "fltl/include/preprocessor/UNPACK.hpp"

#define FLTL_FOLD_LEFT_0

#define FLTL_FOLD_LEFT_1(m, pd, cp) \
    FLTL_EVAL(m, (1, FLTL_UNPACK(pd), cp))

#define FLTL_FOLD_LEFT_2(m, pd, cp) \
    FLTL_FOLD_LEFT_1(m, pd, FLTL_EVAL(m, (2, FLTL_UNPACK(pd), cp)))

#define FLTL_FOLD_LEFT_3(m, pd, cp) \
    FLTL_FOLD_LEFT_2(m, pd, FLTL_EVAL(m, (3, FLTL_UNPACK(pd), cp)))

#define FLTL_FOLD_LEFT_4(m, pd, cp) \
    FLTL_FOLD_LEFT_3(m, pd, FLTL_EVAL(m, (4, FLTL_UNPACK(pd), cp)))

#define FLTL_FOLD_LEFT_5(m, pd, cp) \
    FLTL_FOLD_LEFT_4(m, pd, FLTL_EVAL(m, (5, FLTL_UNPACK(pd), cp)))

#define FLTL_FOLD_LEFT_6(m, pd, cp) \
    FLTL_FOLD_LEFT_5(m, pd, FLTL_EVAL(m, (6, FLTL_UNPACK(pd), cp)))

#define FLTL_FOLD_LEFT_7(m, pd, cp) \
    FLTL_FOLD_LEFT_6(m, pd, FLTL_EVAL(m, (7, FLTL_UNPACK(pd), cp)))

#define FLTL_FOLD_LEFT_8(m, pd, cp) \
    FLTL_FOLD_LEFT_7(m, pd, FLTL_EVAL(m, (8, FLTL_UNPACK(pd), cp)))

#define FLTL_FOLD_LEFT_9(m, pd, cp) \
    FLTL_FOLD_LEFT_8(m, pd, FLTL_EVAL(m, (9, FLTL_UNPACK(pd), cp)))

#define FLTL_FOLD_LEFT_10(m, pd, cp) \
    FLTL_FOLD_LEFT_9(m, pd, FLTL_EVAL(m, (10, FLTL_UNPACK(pd), cp)))

#define FLTL_FOLD_LEFT_11(m, pd, cp) \
    FLTL_FOLD_LEFT_10(m, pd, FLTL_EVAL(m, (11, FLTL_UNPACK(pd), cp)))

#define FLTL_FOLD_LEFT0(num_times, macro, packed_data, term_expr) \
    FLTL_FOLD_LEFT_ ## num_times(macro, packed_data, term_expr)

#define FLTL_FOLD_LEFT(num_times, macro, packed_data, term_expr) \
    FLTL_FOLD_LEFT0(num_times, macro, packed_data, term_expr)

#endif /* FLTL_FOLD_LEFT_HPP_ */
