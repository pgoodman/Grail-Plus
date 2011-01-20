/*
 * SizeOf.hpp
 *
 *  Created on: Sep 15, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef FLTL_MPL_SIZEOF_HPP_
#define FLTL_MPL_SIZEOF_HPP_

#include "fltl/include/mpl/If.hpp"
#include "fltl/include/mpl/Unit.hpp"

namespace fltl { namespace mpl {

    namespace detail {
        class sizeof_centinel { };
    }

    /// Get the byte size for a particular type. this allows us to say that
    /// specific types have zero size, which happens to be useful for other
    /// mpl classes.
    template <typename T>
    class SizeOf {
    public:
        enum {
            VALUE = sizeof(T)
        };
    };

    template <typename T>
    class SizeOf<const T> {
    public:
        enum {
            VALUE = SizeOf<T>::VALUE
        };
    };

    template <>
    class SizeOf<void> {
    public:
        enum {
            VALUE = 0
        };
    };

    // set the size of the Unit type, assuming it's distinct from void.
    template <>
    class SizeOf<IfEqual<void,Unit,detail::sizeof_centinel,Unit>::type> {
    public:
        enum {
            VALUE = 0
        };
    };
}}

#endif /* FLTL_MPL_SIZEOF_HPP_ */
