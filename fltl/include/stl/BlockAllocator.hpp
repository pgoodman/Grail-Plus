/*
 * BlockAllocator.hpp
 *
 *  Created on: Sep 19, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef FLTL_STL_BLOCKALLOCATOR_HPP_
#define FLTL_STL_BLOCKALLOCATOR_HPP_

#include <cstddef>
#include <cstdlib>
#include <cassert>
#include <memory>
#include <new>
#include <bitset>

#include "fltl/include/trait/StaticOnly.hpp"
#include "fltl/include/trait/Uncopyable.hpp"

namespace fltl { namespace stl {

    /// An allocator that allocates values of type T in chunks of size
    /// block_size. If the block allocator is unable to allocate a value
    /// of type T then the program will exit.
    template <typename T, const unsigned block_size>
    class BlockAllocator : private trait::Uncopyable {
    private:

        /// wrapped value type.
        class Element {
        public:
            T value;
            Element *next;

            /// default constructor, initializes the next free element to
            /// the address of this element plus one.
            Element(void) : next(this + 1) { }
        };

        /// block of elements
        class Block {
        public:
            Element slots[block_size];

            Block *next;

            /// main constructor, chain this into the block list and fix up
            /// the last free slot to point to NULL.
            /// be zero.
            Block(Block *next_block = 0)
             : next(next_block) {
                slots[block_size - 1].next = 0;
            }
        };

        /// free list
        Element *free_list;

        /// list of blocks
        Block *block_list;

        /// the offset of the value field in the Element class.
        static const size_t offset_in_element = offsetof(Element, value);

    public:

        /// default constructor, allocates nothing
        BlockAllocator(void) throw()
         : free_list(0)
         , block_list(0) { }

        /// destructor, frees all of the allocated objects
        virtual ~BlockAllocator(void) {
            free_list = 0;

            // free the blocks
            for(Block *next(0); 0 != block_list; block_list = next) {
                next = block_list->next;
                delete block_list;
            }
        }

        /// allocate one or more objects. takes from the free list or
        /// allocates a new block.
        T *allocate(const size_t num_objects = 1, const void * = 0)
        throw() {
            assert(1 == num_objects);

            T *value(0);

            if(0 == num_objects) {
                return value;
            }

            // we need to allocate because there are not enough contiguous
            // slots
            if(0 == free_list) {
                try {
                    block_list = new Block(block_list);
                    free_list = &(block_list->slots[0]);
                } catch(std::bad_alloc) {
                    exit(EXIT_FAILURE);
                }
            }

            value = new(&(free_list->value)) T;
            free_list = value->next;

            return value;
        }

        /// de-allocate some memory; puts the de-allocated object at the
        /// head of the free list.
        void deallocate(T *memory, const size_t num_objects = 1) throw() {
            assert(1 == num_objects);

            Element *elm(static_cast<Element *>(
                static_cast<char *>(memory) - offset_in_element)
            );

            elm->value.~T();
            elm->next = free_list;
            free_list = elm;
        }
    };

    /// Allows for the use of the block allocator as a standard C++ allocator
    /// when passed as a template template parameter that expects only one
    /// type parameter.
    template <const unsigned block_size>
    class Block : private trait::StaticOnly {
    public:

        template <typename T>
        class Allocator : public BlockAllocator<T, block_size> {
        public:

            Allocator(void) { }
            virtual ~Allocator(void) { }
        };

    };
}}

#endif /* FLTL_STL_BLOCKALLOCATOR_HPP_ */
