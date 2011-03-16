/*
 * If.hpp
 *
 *  Created on: Jan 17, 2011
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

#ifndef FLTL_IF_HPP_
#define FLTL_IF_HPP_

namespace fltl { namespace mpl {

    template <const bool cond, typename ThenT, typename ElseT>
    class IfTrue {
    public:
        typedef ElseT type;
    };

    template <typename ThenT, typename ElseT>
    class IfTrue<true,ThenT,ElseT> {
    public:
        typedef ThenT type;
    };

    template <typename T0, typename T2, typename ThenT, typename ElseT>
    class IfEqual {
    public:
        typedef ElseT type;
    };

    template <typename T0, typename ThenT, typename ElseT>
    class IfEqual<T0,T0,ThenT,ElseT> {
    public:
        typedef ThenT type;
    };

    template <const int val, typename ThenT, typename ElseT>
    class IfZero {
    public:
        typedef ElseT type;
    };

    template <typename ThenT, typename ElseT>
    class IfZero<0,ThenT,ElseT> {
    public:
        typedef ThenT type;
    };

    template <const unsigned val, typename ThenT, typename ElseT>
    class IfUZero {
    public:
        typedef ElseT type;
    };

    template <typename ThenT, typename ElseT>
    class IfUZero<0U,ThenT,ElseT> {
    public:
        typedef ThenT type;
    };

    template <typename T0, typename T1>
    class IfTypesEqual {
    public:
        enum {
            RESULT = 0
        };
    };

    template <typename T0, typename T1>
    class IfTypesEqual<const T0, const T1> {
    public:
        enum {
            RESULT = IfTypesEqual<T0,T1>::RESULT
        };
    };

    template <typename T0, typename T1>
    class IfTypesEqual<const T0, T1> {
    public:
        enum {
            RESULT = IfTypesEqual<T0,T1>::RESULT
        };
    };

    template <typename T0, typename T1>
    class IfTypesEqual<T0, const T1> {
    public:
        enum {
            RESULT = IfTypesEqual<T0,T1>::RESULT
        };
    };

    template <typename T0>
    class IfTypesEqual<T0,T0> {
    public:
        enum {
            RESULT = 1
        };
    };
}}

#endif /* FLTL_IF_HPP_ */
