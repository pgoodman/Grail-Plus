/*
 * SizeOf.hpp
 *
 *  Created on: Sep 15, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef FLTL_MPL_SIZEOF_HPP_
#define FLTL_MPL_SIZEOF_HPP_

#include "fltl/include/mpl/Unit.hpp"

namespace fltl { namespace mpl {

    namespace {
        class centinel { };

        template <typename T0, typename T1>
        class SizeOfTypeEq {
        public:
            typedef T0 type_t;
        };

        template <typename T0>
        class SizeOfTypeEq<T0, T0> {
        public:
            typedef centinel type_t;
        };
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
    class SizeOf<SizeOfTypeEq<Unit,void>::type_t> {
    public:
        enum {
            VALUE = 0
        };
    };
}}

#endif /* FLTL_MPL_SIZEOF_HPP_ */
