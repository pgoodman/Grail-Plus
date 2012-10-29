/*
 * Placeholder.hpp
 *
 *  Created on: May 11, 2012
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef Grail_Plus_FLTL_PLACEHOLDER_HPP_
#define Grail_Plus_FLTL_PLACEHOLDER_HPP_

#include <cstring>
#include <stdint.h>

#include "fltl/include/helper/UnsafeCast.hpp"
#include "fltl/include/trait/AlignmentOf.hpp"

namespace fltl { namespace helper {

    namespace detail {

        template <size_t div, size_t rem, size_t factor>
        class storage;

        /// one byte
        template <>
        class storage<1, 0, 1> {
            uint8_t space;
        };

        /// two bytes
        template <>
        class storage<0, 1, 2> {
            uint8_t space1;
        };

        template <>
        class storage<1, 1, 2> {
            uint16_t space0;
            uint8_t space1;
        };

        template <>
        class storage<1, 0, 2> {
            uint16_t space0;
        };

        /// 4 bytes
        template <size_t rem>
        class storage<1, rem, 4> {
            uint32_t space0;
            storage<rem / 2, rem % 2, 2> space1;
        };

        template <size_t rem>
        class storage<0, rem, 4> {
            storage<rem / 2, rem % 2, 2> space1;
        };

        template <>
        class storage<1, 0, 4> {
            uint32_t space;
        };

        /// 8 bytes
        template <size_t div, size_t rem>
        class storage<div, rem, 8> {
            uint64_t space0[div];
            storage<rem / 4, rem % 4, 4> space1;
        };

        template <size_t rem>
        class storage<0, rem, 8> {
            storage<rem / 4, rem % 4, 4> space1;
        };

        template <size_t div>
        class storage<div, 0, 8> {
            uint64_t space[div];
        };
    }

    /// Placeholder for some data with the right amount of space and
    /// alignment. this is mainly used in the garbage collector so that
    /// the default allocation of a garbage collected value is just empty
    /// space
    template <typename T>
    class Placeholder {
    private:

        enum {
            SIZE_ORIG = sizeof(T)
        };

        typedef detail::storage<SIZE_ORIG / 8, SIZE_ORIG % 8, 8> storage_type;

        enum {
            ALIGNMENT_ORIG = trait::AlignmentOf<T>::VALUE,
            ALIGNMENT_NEW = trait::AlignmentOf<storage_type>::VALUE,
            SIZE_NEW = sizeof(storage_type)
        };

        static_assert(SIZE_NEW == SIZE_ORIG,
            "Placeholder storage type size is not the same as the input type size."
        );

        static_assert(ALIGNMENT_NEW >= ALIGNMENT_ORIG,
            "Alignment of Placeholder storage type is less restrictive than the "
            "alignment of the input type."
        );

        storage_type storage;

    public:

        Placeholder(void) throw() {
            memset(&storage, 0, sizeof storage);
        }

        T *construct(void) throw() {
            return new (unsafe_cast<T *>(&storage)) T;
        }

        template <typename... CTypes>
        T *construct(CTypes&&... cvals) throw() {
            return new (unsafe_cast<T *>(&storage)) T(cvals...);
        }

        void destruct(void) throw() {
            unsafe_cast<T *>(&storage)->~T();
            memset(&storage, 0, sizeof storage);
        }

        T *operator->(void) throw() {
            return unsafe_cast<T *>(&storage);
        }
    };

}}


#endif /* Grail_Plus_FLTL_PLACEHOLDER_HPP_ */
