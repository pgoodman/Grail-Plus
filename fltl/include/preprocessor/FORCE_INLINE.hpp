/*
 * FORCE_INLINE.hpp
 *
 *  Created on: Jan 23, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_FORCE_INLINE_HPP_
#define FLTL_FORCE_INLINE_HPP_

#if defined(__INTEL_COMPILER)
#define FLTL_FORCE_INLINE __inline
#elif defined(_MSC_VER) && (_MSC_VER >= 1200)
#define FLTL_FORCE_INLINE __forceinline
#elif defined(__GNUC__) && ((__GNUC__ > 3) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 2)))
#define FLTL_FORCE_INLINE __attribute__((always_inline))
#elif defined(__clang__)
#define FLTL_FORCE_INLINE __attribute__((always_inline))
#else
#define FLTL_FORCE_INLINE inline
#endif

#endif /* FLTL_FORCE_INLINE_HPP_ */
