/*
 * STATIC_ASSERT.hpp
 *
 *  Created on: Sep 18, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef CFTL_STATIC_ASSERT_HPP_
#define CFTL_STATIC_ASSERT_HPP_

/// create a compile-time assertion. must be followed by a semicolon.
#define CFTL_STATIC_ASSERT1(cond, line) \
    char __failed_assertion_ ## line [cond ? 1 : -1]; \
    (void) __failed_assertion_ ## line

#define CFTL_STATIC_ASSERT0(cond, line) \
    CFTL_STATIC_ASSERT1(cond, line)

#define CFTL_STATIC_ASSERT(cond) \
    CFTL_STATIC_ASSERT0((cond), __LINE__)

#endif /* CFTL_STATIC_ASSERT_HPP_ */
