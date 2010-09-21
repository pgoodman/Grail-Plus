/*
 * STATIC_ASSERT.hpp
 *
 *  Created on: Sep 18, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef FLTL_STATIC_ASSERT_HPP_
#define FLTL_STATIC_ASSERT_HPP_

/// create a compile-time assertion. must be followed by a semicolon.
#define FLTL_STATIC_ASSERT1(cond, line) \
    char __failed_assertion_ ## line [cond ? 1 : -1]; \
    (void) __failed_assertion_ ## line

#define FLTL_STATIC_ASSERT0(cond, line) \
    FLTL_STATIC_ASSERT1(cond, line)

#define FLTL_STATIC_ASSERT(cond) \
    FLTL_STATIC_ASSERT0((cond), __LINE__)

#endif /* FLTL_STATIC_ASSERT_HPP_ */
