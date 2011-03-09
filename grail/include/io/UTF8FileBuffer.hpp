/*
 * UTF8FileBuffer.hpp
 *
 *  Created on: Feb 20, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_UTF8WINDOW_HPP_
#define FLTL_UTF8WINDOW_HPP_

#include <cassert>
#include <cstdio>
#include <cstring>
#include <cctype>
#include <stdint.h>

#define UTF8_ACCEPT 0
#define UTF8_REJECT 12

namespace grail { namespace io {

    namespace utf8 {
        uint32_t decode(
            uint32_t* state,
            uint32_t byte
        );
    }

    /// maintains a window into some sort of buffer
    template <const unsigned WINDOW_SIZE_>
    class UTF8FileBuffer {
    private:

        enum {
            // large enough to hold the longest null-terminated UTF-8
            // character
            MAX_CODE_POINT_SIZE = 4,
            SCRATCH_SIZE = MAX_CODE_POINT_SIZE + 1,
            WINDOW_SIZE = WINDOW_SIZE_ < MAX_CODE_POINT_SIZE ? MAX_CODE_POINT_SIZE : WINDOW_SIZE_,
            TAB_SIZE = 4
        };

        /// state for buffer
        char buffer[WINDOW_SIZE + 1];
        char scratch[SCRATCH_SIZE];
        char *curr_offset;
        char *end_offset;
        bool done_reading;
        bool at_end;

        /// state for line and column numbers; this information is helpful
        /// for any tool that needs to report errors at a specific location
        unsigned peek_column[2];
        unsigned peek_line[2];
        unsigned peek;

        /// did we unread? note: we can only unread a single codepoint!
        bool bactracked;

        unsigned length;

        /// file pointer
        FILE *fp;

    public:

        UTF8FileBuffer(FILE *fp_) throw()
            : fp(fp_)
        {
            reset();
        }

        void reset(void) throw() {

            memset(buffer, 0, sizeof(char) * (WINDOW_SIZE + 1));
            memset(scratch, 0, sizeof(char) * SCRATCH_SIZE);
            memset(peek_column, 0, sizeof(unsigned) * 2);
            memset(peek_line, 0, sizeof(unsigned) * 2);

            bactracked = false;
            curr_offset = &(buffer[0]);
            end_offset = &(buffer[0]);
            peek = 0;
            peek_line[0] = 1;
            peek_column[0] = 1;
            length = 0;
            done_reading = false;

            if(0 != fp) {
                if(0 != fseek(fp, 0, SEEK_SET)) {
                    at_end = true;
                } else {
                    at_end = false;
                }
            } else {
                at_end = true;
            }
        }

        /// return the last uft-8 codepoint read
        const char *reread(void) const throw() {
            return &(scratch[0]);
        }

        /// try to unread a codepoint
        void unread(void) throw() {
            assert(!bactracked);

            size_t amount(strlen(scratch));
            peek = 1 - peek;
            bactracked = true;
            curr_offset = curr_offset - amount;

            if(at_end && done_reading) {
                at_end = false;
            }

            assert(curr_offset >= &(buffer[0]));
        }

        /// return the byte length of the most recently read codepoint
        unsigned byte_length(void) const throw() {
            return length;
        }

        /// read out a single utf-8 codepoint
        const char *read(void) throw() {
        get_first_char:

            scratch[0] = '\0';

            if(at_end) {
                return &(scratch[0]);
            }

            // fill the buffer if we need to
            const size_t diff(static_cast<unsigned>(end_offset - curr_offset));
            if(MAX_CODE_POINT_SIZE > diff) {
                if(!done_reading) {

                    // slide
                    if(curr_offset != buffer) {
                        memcpy(buffer, curr_offset, diff);
                    }

                    // fill
                    const size_t desired_amount(WINDOW_SIZE - diff);
                    size_t amount(fread(
                        reinterpret_cast<void *>(&(buffer[diff])),
                        sizeof(char),
                        desired_amount,
                        fp
                    ));

                    curr_offset = &(buffer[0]);
                    end_offset = &(buffer[diff + amount]);

                    // done reading from the file
                    if(desired_amount > amount) {
                        done_reading = (0 != feof(fp));
                        memset(
                            end_offset,
                            0,
                            WINDOW_SIZE - (diff + amount)
                        );
                    }
                }
            }

            // try to read one code point
            unsigned i(0);
            uint32_t state(UTF8_ACCEPT);

            for(; curr_offset < end_offset; ) {

                utf8::decode(
                    &state,
                    static_cast<uint32_t>(
                        static_cast<unsigned char>(*curr_offset)
                    )
                );

                if(UTF8_REJECT == state) {
                    ++curr_offset;
                    goto get_first_char;
                }

                scratch[i] = *curr_offset;
                ++i;
                ++curr_offset;

                if(UTF8_ACCEPT == state) {
                    break;
                }
            }

            length = i;
            scratch[i] = '\0';

            const unsigned prev_peek(peek);
            const char ch(scratch[0]);
            peek = 1 - peek;

            // figure out the *next* line and column numbers, based on the
            // current character
            if(!bactracked) {
                switch(ch) {
                case '\0':
                    peek_line[peek] = peek_line[prev_peek];
                    peek_column[peek] = peek_column[prev_peek];
                    at_end = true;
                    break;
                case ' ':
                    peek_line[peek] = peek_line[prev_peek];
                    peek_column[peek] = 1 + peek_column[prev_peek];
                    break;
                case '\n':
                    peek_line[peek] = 1 + peek_line[prev_peek];
                    peek_column[peek] = 1;
                    break;
                case '\t':
                    peek_line[peek] = peek_line[prev_peek];
                    peek_column[peek] = TAB_SIZE + peek_column[prev_peek];
                    break;
                default:
                    // printable ascii or assume printable utf-8
                    if(isprint(ch) || ch < 0) {
                        peek_line[peek] = peek_line[prev_peek];
                        peek_column[peek] = 1 + peek_column[prev_peek];

                    // non-printable ascii
                    } else {
                        peek_line[peek] = peek_line[prev_peek];
                        peek_column[peek] = peek_column[prev_peek];
                    }
                    break;
                }
            } else {
                bactracked = false;
            }

            return &(scratch[0]);
        }

        /// return the line number of the last codepoint read
        unsigned line(void) const throw() {
            return peek_line[1 - peek];
        }

        /// return the column number of the last codepoint read
        unsigned column(void) const throw() {
            return peek_column[1 - peek];
        }
    };

}}

#endif /* FLTL_UTF8WINDOW_HPP_ */
