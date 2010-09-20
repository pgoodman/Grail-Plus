/*
 * Destroyer.hpp
 *
 *  Created on: Sep 15, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef CFTL_DESTROYER_HPP_
#define CFTL_DESTROYER_HPP_

#include "src/include/trait/StaticOnly.hpp"

namespace cftl { namespace mpl {

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

#endif /* CFTL_DESTROYER_HPP_ */
