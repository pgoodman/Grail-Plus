/*
 * AddReference.hpp
 *
 *  Created on: Sep 15, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef FLTL_ADDREFERENCE_HPP_
#define FLTL_ADDREFERENCE_HPP_

#include "fltl/include/trait/StaticOnly.hpp"

namespace fltl { namespace mpl {

    template <typename T>
    class AddReference : private trait::StaticOnly {
    public:
        typedef T &type_t;
    };

    template <typename T>
    class AddReference<T &> : private trait::StaticOnly {
    public:
        typedef T &type_t;
    };
}}

#endif /* FLTL_ADDREFERENCE_HPP_ */
