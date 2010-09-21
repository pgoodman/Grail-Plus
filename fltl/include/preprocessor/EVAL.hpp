/*
 * EVAL.hpp
 *
 *  Created on: Sep 14, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef FLTL_EVAL_HPP_
#define FLTL_EVAL_HPP_

/// macro that takes in the name of a function macro and a parenthesized,
/// comma-separated list of values to pass to that function macro and then
/// expands the passed-in macro with those values as parameters.
#define FLTL_EVAL0(m, p) m p
#define FLTL_EVAL(macro, params) FLTL_EVAL0(macro, params)

#endif /* FLTL_EVAL_HPP_ */
