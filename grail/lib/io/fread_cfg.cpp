/*
 * fread_cfg.cpp
 *
 *  Created on: Feb 21, 2011
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

#include "grail/include/io/fread_cfg.hpp"

namespace grail { namespace io { namespace cfg {

    /// symbol character
    bool is_symbol_codepoint(const char * const cp) throw() {
        return *cp < 0 || (0 != isalnum(static_cast<int>(*cp))) || '_' == *cp;
    }

    bool is_numeric_codepoint(const char * const cp) throw() {
        return 0 != isdigit(static_cast<int>(*cp));
    }

    uint8_t next_state(uint8_t curr_state, token_type input) throw() {

        // ./fa -mdfa "((N*)((S(N*):(N|S|T|\|)*;)|(S>(S|T)*N))(N*))*E"
        // http://graph.gafol.net/dVZByKvWE

        static uint8_t trans[9][8] = {
             /*  T   S   ->   :   |   ;   \n   EOF */
        /* 0 */ {8,  1,  8,   8,  8,  8,  2,   3},
        /* 1 */ {8,  8,  5,   6,  8,  8,  4,   8},
        /* 2 */ {8,  1,  8,   8,  8,  8,  2,   8},
        /* 3 */ {8,  8,  8,   8,  8,  8,  8,   8},
        /* 4 */ {8,  8,  8,   6,  8,  8,  4,   8},
        /* 5 */ {5,  5,  8,   8,  8,  8,  7,   8},
        /* 6 */ {6,  6,  8,   8,  6,  7,  6,   8},
        /* 7 */ {8,  1,  8,   8,  7,  8,  7,   3},
        /* 8 */ {8,  8,  8,   8,  8,  8,  8,   8},
        };

        return trans[curr_state][input];
    }

}}}
