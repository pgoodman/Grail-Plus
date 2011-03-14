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
#include "fltl/include/preprocessor/VARG_CHECK_FORMAT.hpp"

namespace grail { namespace io {

    int verbose(const char *str, ...) throw() FLTL_VARG_CHECK_FORMAT(1,2);

    void verbose_enable(void) throw();
    void verbose_disable(void) throw();
}}

#endif /* FLTL_VERBOSE_HPP_ */
