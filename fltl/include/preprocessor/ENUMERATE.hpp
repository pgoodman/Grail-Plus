/*
 * ENUMERATE.hpp
 *
 *  Created on: Sep 15, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef FLTL_ENUMERATE_HPP_
#define FLTL_ENUMERATE_HPP_

#include "fltl/include/preprocessor/REPEAT_LEFT.hpp"

#define FLTL_ENUMERATED_VALUE(_, d) \
    , d

#define FLTL_ENUMERATE0(n, d) \
    FLTL_REPEAT_LEFT(n, FLTL_ENUMERATED_VALUE, d)

#define FLTL_ENUMERATE(num_times, val) \
    FLTL_ENUMERATE0(num_times, val)

#endif /* FLTL_ENUMERATE_HPP_ */
