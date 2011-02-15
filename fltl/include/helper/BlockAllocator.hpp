/*
 * BlockAllocator.hpp
 *
 *  Created on: Jan 3, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_LISTALLOCATOR_HPP_
#define FLTL_LISTALLOCATOR_HPP_

#include <cassert>
#include <new>

#include "fltl/include/helper/UnsafeCast.hpp"

#include "fltl/include/trait/Uncopyable.hpp"

#define FLTL_USE_BLOCK_ALLOCATOR 1

namespace fltl { namespace helper {

    namespace detail {

        template <typename T>
        struct BlockAllocatorSlot {
        public:

            typedef BlockAllocatorSlot<T> self_type;

            T obj;
            self_type *next;

            BlockAllocatorSlot(void)
                : obj()
                , next(0)
            { }

            ~BlockAllocatorSlot() { }
        };

        template <typename T, const unsigned NUM_SLOTS>
        struct BlockAllocatorBlock {
        public:

            typedef BlockAllocatorBlock<T,NUM_SLOTS> self_type;
            typedef BlockAllocatorSlot<T> slot_type;

            self_type *next;
            slot_type slots[NUM_SLOTS];

            BlockAllocatorBlock(self_type *_next) throw()
                : next(_next)
                , slots()
            {
                slot_type *last(&(slots[NUM_SLOTS - 1]));
                for(slot_type *curr(&(slots[0])); curr <= last; ++curr) {
                    curr->next = curr + 1;
                }
                last->next = 0;
            }

            BlockAllocatorBlock(const self_type &) throw()
                : next(0)
                , slots()
            {
                assert(false);
            }

            ~BlockAllocatorBlock(void) throw() { }

            self_type &operator=(const self_type &) throw() {
                assert(false);
                return *this;
            }
        };
    }

    /// note: - destructors of parameterized type are only called if all
    ///         allocated objects are deallocated!
    template <typename T, const unsigned BLOCK_SIZE=256U>
    class BlockAllocator : private trait::Uncopyable {
    private:

        typedef detail::BlockAllocatorSlot<T> slot_type;
        typedef detail::BlockAllocatorBlock<T, BLOCK_SIZE> block_type;
        typedef BlockAllocator<T,BLOCK_SIZE> self_type;

        slot_type *free_list;
        block_type *block_list;

    public:

        BlockAllocator(void) throw()
            : free_list(0)
            , block_list(0)
        { }

        BlockAllocator(const self_type &) throw()
            : free_list(0)
            , block_list(0)
        {
            assert(false);
        }

        ~BlockAllocator(void) throw() {
            for(block_type *curr(block_list), *next(0); 0 != curr; curr = next) {
                next = curr->next;
                delete curr;
            }

            free_list = 0;
            block_list = 0;
        }

        self_type &operator=(const self_type &) throw() {
            assert(false);
            return *this;
        }

        inline T *allocate(void) throw() {
#if FLTL_USE_BLOCK_ALLOCATOR
            if(0 == free_list) {
                block_list = new block_type(block_list);
                free_list = &(block_list->slots[0]);
            }

            slot_type *obj(free_list);
            free_list = obj->next;

            return &(obj->obj);
#else
            return new T;
#endif
        }

        inline void deallocate(T *ptr) throw() {
#if FLTL_USE_BLOCK_ALLOCATOR
            // destroy and re-instantiate
            ptr->~T();
            new (ptr) T;

            const ptrdiff_t diff(
                reinterpret_cast<char *>(&(block_list->slots[0].obj)) -
                reinterpret_cast<char *>(&(block_list->slots[0]))
            );

            slot_type *new_head(0);

            if(diff > 0) {
                new_head = helper::unsafe_cast<slot_type *>(
                    reinterpret_cast<char *>(ptr) - diff
                );
            } else {
                new_head = helper::unsafe_cast<slot_type *>(ptr);
            }

            new_head->next = free_list;
            free_list = new_head;
#else
            delete ptr;
#endif
        }
    };

}}

#endif /* FLTL_LISTALLOCATOR_HPP_ */
