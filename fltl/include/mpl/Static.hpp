/*
 * Static.hpp
 *
 *  Created on: Jan 13, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_STATIC_HPP_
#define FLTL_STATIC_HPP_

namespace fltl { namespace mpl {

    namespace detail {

        template <typename T>
        class StaticValue {
        public:
            inline static T default_value(void) throw() {
                T val;
                return val;
            }
        };

        template <>
        class StaticValue<char> {
        public:
            inline static char default_value(void) throw() {
                return '\0';
            }
        };

        template <>
        class StaticValue<unsigned char> {
        public:
            inline static unsigned char default_value(void) throw() {
                return static_cast<unsigned char>(0U);
            }
        };

        template <>
        class StaticValue<short> {
        public:
            inline static short default_value(void) throw() {
                return 0;
            }
        };

        template <>
        class StaticValue<unsigned short> {
        public:
            inline static unsigned short default_value(void) throw() {
                return 0U;
            }
        };

        template <>
        class StaticValue<int> {
        public:
            inline static int default_value(void) throw() {
                return 0;
            }
        };

        template <>
        class StaticValue<unsigned> {
        public:
            inline static unsigned default_value(void) throw() {
                return 0U;
            }
        };

        template <>
        class StaticValue<long> {
        public:
            inline static long default_value(void) throw() {
                return 0L;
            }
        };

        template <>
        class StaticValue<unsigned long> {
        public:
            inline static unsigned long default_value(void) throw() {
                return 0UL;
            }
        };

        template <>
        class StaticValue<float> {
        public:
            inline static float default_value(void) throw() {
                return 0.0;
            }
        };

        template <>
        class StaticValue<double> {
        public:
            inline static double default_value(void) throw() {
                return 0.0;
            }
        };
    }

    /// produces a value of a given type
    template <typename T>
    class Static {
    public:
        static const T VALUE;
    };

    template <typename T>
    const T Static<T>::VALUE(detail::StaticValue<T>::default_value());
}}

#endif /* FLTL_STATIC_HPP_ */
