/*
 * EVAL.hpp
 *
 *  Created on: Sep 14, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef CFTL_EVAL_HPP_
#define CFTL_EVAL_HPP_

/// macro that takes in the name of a function macro and a parenthesized,
/// comma-separated list of values to pass to that function macro and then
/// expands the passed-in macro with those values as parameters.
#define CFTL_EVAL(macro, params) \
    macro params

#endif /* CFTL_EVAL_HPP_ */
