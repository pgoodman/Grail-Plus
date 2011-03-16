/*
 * UTF8FileLineBuffer.hpp
 *
 *  Created on: Mar 15, 2011
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

#ifndef FLTL_UTF8FILELINEBUFFER_HPP_
#define FLTL_UTF8FILELINEBUFFER_HPP_

#include <cstring>

#include "grail/include/io/UTF8FileBuffer.hpp"

namespace grail { namespace io {

    template <const unsigned LINE_LENGTH>
    class UTF8FileLineBuffer {
    private:

        enum {
            LINE_BUFF_LEN = LINE_LENGTH + 8U
        };

        UTF8FileBuffer<LINE_LENGTH * 2> buffer;
        char line[LINE_BUFF_LEN];

    public:

        UTF8FileLineBuffer(FILE *ff)
            : buffer(ff)
        {
            memset(line, 0, sizeof(char) * LINE_BUFF_LEN);
        }

        void reset(void) throw() {
            buffer.reset();
            memset(line, 0, sizeof(char) * LINE_BUFF_LEN);
        }

        const char *read(void) throw() {
            const char *codepoint(0);
            line[0] = '\0';

            do {
                codepoint = buffer.read();
            } while('\n' == *codepoint || '\r' == *codepoint);

            // can't read past here
            if('\0' == codepoint) {
                return line;
            }

            char *ptr(&(line[0]));
            const char * const max(&(line[LINE_LENGTH]));

            // fill up the line buffer
            for( ; ptr < max
                && '\0' != *codepoint
                && '\n' != *codepoint
                && '\r' != *codepoint;
                codepoint = buffer.read()) {

                for(const char *cp_ptr(codepoint); '\0' != *cp_ptr; ) {
                    *ptr++ = *cp_ptr++;
                }

                *ptr = '\0';
            }

            return line;
        }
    };

}}

#endif /* FLTL_UTF8FILELINEBUFFER_HPP_ */
