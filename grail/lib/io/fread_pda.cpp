/*
 * fread_pda.cpp
 *
 *  Created on: Mar 3, 2012
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

#include "grail/include/io/fread_pda.hpp"

namespace grail { namespace io { namespace pda {

    /// symbol character
    bool is_symbol_codepoint(const char * const cp) throw() {
        return *cp < 0 || isalnum(*cp) || '_' == *cp;
    }

    bool is_numeric_codepoint(const char * const cp) throw() {
        return isdigit(*cp);
    }

    uint8_t next_state(uint8_t curr_state, token_type input) throw() {

        // ./fa -mdfa "((N*)(((S>Z)|(Z<F)|(Z(\"|I)Z(,(\"|I)?/(\"|I)?)?)))N)*(N*)E"

        static uint8_t trans[13][11] = {
             /*
                 (START) |-  (FINAL)  -|  /   ,   \n          Z   \0
                 S       >   F        <   /   ,   N   "   I   Z   E
             */
        /* 0 */ {2,      12, 12,      12, 12, 12, 0,  12, 12, 1,  3},
        /* 1 */ {12,     12, 12,      7, 12, 12, 12,  6,  6, 12,  12},
        /* 2 */ {12,     4,  12,      12, 12, 12, 12, 12, 12, 12, 12},
        /* 3 */ {12,     12, 12,      12, 12, 12, 12, 12, 12, 12, 12}, // final
        /* 4 */ {12,     12, 12,      12, 12, 12, 12, 12, 12, 5,  12},
        /* 5 */ {12,     12, 12,      12, 12, 12, 0,  12, 12, 12, 12},
        /* 6 */ {12,     12, 12,      12, 12, 12, 12, 12, 12, 8,  12},
        /* 7 */ {12,     12, 5,       12, 12, 12, 12, 12, 12, 12, 12},
        /* 8 */ {12,     12, 12,      12, 12, 9,  0,  12, 12, 12, 12},
        /* 9 */ {12,     12, 12,      12, 11, 12, 12, 10, 10, 12, 12},
        /* 10 */{12,     12, 12,      12, 11, 12, 12, 12, 12, 12, 12},
        /* 11 */{12,     12, 12,      12, 12, 12, 12, 5,  5,  12, 12},
        /* 12 */{12,     12, 12,      12, 12, 12, 12, 12, 12, 12, 12},
        };

        return trans[curr_state][input];
    }

}}}
