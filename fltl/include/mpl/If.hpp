/*
 * If.hpp
 *
 *  Created on: Jan 17, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
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
