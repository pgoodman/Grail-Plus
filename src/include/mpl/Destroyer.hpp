/*
 * Destroyer.hpp
 *
 *  Created on: Sep 15, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef CFTL_DESTROYER_HPP_
#define CFTL_DESTROYER_HPP_

namespace cftl {

    /// Call the destructor of an object through a pointer.
    template <typename T>
    class Destroyer {
    public:
        static void destroy(T *memory) {
            memory->~T();
        }
        static void destroy(void *memory) {
            reinterpret_cast<T *>(memory)->~T();
        }
    };
}

#endif /* CFTL_DESTROYER_HPP_ */
