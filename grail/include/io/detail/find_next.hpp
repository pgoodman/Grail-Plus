/*
 * find_next.hpp
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

#ifndef FLTL_FIND_NEXT_HPP_
#define FLTL_FIND_NEXT_HPP_

#include "grail/include/io/UTF8FileBuffer.hpp"

namespace grail { namespace io { namespace detail {

    /// look for a string in the input stream; assumes what we're looking
    /// for has at least one non-null character in it. this also assumes
    /// that we're looking for ASCII, and so we can ignore the rest of
    /// the codepoint
    template <const unsigned BUFFER_SIZE, const bool LOOK_FOR_ERRORS>
    static bool find_next(
        UTF8FileBuffer<BUFFER_SIZE> &buffer,
        const char *close
    ) throw() {
        char ch('\0');
        const char *curr(close);

        for(;;) {
            ch = *(buffer.read());
            if(LOOK_FOR_ERRORS && '\0' == ch) {
                return false;
            } else if(*curr != ch) {
                curr = close;
            } else {
                if('\0' == *++curr) {
                    return true;
                }
            }
        }
        return false;
    }

}}}

#endif /* FLTL_FIND_NEXT_HPP_ */
