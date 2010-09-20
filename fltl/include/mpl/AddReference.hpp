/*
 * AddReference.hpp
 *
 *  Created on: Sep 15, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef CFTL_ADDREFERENCE_HPP_
#define CFTL_ADDREFERENCE_HPP_

#include "src/include/trait/StaticOnly.hpp"

namespace cftl { namespace mpl {

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

#endif /* CFTL_ADDREFERENCE_HPP_ */
