/*
 * CATENATE.hpp
 *
 *  Created on: Sep 14, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef CFTL_CATENATE_HPP_
#define CFTL_CATENATE_HPP_

/// pre-processor macro for concatenating two strings
#define CFTL_CATENATE0(l, r) l ## r
#define CFTL_CATENATE(left, right) CFTL_CATENATE0(left, right)

#endif /* CFTL_CATENATE_HPP_ */
