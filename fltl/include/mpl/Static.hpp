/*
 * Static.hpp
 *
 *  Created on: Jan 13, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef FLTL_STATIC_HPP_
#define FLTL_STATIC_HPP_

#include <new>

namespace fltl { namespace mpl {

    namespace detail {

        template <typename T>
        class StaticValue {
        public:
            inline static T &default_value(void) throw() {
                static T val;
                static bool is_initialized(false);
                if(!is_initialized) {
                    is_initialized = true;
                    new (&val) T;
                }
                return val;
            }
        };

        template <typename T>
        class StaticValue<T *> {
        public:
            inline static T *&default_value(void) throw() {
                static T *val(0);
                return val;
            }
        };

        template <>
        class StaticValue<char> {
        public:
            inline static char &default_value(void) throw() {
                static char val('\0');
                return val;
            }
        };

        template <>
        class StaticValue<unsigned char> {
        public:
            inline static unsigned char &default_value(void) throw() {
                static unsigned char val(0U);
                return val;
            }
        };

        template <>
        class StaticValue<short> {
        public:
            inline static short &default_value(void) throw() {
                static short val(0);
                return val;
            }
        };

        template <>
        class StaticValue<unsigned short> {
        public:
            inline static unsigned short &default_value(void) throw() {
                static unsigned short val(0U);
                return val;
            }
        };

        template <>
        class StaticValue<int> {
        public:
            inline static int &default_value(void) throw() {
                static int val(0);
                return val;
            }
        };

        template <>
        class StaticValue<unsigned> {
        public:
            inline static unsigned &default_value(void) throw() {
                static unsigned val(0U);
                return val;
            }
        };

        template <>
        class StaticValue<long> {
        public:
            inline static long &default_value(void) throw() {
                static long val(0L);
                return val;
            }
        };

        template <>
        class StaticValue<unsigned long> {
        public:
            inline static unsigned long &default_value(void) throw() {
                static unsigned long val(0UL);
                return val;
            }
        };

        template <>
        class StaticValue<float> {
        public:
            inline static float &default_value(void) throw() {
                static float val(0.0);
                return val;
            }
        };

        template <>
        class StaticValue<double> {
        public:
            inline static double &default_value(void) throw() {
                static double val(0.0);
                return val;
            }
        };
    }

    /// produces a value of a given type
    template <typename T>
    class Static {
    public:
        static const T &VALUE;
    };

    template <typename T>
    const T &Static<T>::VALUE(detail::StaticValue<T>::default_value());
}}

#endif /* FLTL_STATIC_HPP_ */
