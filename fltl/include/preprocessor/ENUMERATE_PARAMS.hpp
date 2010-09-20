/*
 * ENUMERATE_PARAMS.hpp
 *
 *  Created on: Sep 14, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef CFTL_ENUMERATE_PARAMS_HPP_
#define CFTL_ENUMERATE_PARAMS_HPP_

#include "src/include/preprocessor/CATENATE.hpp"
#include "src/include/preprocessor/REPEAT_LEFT.hpp"

#define CFTL_ENUMERATED_PARAM(n, p) , CFTL_CATENATE(p, n)

/// pre-processor comma-separated list maker. the elements of the list are
/// prefixed with commas. the parameter name p is joined with n. E.g.
/// CFTL_ENUMERATE_TYPE_PARAMS(3, T) ==> , T2, T1, T0
#define CFTL_ENUMERATE_PARAMS0(n, p) \
    CFTL_REPEAT_LEFT(n, CFTL_ENUMERATED_PARAM, p)

#define CFTL_ENUMERATE_PARAMS(n, p) \
    CFTL_ENUMERATE_PARAMS0(n, p)

#endif /* CFTL_ENUMERATE_PARAMS_HPP_ */
