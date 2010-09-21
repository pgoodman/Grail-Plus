/*
 * PolyadicOperator.hpp
 *
 *  Created on: Sep 17, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef FLTL_TRAIT_OPERATORTAG_HPP_
#define FLTL_TRAIT_OPERATORTAG_HPP_

namespace fltl { namespace trait {

    template <const unsigned arity>
    class PolyadicOperator {
    public:
        enum {
            ARITY = arity
        };
    };
}}

#endif /* FLTL_TRAIT_OPERATORTAG_HPP_ */
