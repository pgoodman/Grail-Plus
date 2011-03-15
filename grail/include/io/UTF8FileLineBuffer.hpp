/*
 * UTF8FileLineBuffer.hpp
 *
 *  Created on: Mar 15, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
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
