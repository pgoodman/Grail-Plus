/*
 * find_symbol.hpp
 *
 *  Created on: Mar 1, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
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
