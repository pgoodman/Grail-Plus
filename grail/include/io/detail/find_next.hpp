/*
 * find_next.hpp
 *
 *  Created on: Mar 1, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
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
