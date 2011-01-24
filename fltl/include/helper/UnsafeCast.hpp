/*
 * UnsafeCast.hpp
 *
 *  Created on: Jan 23, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_PUNNEDCAST_HPP_
#define FLTL_PUNNEDCAST_HPP_

#include <stdint.h>
#include <cstring>

#include "fltl/include/preprocessor/FORCE_INLINE.hpp"

namespace fltl { namespace helper {

    template <typename ToT, typename FromT>
    FLTL_FORCE_INLINE ToT unsafe_cast(const FromT &v) throw() {
        ToT dest;
        memcpy(&dest, &v, sizeof(ToT));
        return dest;
    }

    template <typename ToT, typename FromT>
    FLTL_FORCE_INLINE ToT unsafe_cast(FromT *v) throw() {
        return unsafe_cast<ToT>(
            reinterpret_cast<uintptr_t>(v)
        );
    }
}}

#endif /* FLTL_PUNNEDCAST_HPP_ */
