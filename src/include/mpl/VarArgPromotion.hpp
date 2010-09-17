/*
 * VarArgPromotion.hpp
 *
 *  Created on: Sep 16, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef CFTL_MPL_VARARGPROMOTION_HPP_
#define CFTL_MPL_VARARGPROMOTION_HPP_

#include <cstdarg>

namespace cftl {

    /// define type promotions for the variadic functions
    template <typename T>
    class VarArgPromotion {
    public:
        typedef T type_t;
    };

    /// const-correct promotions, lets us define promotions in terms of
    /// non-const types
    template <typename T>
    class VarArgPromotion<const T> {
    public:
        typedef const typename VarArgPromotion<T>::type_t type_t;
    };

    template <>
    class VarArgPromotion<char> {
    public:
        typedef int type_t;
    };

    template <>
    class VarArgPromotion<unsigned char> {
    public:
        typedef unsigned type_t;
    };
}

#endif /* CFTL_MPL_VARARGPROMOTION_HPP_ */
