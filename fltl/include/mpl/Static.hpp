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

    /// produces a value of a given type
    template <typename T>
    class Static {
    public:
        static const T VALUE;
    };

    template <typename T>
    const T Static<T>::VALUE;

    template <>
    const char Static<char>::VALUE('\0');

    template <>
    const unsigned char Static<unsigned char>::VALUE(0U);

    template <>
    const short Static<short>::VALUE(0);

    template <>
    const unsigned short Static<unsigned short>::VALUE(0U);

    template <>
    const int Static<int>::VALUE(0);

    template <>
    const unsigned Static<unsigned>::VALUE(0);

    template <>
    const long Static<long>::VALUE(0L);

    template <>
    const unsigned long Static<unsigned long>::VALUE(0UL);

    template <>
    const float Static<float>::VALUE(0.0);

    template <>
    const double Static<double>::VALUE(0.0);
}}

#endif /* FLTL_STATIC_HPP_ */
