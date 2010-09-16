/*
 * Initialize.hpp
 *
 *  Created on: Sep 15, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef CFTL_MPL_INITIALIZER_HPP_
#define CFTL_MPL_INITIALIZER_HPP_

#include <cstring>

namespace cftl {

    /// Default initializes a value of the parameterized type, and then
    /// copies the default initialized value to the memory pointed to by the
    /// function parameter.
    ///
    /// Note: - the original default initialized value is likely destroyed
    ///         when the function returns and so the destructors of types that
    ///         are initialized by this function should be side-effect-free.
    template <typename T>
    class Initializer {
    public:
        static void initialize(void *memory) {
            T default_value;
            memcpy(
                memory,
                reinterpret_cast<void *>(&default_value),
                sizeof(T)
            );
        }
    };
}

#endif /* CFTL_MPL_INITIALIZER_HPP_ */
