/*
 * fread_nfa.cpp
 *
 *  Created on: Mar 8, 2011
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

#include "grail/include/io/fread_nfa.hpp"

namespace grail { namespace io { namespace nfa {

    uint8_t next_state(uint8_t curr_state, pda::token_type input) throw() {
        // ./fa -mdfa "(N*(S>Z|Z<F|Z(\"|I)Z))*(N*)E"

        static uint8_t trans[13][11] = {
             /*
                 (START) |-  (FINAL)  -|  /   ,   \n          Z   \0
                 S       >   F        <   /   ,   N   "   I   Z   E
             */
        /* 0 */ {2,      6,  6,       6,  6,  6,  0,  6,  6,  1,   3},
        /* 1 */ {6,      6,  6,       5,  6,  6,  6,  4,  4,  6,   6},
        /* 2 */ {6,      4,  6,       6,  6,  6,  6,  6,  6,  6,   6},
        /* 3 */ {6,      6,  6,       6,  6,  6,  6,  6,  6,  6,   6}, // final
        /* 4 */ {6,      6,  6,       6,  6,  6,  6,  6,  6,  0,   6},
        /* 5 */ {6,      6,  0,       6,  6,  6,  6,  6,  6,  6,   6},
        /* 6 */ {6,      6,  6,       6,  6,  6,  6,  6,  6,  6,   6},
        };

        return trans[curr_state][input];
    }

}}}
