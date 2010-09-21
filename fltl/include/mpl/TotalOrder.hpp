/*
 * TotalOrder.hpp
 *
 *  Created on: Sep 14, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef FLTL_MPL_TOTALORDER_HPP_
#define FLTL_MPL_TOTALORDER_HPP_

#include "fltl/include/trait/OperatorTag.hpp";

namespace fltl { namespace mpl {

    /// Less-than-or-equal operator for some type. Defines a total ordering
    /// for elements of that type.
    template <typename T>
    class TotalOrder {
    public:

        typedef trait::BinaryOperatorTag operator_tag_t;

        bool operator()(const T &left, const T &right) const {
            return left <= right;
        }
    };

    template <typename T>
    class TotalOrder<T &> {
    public:

        typedef trait::BinaryOperatorTag operator_tag_t;

        bool operator()(const T &left, const T &right) const {
            return left <= right;
        }
    };
}}

#endif /* FLTL_MPL_TOTALORDER_HPP_ */
