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

#include "fltl/include/preprocessor/FORCE_INLINE.hpp"

namespace fltl { namespace helper {
#if 0
    namespace detail {

        template <typename FromT, typename ToT>
        union UnionCast {
        public:
            FromT *from_type;
            ToT *to_type;
        };

        template <const bool, typename FromT, typename ToT>
        class UnsafeCast;

        template <typename FromT, typename ToT>
        class UnsafeCast<true, FromT, ToT> {
        public:

            FLTL_FORCE_INLINE static ToT
            cast(FromT val) throw() {
                UnionCast<FromT,ToT> trans;
                trans.from_type = &val;
                return *(trans.to_type);
            }
        };

        template <typename FromT, typename ToT>
        class UnsafeCast<false, FromT, ToT> {
        public:
            FLTL_FORCE_INLINE static ToT
            cast(FromT val) throw() {
                return *reinterpret_cast<ToT *>(
                    reinterpret_cast<uintptr_t>(&val)
                );
            }
        };
    }
#endif

    template <typename ToT, typename FromT>
    FLTL_FORCE_INLINE ToT unsafe_cast(FromT v) throw() {
        return *reinterpret_cast<ToT *>(
            reinterpret_cast<uintptr_t>(&v)
        );
#if 0
        return detail::UnsafeCast<
            sizeof(detail::UnionCast<FromT,ToT>) == (sizeof(FromT *)),
            FromT,
            ToT
        >::cast(v);
#endif
    }
}}

#endif /* FLTL_PUNNEDCAST_HPP_ */
