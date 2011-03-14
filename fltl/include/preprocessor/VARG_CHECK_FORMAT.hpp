/*
 * VARG_CHECK_FORMAT.hpp
 *
 *  Created on: Mar 14, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_VARG_CHECK_FORMAT_HPP_
#define FLTL_VARG_CHECK_FORMAT_HPP_

#if defined(__GNUC__)
#define FLTL_VARG_CHECK_FORMAT(fmt,start) __attribute__ ((format (printf, fmt, start)))
#else
#define FLTL_VARG_CHECK_FORMAT(fmt,start)
#endif

#endif /* FLTL_VARG_CHECK_FORMAT_HPP_ */
