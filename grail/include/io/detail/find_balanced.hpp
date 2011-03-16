/*
 * find_balanced.hpp
 *
 *  Created on: Mar 1, 2011
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

#ifndef FLTL_FIND_BALANCED_HPP_
#define FLTL_FIND_BALANCED_HPP_

#include "grail/include/io/UTF8FileBuffer.hpp"

namespace grail { namespace io { namespace detail {

    /// try to find something that needs to be balanced; assumes non-null
    /// ascii
    template <const unsigned BUFFER_SIZE, const bool LOOK_FOR_ERRORS>
    static bool find_balanced(
        UTF8FileBuffer<BUFFER_SIZE> &buffer,
        const char open,
        const char close
    ) throw() {
        char ch('\0');
        unsigned num(1); // called after we've already found the opening

        for(;;) {
            ch = *(buffer.read());
            if(LOOK_FOR_ERRORS && '\0' == ch) {
                return false;
            } else if(open == ch) {
                ++num;
            } else if(close == ch) {
                // done
                if(0 == --num) {
                    return true;
                }
            }
        }
        return false;
    }

}}}

#endif /* FLTL_FIND_BALANCED_HPP_ */
