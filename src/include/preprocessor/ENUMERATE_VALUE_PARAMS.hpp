/*
 * ENUMERATE_VALUE_PARAMS.hpp
 *
 *  Created on: Sep 14, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef CFTL_ENUMERATE_VALUE_PARAMS_HPP_
#define CFTL_ENUMERATE_VALUE_PARAMS_HPP_

#include "src/include/preprocessor/CATENATE.hpp"
#include "src/include/preprocessor/EVAL.hpp"
#include "src/include/preprocessor/REPEAT_LEFT.hpp"
#include "src/include/preprocessor/UNPACK.hpp"

/// the definition of a value param.
#define CFTL_VALUE_PARAM(type, param, default_assign, num) \
    type CFTL_CATENATE(param, num) default_assign

/// unboxes the data into a call to the VALUE_PARAM macro
#define CFTL_ENUMERATED_VALUE_PARAM(n, d) \
    , CFTL_EVAL(CFTL_VALUE_PARAM, (CFTL_UNPACK_3(d), n))

/// pre-processor comma-separated list maker. the elements of the list are
/// prefixed with commas. the parameter name p is joined with n. E.g.
/// CFTL_ENUMERATE_TYPE_PARAMS(3, T) ==> , T2, T1, T0
#define CFTL_ENUMERATE_VALUE_PARAMS0(n, p, type, default_assign) \
    CFTL_REPEAT_LEFT(n, CFTL_ENUMERATED_VALUE_PARAM, (type, p, default_assign))

#define CFTL_ENUMERATE_VALUE_PARAMS(n, p, type, default_assign) \
    CFTL_ENUMERATE_VALUE_PARAMS0(n, p, type, default_assign)

#endif /* CFTL_ENUMERATE_VALUE_PARAMS_HPP_ */
