/*
 * Or.hpp
 *
 *  Created on: Mar 3, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_OR_HPP_
#define FLTL_OR_HPP_

namespace fltl { namespace mpl {

    template <typename T0, typename T1=void, typename T2=void>
    class Or;

    template <typename T0>
    class Or<T0,void,void> {
    public:
        enum {
            RESULT = T0::RESULT
        };
    };

    template <typename T0, typename T1>
    class Or<T0,T1,void> {
    public:
        enum {
            RESULT = (T0::RESULT || T1::RESULT)
        };
    };

    template <typename T0, typename T1, typename T2>
    class Or {
    public:
        enum {
            RESULT = (T0::RESULT || T1::RESULT || T2::RESULT)
        };
    };
}}

#endif /* FLTL_OR_HPP_ */
