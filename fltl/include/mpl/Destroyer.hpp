/*
 * Destroyer.hpp
 *
 *  Created on: Sep 15, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef FLTL_DESTROYER_HPP_
#define FLTL_DESTROYER_HPP_

#include "fltl/include/trait/StaticOnly.hpp"

namespace fltl { namespace mpl {

    /// Call the destructor of an object through a pointer.
    template <typename T>
    class Destroyer : private trait::StaticOnly {
    public:
        inline static void destroy(T *memory) {
            memory->~T();
        }
        inline static void destroy(void *memory) {
            reinterpret_cast<T *>(memory)->~T();
        }
    };
}}

#endif /* FLTL_DESTROYER_HPP_ */
