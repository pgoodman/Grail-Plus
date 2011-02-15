/*
 * CommandLineOptions.cpp
 *
 *  Created on: Feb 15, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_COMMANDLINEOPTIONS_CPP_
#define FLTL_COMMANDLINEOPTIONS_CPP_

#include "grail/include/CommandLineOptions.hpp"

namespace grail {

    /// constructor
    CommandLineOptions::CommandLineOptions(
        const int argc_,
        const char **argv_
    ) throw()
        : argc(argc_)
        , argv(argv_)
    { }

    /// parse the basic structure of command-line options
    bool CommandLineOptions::parse(void) throw() {

        for(int i(1); i < argc; ++i) {

            // keyword argument
            if('-' == argv[i][0]) {

                // long option
                if('-' == argv[i][1]) {

                // short option
                } else if(isalpha(argv[i][1])){

                // error option
                } else {
                    return error(
                        "Unexpected character found. Expected either an "
                        "alphabetical character (for short option) or "
                        "another '-' (for long option).",
                        argv,
                        i,
                        1UL
                    );
                }

            // positional argument
            } else {

            }
        }

        return true;
    }

    /// report an error with the command-line arguments. this writes errors
    /// out in a Clang-like way, i.e. it tries to highlight and pinpoint the
    /// context of an error
    bool CommandLineOptions::error(
        const char *error,
        const char **argv,
        const int argv_offset,
        const size_t err_offset
    ) throw() {

        enum {
            BUFF_LEN = 128,
            ERROR_CONTEXT = 25
        };

        char error_buffer[BUFF_LEN];
        memset(error_buffer, 0, 81UL * sizeof(char));

        assert(0 <= argv_offset);

        const size_t len(strlen(argv[argv_offset]));
        const size_t diff(len - err_offset);

        size_t context(ERROR_CONTEXT);

        // there is enough context in the current arg cell for this error
        if(ERROR_CONTEXT < err_offset) {
            memcpy(
                error_buffer,
                &(argv[argv_offset][err_offset - context]),
                (diff > ERROR_CONTEXT ? ERROR_CONTEXT : diff) * sizeof(char)
            );
        // we can't get any context from previous arguments or we already have
        // enough context
        } else if(argv_offset == 0 || err_offset >= ERROR_CONTEXT) {
            context = (err_offset > ERROR_CONTEXT ? ERROR_CONTEXT : err_offset);
            memcpy(
                error_buffer,
                &(argv[argv_offset][err_offset - context]),
                (diff > ERROR_CONTEXT ? ERROR_CONTEXT : diff) * sizeof(char)
            );
        // we need to look to previous argv cells for context
        } else {

            context = err_offset;
            size_t begin_offset(0);
            size_t prev_context(0);
            int i(argv_offset);
            for(; i >= 0; --i) {
                const size_t cell_len(strlen(argv[i]));

                prev_context = context;
                context += cell_len + 1; // +1 for space between cells

                if(context > ERROR_CONTEXT) {
                    begin_offset = cell_len - (ERROR_CONTEXT - prev_context);
                    break;
                } else if(context == ERROR_CONTEXT) {
                    break;
                }
            }

            context = 0;
            for(++i; i < argv_offset; ++i, begin_offset = 0) {

                const size_t cell_len(strlen(&(argv[i][begin_offset])));
                memcpy(
                    &(error_buffer[context]),
                    &(argv[i][begin_offset]),
                    cell_len
                );

                context += cell_len + 1;
                error_buffer[context - 1] = ' ';
            }

            memcpy(
                &(error_buffer[context]),
                argv[argv_offset],
                (len > ERROR_CONTEXT ? ERROR_CONTEXT : len) * sizeof(char)
            );
        }

        printf(FLTL_F_DEF);
        printf(
            FLTL_F_BOLD FLTL_F_RED "error:" FLTL_F_DEF
            FLTL_F_BOLD " %s" FLTL_F_DEF "\n",
            error
        );

        if(0 != context && '\0' != error_buffer[0]) {
            printf("    %s\n    " FLTL_F_GREEN , error_buffer);
            for(size_t i(0); i < context; ++i) {
                printf("~");
            }
            printf("~^" FLTL_F_DEF "\n\n");
        } else {
            printf("\n");
        }

        return false;
    }
}

#endif /* FLTL_COMMANDLINEOPTIONS_CPP_ */
