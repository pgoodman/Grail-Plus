/*
 * NO_INLINE.hpp
 *
 *  Created on: Jan 21, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_NO_INLINE_HPP_
#define FLTL_NO_INLINE_HPP_

#if defined(_WIN32)
#define FLTL_NO_INLINE __declspec(noinline)
#elif defined(__GNUC__) || defined(__GNUG__)
#define FLTL_NO_INLINE __attribute__((noinline))
#elif defined(__clang__)
#define FLTL_NO_INLINE __attribute__((noinline))
#else
#define FLTL_NO_INLINE
#endif

#endif /* FLTL_NO_INLINE_HPP_ */
