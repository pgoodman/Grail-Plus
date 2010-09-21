/*
 * CATENATE.hpp
 *
 *  Created on: Sep 14, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef FLTL_CATENATE_HPP_
#define FLTL_CATENATE_HPP_

/// pre-processor macro for concatenating two strings
#define FLTL_CATENATE0(l, r) l ## r
#define FLTL_CATENATE(left, right) FLTL_CATENATE0(left, right)

#endif /* FLTL_CATENATE_HPP_ */
