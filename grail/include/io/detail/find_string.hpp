/*
 * find_string.hpp
 *
 *  Created on: Mar 1, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_FIND_STRING_HPP_
#define FLTL_FIND_STRING_HPP_

#include "grail/include/io/UTF8FileBuffer.hpp"

namespace grail { namespace io { namespace detail {

    typedef enum {
        STRING_TOO_LONG,
        STRING_EARLY_EOF,
        STRING_UNKNOWN_ESCAPE,
        STRING_ACCEPT,
        STRING_FAIL
    } string_state_type;

    /// fill up the buffer with a token
    template <const unsigned BUFFER_SIZE, const bool LOOK_FOR_ERRORS>
    static string_state_type find_string(
        UTF8FileBuffer<BUFFER_SIZE> &buffer,
        char *scratch,
        const char * const scratch_end,
        const char delim
    ) throw() {

        // we have already read in delim, go look for it and collect what we
        // find into scratch
        unsigned num_delims(1);
        unsigned num_delims_seen(0);
        const char *codepoint(0);
        char *mark(0);
        char ch('\0');

        // try to figure out how many delimiter characters we are using
        for(;;) {
            codepoint = buffer.read();
            ch = *codepoint;

            if(delim == ch) {
                ++num_delims;
            } else {
                goto handle_next_char;
            }
        }

        for(;;) {
        load_next_char:
            codepoint = buffer.read();
            ch = *codepoint;

        handle_next_char:

            if(LOOK_FOR_ERRORS && scratch >= scratch_end) {
                return STRING_TOO_LONG;
            }

            // looks like an escape sequence
            if('\\' == ch) {
                codepoint = buffer.read();
                ch = *codepoint;

                if(LOOK_FOR_ERRORS && '\0' == ch) {
                    return STRING_EARLY_EOF;
                }

                switch(ch) {
                case 's': *scratch++ = ' '; break;
                case 't': *scratch++ = '\t'; break;
                case 'n': *scratch++ = '\n'; break;
                case 'r': *scratch++ = '\r'; break;
                case '\\': *scratch++ = '\\'; break;
                case '"': *scratch++ = '"'; break;
                case '\'': *scratch++ = '\''; break;

                default:
                    if(LOOK_FOR_ERRORS) {
                        return STRING_UNKNOWN_ESCAPE;
                    }
                }

            // found a delimiter, might be the end of the string, or might
            // be an embedding of the delimiter in the string assuming
            // num_delims > 1
            } else if(delim == ch) {
                mark = scratch;
                *scratch++ = ch;
                num_delims_seen = 1;

                for(; num_delims_seen < num_delims; ) {
                    codepoint = buffer.read();
                    strcpy(scratch, codepoint);
                    scratch += buffer.byte_length();

                    if(LOOK_FOR_ERRORS) {
                        if(scratch >= scratch_end) {
                            return STRING_TOO_LONG;
                        } else if('\0' == *codepoint) {
                            return STRING_EARLY_EOF;
                        }
                    }

                    if(*codepoint == delim) {
                        ++num_delims_seen;
                    } else {
                        goto load_next_char;
                    }
                }

                mark[scratch - mark - num_delims] = '\0';
                return STRING_ACCEPT;

            // error, we got to the end of the file before the end of the
            // string
            } else if(LOOK_FOR_ERRORS && '\0') {
                return STRING_EARLY_EOF;

            // copy the codepoint into scratch
            } else {
                strcpy(scratch, codepoint);
                scratch += buffer.byte_length();
            }
        }

        // shouldn't even get down here
        return STRING_FAIL;
    }

}}}

#endif /* FLTL_FIND_STRING_HPP_ */
