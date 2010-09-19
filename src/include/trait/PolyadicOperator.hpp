/*
 * PolyadicOperator.hpp
 *
 *  Created on: Sep 17, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef CFTL_TRAIT_OPERATORTAG_HPP_
#define CFTL_TRAIT_OPERATORTAG_HPP_

namespace cftl { namespace trait {

    template <const unsigned arity>
    class PolyadicOperator {
    public:
        enum {
            ARITY = arity
        };
    };
}}

#endif /* CFTL_TRAIT_OPERATORTAG_HPP_ */
