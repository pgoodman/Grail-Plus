/*
 * verbose.hpp
 *
 *  Created on: Mar 11, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_VERBOSE_HPP_
#define FLTL_VERBOSE_HPP_

#include <cstring>
#include <cstdio>
#include <cstdarg>

#include "fltl/include/preprocessor/COLOR.hpp"

#if defined(__GNUC__)
#define GCC_PRINTF(fmt,start) __attribute__ ((format (printf, fmt, start)))
#else
#define GCC_PRINTF(fmt,start)
#endif

namespace grail { namespace io {

    int verbose(const char *str, ...) throw() GCC_PRINTF(1,2);

    void verbose_enable(void) throw();
    void verbose_disable(void) throw();
}}

#endif /* FLTL_VERBOSE_HPP_ */
