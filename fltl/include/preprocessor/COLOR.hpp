/*
 * COLOR.hpp
 *
 *  Created on: Feb 4, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
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
