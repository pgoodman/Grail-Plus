/*
 * COLOR.hpp
 *
 *  Created on: Feb 4, 2011
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

#ifndef FLTL_COLOR_HPP_
#define FLTL_COLOR_HPP_

#if defined(_MSC_VER)

#define FLTL_F_DEF ""
#define FLTL_F_YELLOW ""
#define FLTL_F_PINK ""
#define FLTL_F_GREEN ""
#define FLTL_F_RED ""
#define FLTL_F_BLUE ""

#define FLTL_F_BOLD ""
#define FLTL_F_UNDER ""

#else

#define FLTL_F_DEF "\033[0m"
#define FLTL_F_YELLOW "\033[33m"
#define FLTL_F_PINK "\033[35m"
#define FLTL_F_GREEN "\033[32m"
#define FLTL_F_RED "\033[31m"
#define FLTL_F_BLUE "\033[34m"

#define FLTL_F_BOLD "\033[1m"
#define FLTL_F_UNDER "\033[4m"

#endif

#endif /* FLTL_COLOR_HPP_ */
