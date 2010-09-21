/*
 * Initialize.hpp
 *
 *  Created on: Sep 15, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef FLTL_MPL_INITIALIZER_HPP_
#define FLTL_MPL_INITIALIZER_HPP_

#include <cstring>
#include <new>

#include "fltl/include/trait/StaticOnly.hpp"

namespace fltl { namespace mpl {

    /// Default initializes a value of the parameterized type, and then
    /// copies the default initialized value to the memory pointed to by the
    /// function parameter.
    ///
    /// Note: - the original default initialized value is likely destroyed
    ///         when the function returns and so the destructors of types that
    ///         are initialized by this function should be side-effect-free.
    template <typename T>
    class Initializer : private trait::StaticOnly {
    public:
        inline static T *initialize(void *memory) {
            /*T default_value;
            memcpy(
                memory,
                reinterpret_cast<void *>(&default_value),
                sizeof(T)
            );*/
            memset(memory, 0, sizeof(T));
            return new (memory) T;
        }
    };
}}

#endif /* FLTL_MPL_INITIALIZER_HPP_ */
