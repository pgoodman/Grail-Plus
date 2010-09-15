/*
 * FOLD_LEFT.hpp
 *
 *  Created on: Sep 14, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef CFTL_FOLD_LEFT_HPP_
#define CFTL_FOLD_LEFT_HPP_

#include "src/include/preprocessor/EVAL.hpp"
#include "src/include/preprocessor/UNPACK.hpp"

#define CFTL_FOLD_LEFT_0

#define CFTL_FOLD_LEFT_1(m, pd, cp) \
    CFTL_EVAL(m, (1, CFTL_UNPACK(pd), cp))

#define CFTL_FOLD_LEFT_2(m, pd, cp) \
    CFTL_FOLD_LEFT_1(m, pd, CFTL_EVAL(m, (2, CFTL_UNPACK(pd), cp)))

#define CFTL_FOLD_LEFT_3(m, pd, cp) \
    CFTL_FOLD_LEFT_2(m, pd, CFTL_EVAL(m, (3, CFTL_UNPACK(pd), cp)))

#define CFTL_FOLD_LEFT_4(m, pd, cp) \
    CFTL_FOLD_LEFT_3(m, pd, CFTL_EVAL(m, (4, CFTL_UNPACK(pd), cp)))

#define CFTL_FOLD_LEFT_5(m, pd, cp) \
    CFTL_FOLD_LEFT_4(m, pd, CFTL_EVAL(m, (5, CFTL_UNPACK(pd), cp)))

#define CFTL_FOLD_LEFT_6(m, pd, cp) \
    CFTL_FOLD_LEFT_5(m, pd, CFTL_EVAL(m, (6, CFTL_UNPACK(pd), cp)))

#define CFTL_FOLD_LEFT_7(m, pd, cp) \
    CFTL_FOLD_LEFT_6(m, pd, CFTL_EVAL(m, (7, CFTL_UNPACK(pd), cp)))

#define CFTL_FOLD_LEFT_8(m, pd, cp) \
    CFTL_FOLD_LEFT_7(m, pd, CFTL_EVAL(m, (8, CFTL_UNPACK(pd), cp)))

#define CFTL_FOLD_LEFT_9(m, pd, cp) \
    CFTL_FOLD_LEFT_8(m, pd, CFTL_EVAL(m, (9, CFTL_UNPACK(pd), cp)))

#define CFTL_FOLD_LEFT0(num_times, macro, packed_data, term_expr) \
    CFTL_FOLD_LEFT_ ## num_times(macro, packed_data, term_expr)

#define CFTL_FOLD_LEFT(num_times, macro, packed_data, term_expr) \
    CFTL_FOLD_LEFT0(num_times, macro, packed_data, term_expr)

#endif /* CFTL_FOLD_LEFT_HPP_ */
