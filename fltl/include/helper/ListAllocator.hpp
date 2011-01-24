/*
 * ListAllocator.hpp
 *
 *  Created on: Jan 3, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_LISTALLOCATOR_HPP_
#define FLTL_LISTALLOCATOR_HPP_

#include "fltl/include/trait/Uncopyable.hpp"
#include "fltl/include/mpl/Static.hpp"

namespace fltl { namespace helper {

    /// allocator for structures that contain a pointer to themselves and so
    /// can be naturally stringed into a linked list.
    template <typename T, T **(*NEXT_PTR)(T *), const unsigned BLOCK_SIZE=256U>
    class ListAllocator : private trait::Uncopyable {
    private:

        class Block {
        public:
            Block *next;
            T slots[BLOCK_SIZE];

            Block(Block *next_block) throw()
                : next(next_block)
            {
                T *last(&(slots[BLOCK_SIZE - 1]));
                for(T *curr(&(slots[0])); curr <= last; ++curr) {
                    *(NEXT_PTR(curr)) = curr + 1;
                }
                *(NEXT_PTR(last)) = 0;
            }

            ~Block(void) throw() { }
        };

        T *free_list;
        Block *block_list;

    public:

        ListAllocator(void) throw()
            : free_list(0)
            , block_list(0)
        { }

        ~ListAllocator(void) throw() {
            for(Block *curr(block_list), *next(0); 0 != curr; curr = next) {
                next = curr->next;
                delete curr;
            }
            free_list = 0;
            block_list = 0;
        }

        inline T *allocate(void) throw() {
            if(0 == free_list) {
                block_list = new Block(block_list);
                free_list = &(block_list->slots[0]);
            }

            T *next(free_list);
            free_list = *(NEXT_PTR(next));
            return next;
        }

        inline void deallocate(T *ptr) throw() {
            *(NEXT_PTR(ptr)) = free_list;
            free_list = ptr;
        }
    };
}}

#endif /* FLTL_LISTALLOCATOR_HPP_ */
