/*
 * Table.hpp
 *
 *  Created on: Mar 4, 2011
 *      Author: Peter Goodman
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

#ifndef FLTL_TABLE_HPP_
#define FLTL_TABLE_HPP_

#include <cassert>

namespace fltl { namespace helper {

    template <typename T>
    class Table {
    private:
        const unsigned num_rows;
        const unsigned num_cols;
        T *slots;

        typedef Table<T> self_type;

        self_type &operator=(const self_type &) throw() {
            assert(false);
            return *this;
        }

    public:

        Table(const unsigned nr, const unsigned nc) throw()
            : num_rows(nr)
            , num_cols(nc)
            , slots(new T[nr * nc])
        { }

        ~Table(void) throw() {
            if(0 != slots) {
                delete [] slots;
                slots = 0;
            }
        }

        T &operator()(const unsigned row, const unsigned col) throw() {
            assert(row < num_rows);
            assert(col < num_cols);
            return slots[(row * num_rows) + col];
        }
    };

}}

#endif /* FLTL_TABLE_HPP_ */
