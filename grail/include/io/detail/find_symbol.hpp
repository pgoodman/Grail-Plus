/*
 * find_symbol.hpp
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

#ifndef FLTL_FIND_SYMBOL_HPP_
#define FLTL_FIND_SYMBOL_HPP_

#include "grail/include/io/UTF8FileBuffer.hpp"

namespace grail { namespace io { namespace detail {

    typedef enum {
        SYMBOL_ACCEPT,
        SYMBOL_FAIL,
        SYMBOL_TOO_LONG
    } symbol_state_type;

    /// try to fill up scratch with a symbol
    template <const unsigned BUFFER_SIZE, const bool LOOK_FOR_ERRORS>
    symbol_state_type find_symbol(
        UTF8FileBuffer<BUFFER_SIZE> &buffer,
        char *scratch,
        const char * const scratch_end,
        bool (*predicate)(const char *)
    ) throw() {
        (void) scratch_end;

        const char *codepoint(0);
        for(;;) {
            codepoint = buffer.read();
            if(predicate(codepoint)) {
                strcpy(scratch, codepoint);
                scratch += buffer.byte_length();

                if(LOOK_FOR_ERRORS && scratch >= scratch_end) {
                    return SYMBOL_TOO_LONG;
                }
            } else {
                buffer.unread();
                *scratch = '\0';
                return SYMBOL_ACCEPT;
            }
        }

        return SYMBOL_FAIL;
    }

}}}

#endif /* FLTL_FIND_SYMBOL_HPP_ */
