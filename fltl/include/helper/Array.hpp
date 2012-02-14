/*
 * Array.h
 *
 *  Created on: Jun 6, 2010
 *      Author: petergoodman
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

#ifndef PAG_ARRAY_H_
#define PAG_ARRAY_H_

#include <cassert>

#include "fltl/include/mpl/AddReference.hpp"

#include "fltl/include/preprocessor/NO_INLINE.hpp"

#include "fltl/include/trait/Uncopyable.hpp"

namespace fltl { namespace helper {

    /// base type representing some contiguous memory and those operations
    /// on that memory
    ///
    /// this class imposes the following restrictions on parameterized
    /// types:
    ///     - they must have an assignment operator defined
    ///     - they must have a zero-argument constructor defined, or a
    ///       constructor with all arguments having default value_types.
    ///
    template <typename T>
    class Array : private trait::Uncopyable {
    public:

        typedef typename mpl::AddReference<T>::type reference_type;
        typedef typename mpl::AddReference<T>::const_type const_reference_type;
        typedef T *pointer_type;
        typedef T value_type;

    private:

        pointer_type slots;
        unsigned num_slots;
        mutable unsigned num_used_slots;

    public:

        Array(void) throw()
         : slots(0)
         , num_slots(0)
         , num_used_slots(0) { }

        /// basic constructor
        Array(const unsigned len) throw()
         : slots(0)
         , num_slots(0)
         , num_used_slots(0) {
            reserve(len, false);
        }

        /// copy constructor
        Array(const Array<T> &other) throw()
         : slots(0)
         , num_slots(0)
         , num_used_slots(0) {
            extend(other);
        }

        ~Array(void) throw() {
            if(0 != slots) {
                delete [] slots;
                slots = 0;
            }
            num_slots = 0;
            num_used_slots = 0;
        }

        Array<T> &operator=(const Array<T> &) const throw() {
            assert(false);
            return *this;
        }

        /// access an element in a slot of the Array
        inline reference_type get(const unsigned offset) const throw() {
            assert(offset < num_slots && "Out-of-bounds get() to Array.");
            assert(0 != slots && "Array slots not allocated.");

            if(offset > num_used_slots) {
                num_used_slots = offset + 1;
            }

            return slots[offset];
        }

        inline reference_type operator[](const unsigned offset) throw() {
            return get(offset);
        }

        /// get the element at the back
        inline reference_type back(void) throw() {
            return get(num_used_slots - 1);
        }

        /// update an element in a slot
        inline void set(const unsigned offset, const_reference_type item) throw() {
            assert(offset < num_used_slots && "Out-of-bounds set() to Array.");
            assert(0 != slots && "Array slots not allocated.");

            slots[offset] = item;
        }

        /// swap two elements in the Array
        inline void swap(const unsigned i, const unsigned j) throw() {
            assert(0 != slots && "Array slots not allocated.");
            assert(i < num_used_slots && "Out-of-bounds swap() to Array.");
            assert(j < num_used_slots && "Out-of-bounds swap() to Array.");

            reference_type old_i(slots[i]);
            slots[i] = slots[j];
            slots[j] = old_i;
        }

        /// get the length of the Array
        inline unsigned size(void) const throw() {
            return num_used_slots;
        }

        /// extend this Array with another Array
        void extend(const Array<T> &other) throw() {

            if(0 == other.num_used_slots) {
                return;
            }

            const unsigned old_len(num_used_slots);
            reserve(old_len + other.num_used_slots, true);
            set_size(old_len + other.num_used_slots);

            for(unsigned i(0); i < other.num_used_slots; ++i) {
                slots[i + old_len] = other.slots[i];
            }
        }

        /// append an item to this Array
        void append(const_reference_type item) throw() {
            const unsigned old_len(num_used_slots);
            reserve(old_len + 1, true);
            set_size(old_len + 1);
            slots[old_len] = item;
        }

        inline reference_type pop(void) throw() {
            assert(0 < num_used_slots && "Cannot pop() from empty Array.");
            return slots[--num_used_slots];
        }

        /// reverse the order of items in this Array
        void reverse(void) throw() {
            const unsigned mid = num_used_slots / 2;
            for (unsigned j(0); j < mid; ++j) {
                swap(j, num_used_slots - j - 1);
            }
        }

        /// clear out the Array
        inline void clear(void) throw() {
            num_used_slots = 0;
            delete [] slots;
            slots = 0;
        }

        /// remove an element from the Array and slide all elements that
        /// follow it back into its position
        void remove(const unsigned i) throw() {
            // is j in range?
            if (i >= num_used_slots) {
                return;
            }

            // remove T at index j
            --num_used_slots;
            for (unsigned j(i) ; j < num_used_slots; ++j) {
                slots[j] = slots[j + 1];
            }
        }

        /// guarantee that this Array can scale up to a minimum length
        FLTL_NO_INLINE void reserve(
            const unsigned min_slots,
            const bool copy_old=true
        ) throw() {

            if(num_slots >= min_slots) {
                return;
            }

            const unsigned old_num_slots(num_slots);
            unsigned new_num_slots(num_slots);

            if(!(new_num_slots & 1)) {
                ++new_num_slots;
            }

            for(; new_num_slots < min_slots; new_num_slots *= 2) {
                // do do la la de do
            }
            
            pointer_type new_slots(new value_type[new_num_slots]);

            assert(0 != new_slots && "Unable to reserve new space for Array.");

            if(0 != slots) {
                if(copy_old) {
                    for(unsigned i(0); i < old_num_slots; ++i) {
                        new_slots[i] = slots[i];
                    }
                }

                delete [] slots;
            }

            num_slots = new_num_slots;
            slots = new_slots;
        }

        /// change the current length of the Array
        inline void set_size(const unsigned new_len) throw() {
            assert(new_len <= num_slots && "Can't grow Array with set_size().");
            num_used_slots = new_len;
        }

        inline bool is_empty(void) const throw() {
            return 0 == num_used_slots;
        }
    };
}}

#endif /* PAG_ARRAY_H_ */
