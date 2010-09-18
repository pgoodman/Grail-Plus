/*
 * OperatorTag.hpp
 *
 *  Created on: Sep 17, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef CFTL_TRAIT_OPERATORTAG_HPP_
#define CFTL_TRAIT_OPERATORTAG_HPP_

namespace cftl { namespace trait {

    class OperatorTag { };

    class UnaryOperatorTag : public OperatorTag { };

    class BinaryOperatorTag : public OperatorTag { };
}}

#endif /* CFTL_TRAIT_OPERATORTAG_HPP_ */
