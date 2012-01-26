/*
 * error.cpp
 *
 *  Created on: Feb 20, 2011
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

#include "grail/include/io/error.hpp"

namespace grail { namespace io {

    enum {
        APPROX_LINE_SPLIT_POINT = 75
    };

    static void send_string(
        FILE *fp,
        const char *str,
        const char *pad,
        size_t pad_size,
        size_t *col
    ) {

        if(0 == str || '\0' == *str) {
            return;
        }

        size_t i(*col);
        for(; *str != '\0'; ++str, ++i) {
            if(' ' == *str) {
                if(i >= APPROX_LINE_SPLIT_POINT) {
                    fprintf(fp, "\n%s", pad);
                    i = pad_size;
                } else {
                    fprintf(fp, " ");
                }
            } else if('\n' == *str) {
                fprintf(fp, "\\n");
                i += 2;
            } else if('\t' == *str) {
                fprintf(fp, "\\t");
                i += 2;
            } else if('\r' == *str) {
                fprintf(fp, "\\r");
                i += 2;
            } else if('\b' == *str) {
                fprintf(fp, "\\b");
                i += 2;
            } else if('\v' == *str) {
                fprintf(fp, "\\v");
                i += 2;
            } else if('\a' == *str) {
                fprintf(fp, "\\a");
                i += 2;
            } else if('\f' == *str) {
                fprintf(fp, "\\f");
                i += 2;
            } else {
                fprintf(fp, "%c", *str);
            }
        }
        *col = i;
    }

    static void message(
        FILE *fp,
        const char *color,
        const char *type,
        const char *str,
        va_list args,
        const size_t start_col
    ) {

        char pad[64];
        char buff[129];
        char *curr_buff(&(buff[0]));
        char *end_buff(curr_buff + 128);

        memset(pad, 0, 64 * sizeof(char));
        memset(buff, 0, 129 * sizeof(char));

        fprintf(fp,
            FLTL_F_DEF FLTL_F_BOLD "%s%s: " FLTL_F_DEF,
            color, type
        );

        size_t pad_size(strlen(type) + 2);
        size_t col(pad_size + start_col);

        memset(pad, ' ', pad_size * sizeof(char));

        for(; '\0' != *str; ) {
            for(; curr_buff < end_buff && '\0' != *str; ++str, ++curr_buff) {
                ++col;
                if('%' == *str) {
                    ++str;

                    if('%' == *str) {
                        *curr_buff = *str;

                    } else {

                        // flush the buffer
                        *curr_buff = '\0';
                        fprintf(fp, "%s", buff);
                        curr_buff = &(buff[0]) - 1;

                        // string argument
                        if('s' == *str) {
                            send_string(
                                fp,
                                va_arg(args, const char *),
                                pad,
                                pad_size,
                                &col
                            );

                        // integer argument
                        } else if('d' == *str) {
                            col += fprintf(fp, "%d", va_arg(args, int));

                        // unsigned integer argument
                        } else if('u' == *str) {
                            col += fprintf(fp, "%u", va_arg(args, unsigned));

                        // character
                        } else if('c' == *str) {
                            col += fprintf(fp, "%c", static_cast<char>(va_arg(args, int)));
                        }
                    }
                } else if('\n' == *str) {
                    *curr_buff = '\0';
                    fprintf(fp, "%s\n%s", buff, pad);
                    curr_buff = &(buff[0]) - 1;
                    col = pad_size;

                } else if(' ' == *str) {
                    if(col >= APPROX_LINE_SPLIT_POINT) {
                        *curr_buff = '\0';
                        fprintf(fp, "%s\n%s", buff, pad);
                        curr_buff = &(buff[0]) - 1;
                        col = pad_size;
                    } else {
                        *curr_buff = ' ';
                    }
                } else {
                    *curr_buff = *str;
                }
            }

            // flush the buffer when it's full
            *curr_buff = '\0';
            fprintf(fp, "%s", buff);
            curr_buff = &(buff[0]);
        }
        fprintf(fp, "\n\n%s", FLTL_F_DEF);
    }

    void error(
        const char * const file_name,
        const unsigned line,
        const unsigned col,
        const char *str,
        ...
    ) {
        if(0 == str) {
            return;
        }

        int num(fprintf(stderr,
            FLTL_F_DEF FLTL_F_BOLD "%s:%u:%u: " FLTL_F_DEF ,
            file_name, line, col
        ));

        if(num < 0) {
            return;
        }

        va_list args;
        va_start(args, str);
        message(
            stderr,
            FLTL_F_RED, "error",
            str,
            args,
            static_cast<size_t>(num) - strlen(FLTL_F_DEF FLTL_F_BOLD FLTL_F_DEF)
        );
        va_end(args);
    }

    void error(const char *str, ...) {
        if(0 == str) {
            return;
        }
        va_list args;
        va_start(args, str);
        message(stderr, FLTL_F_RED, "error", str, args, 0);
        va_end(args);
    }

    void warning(const char *str, ...) {
        if(0 == str) {
            return;
        }
        va_list args;
        va_start(args, str);
        message(stderr, FLTL_F_PINK, "warning", str, args, 0);
        va_end(args);
    }

}}

