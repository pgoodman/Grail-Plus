/*
 * Align.hpp
 *
 *  Created on: Jan 23, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_ALIGN_HPP_
#define FLTL_ALIGN_HPP_

#include <stdint.h>

#include "fltl/include/preprocessor/FORCE_INLINE.hpp"

namespace fltl { namespace helper {

    template <const uintptr_t align_to, typename T>
    FLTL_FORCE_INLINE T *align(T *ptr) throw() {
        return reinterpret_cast<T *>(
            (reinterpret_cast<uintptr_t>(ptr) + (align_to - 1)) & ~(align_to - 1)
        );
    }
  
}}

#endif /* FLTL_ALIGN_HPP_ */
