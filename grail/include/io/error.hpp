/*
 * error.hpp
 *
 *  Created on: Feb 20, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_IO_ERROR_HPP_
#define FLTL_IO_ERROR_HPP_

#include <cstring>
#include <cstdio>
#include <cstdarg>

#include "fltl/include/preprocessor/COLOR.hpp"
#include "fltl/include/preprocessor/VARG_CHECK_FORMAT.hpp"

namespace grail { namespace io {

    void error(const char *str, ...) FLTL_VARG_CHECK_FORMAT(1,2);

    void error(
        const char * const file_name,
        const unsigned line,
        const unsigned col,
        const char *str,
        ...
    ) FLTL_VARG_CHECK_FORMAT(4,5);

}}

#endif /* FLTL_IO_ERROR_HPP_ */
