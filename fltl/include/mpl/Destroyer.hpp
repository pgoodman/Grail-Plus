/*
 * Destroyer.hpp
 *
 *  Created on: Sep 15, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef FLTL_DESTROYER_HPP_
#define FLTL_DESTROYER_HPP_

namespace fltl { namespace mpl {

    /// Call the destructor of an object through a pointer.
    template <typename T>
    class Destroyer {
    public:
        inline static void destroy(T *memory) {
            memory->~T();
        }
        inline static void destroy(void *memory) {
            reinterpret_cast<T *>(memory)->~T();
        }
    };

    template <>
    class Destroyer<int> {
    public:
        inline static void destroy(int *) { }
        inline static void destroy(void *) { }
    };

    template <>
    class Destroyer<unsigned> {
    public:
        inline static void destroy(unsigned *) { }
        inline static void destroy(void *) { }
    };

    template <>
    class Destroyer<long> {
    public:
        inline static void destroy(long *) { }
        inline static void destroy(void *) { }
    };

    template <>
    class Destroyer<unsigned long> {
    public:
        inline static void destroy(unsigned long *) { }
        inline static void destroy(void *) { }
    };

    template <>
    class Destroyer<short> {
    public:
        inline static void destroy(short *) { }
        inline static void destroy(void *) { }
    };

    template <>
    class Destroyer<unsigned short> {
    public:
        inline static void destroy(unsigned short*) { }
        inline static void destroy(void *) { }
    };

    template <>
    class Destroyer<char> {
    public:
        inline static void destroy(char *) { }
        inline static void destroy(void *) { }
    };

    template <>
    class Destroyer<unsigned char> {
    public:
        inline static void destroy(unsigned char *) { }
        inline static void destroy(void *) { }
    };

    template <>
    class Destroyer<bool> {
    public:
        inline static void destroy(bool *) { }
        inline static void destroy(void *) { }
    };

    template <>
    class Destroyer<float> {
    public:
        inline static void destroy(float *) { }
        inline static void destroy(void *) { }
    };

    template <>
    class Destroyer<double> {
    public:
        inline static void destroy(double *) { }
        inline static void destroy(void *) { }
    };
}}

#endif /* FLTL_DESTROYER_HPP_ */
