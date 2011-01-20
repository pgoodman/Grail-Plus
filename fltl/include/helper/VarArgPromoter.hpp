/*
 * VarArgPromoter.hpp
 *
 *  Created on: Sep 16, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef FLTL_MPL_VARARGPROMOTION_HPP_
#define FLTL_MPL_VARARGPROMOTION_HPP_

#include "fltl/include/mpl/If.hpp"

namespace fltl { namespace helper {

    /// define type promotions for the variadic functions
    template <typename T>
    class VarArgPromoter {
    public:
        typedef typename mpl::IfTrue<
            (sizeof(T) < sizeof(int)),
            int,
            T
        >::type type;
    };

    /// const-correct promotions, lets us define promotions in terms of
    /// non-const types
    template <typename T>
    class VarArgPromoter<const T> {
    public:
        typedef const typename VarArgPromoter<T>::type type;
    };
}}

#endif /* FLTL_MPL_VARARGPROMOTION_HPP_ */
