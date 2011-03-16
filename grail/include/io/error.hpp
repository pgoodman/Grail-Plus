/*
 * error.hpp
 *
 *  Created on: Feb 20, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
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
