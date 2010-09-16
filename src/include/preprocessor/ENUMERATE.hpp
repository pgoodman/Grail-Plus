/*
 * ENUMERATE.hpp
 *
 *  Created on: Sep 15, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef CFTL_ENUMERATE_HPP_
#define CFTL_ENUMERATE_HPP_

#include "src/include/preprocessor/REPEAT_LEFT.hpp"

#define CFTL_ENUMERATED_VALUE(_, d) \
    , d

#define CFTL_ENUMERATE0(n, d) \
    CFTL_REPEAT_LEFT(n, CFTL_ENUMERATED_VALUE, d)

#define CFTL_ENUMERATE(num_times, val) \
    CFTL_ENUMERATE0(num_times, val)

#endif /* CFTL_ENUMERATE_HPP_ */
