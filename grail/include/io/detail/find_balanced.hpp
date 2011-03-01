/*
 * find_balanced.hpp
 *
 *  Created on: Mar 1, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_FIND_BALANCED_HPP_
#define FLTL_FIND_BALANCED_HPP_

#include "grail/include/io/UTF8FileBuffer.hpp"

namespace grail { namespace io { namespace detail {

    /// try to find something that needs to be balanced; assumes non-null
    /// ascii
    template <const unsigned BUFFER_SIZE, const bool LOOK_FOR_ERRORS>
    static bool find_balanced(
        UTF8FileBuffer<BUFFER_SIZE> &buffer,
        const char open,
        const char close
    ) throw() {
        char ch('\0');
        unsigned num(1); // called after we've already found the opening

        for(;;) {
            ch = *(buffer.read());
            if(LOOK_FOR_ERRORS && '\0' == ch) {
                return false;
            } else if(open == ch) {
                ++num;
            } else if(close == ch) {
                // done
                if(0 == --num) {
                    return true;
                }
            }
        }
        return false;
    }

}}}

#endif /* FLTL_FIND_BALANCED_HPP_ */
