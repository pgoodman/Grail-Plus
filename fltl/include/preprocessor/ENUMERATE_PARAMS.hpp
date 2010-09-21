/*
 * ENUMERATE_PARAMS.hpp
 *
 *  Created on: Sep 14, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef FLTL_ENUMERATE_PARAMS_HPP_
#define FLTL_ENUMERATE_PARAMS_HPP_

#include "fltl/include/preprocessor/CATENATE.hpp"
#include "fltl/include/preprocessor/REPEAT_LEFT.hpp"

#define FLTL_ENUMERATED_PARAM(n, p) , FLTL_CATENATE(p, n)

/// pre-processor comma-separated list maker. the elements of the list are
/// prefixed with commas. the parameter name p is joined with n. E.g.
/// FLTL_ENUMERATE_TYPE_PARAMS(3, T) ==> , T2, T1, T0
#define FLTL_ENUMERATE_PARAMS0(n, p) \
    FLTL_REPEAT_LEFT(n, FLTL_ENUMERATED_PARAM, p)

#define FLTL_ENUMERATE_PARAMS(n, p) \
    FLTL_ENUMERATE_PARAMS0(n, p)

#endif /* FLTL_ENUMERATE_PARAMS_HPP_ */
