/*
 * StrictWeakOrder.hpp
 *
 *  Created on: Sep 14, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef FLTL_MPL_STRICTWEAKORDER_HPP_
#define FLTL_MPL_STRICTWEAKORDER_HPP_

#include "fltl/include/trait/OperatorTag.hpp";

namespace fltl { namespace mpl {

    /// Less-than operator for some type. Defines a strict weak ordering
    /// for elements of that type.
    template <typename T>
    class StrictWeakOrder {
    public:

        typedef trait::BinaryOperatorTag operator_tag_t;

        bool operator()(const T &left, const T &right) const {
            return left < right;
        }
    };

    template <typename T>
    class StrictWeakOrder<T &> {
    public:

        typedef trait::BinaryOperatorTag operator_tag_t;

        bool operator()(const T &left, const T &right) const {
            return left < right;
        }
    };
}}

#endif /* FLTL_MPL_STRICTWEAKORDER_HPP_ */
