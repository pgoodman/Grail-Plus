/*
 * fread_cfg.cpp
 *
 *  Created on: Feb 21, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#include "grail/include/io/fread_cfg.hpp"

namespace grail { namespace io { namespace cfg {

    /// symbol character
    bool is_symbol_codepoint(const char * const cp) throw() {
        return *cp < 0 || isalnum(*cp) || '_' == *cp || '$' == *cp;
    }

    uint8_t next_state(uint8_t curr_state, token_type input) throw() {

        // ./fa -mdfa "(N*((SN*:(N*(S|T))*(\|(N*(S|T))*)*N*;)|(S>(S|T)*N)))*E"
        // http://graph.gafol.net/dVZByKvWE

        static uint8_t trans[9][8] = {
             /*  T   S   ->   :   |   ;   \n   EOF */
        /* 0 */ {8,  1,  8,   8,  8,  8,  2,   3},
        /* 1 */ {8,  8,  5,   6,  8,  8,  4,   8},
        /* 2 */ {8,  1,  8,   8,  8,  8,  2,   8},
        /* 3 */ {8,  8,  8,   8,  8,  8,  8,   8},
        /* 4 */ {8,  8,  8,   6,  8,  8,  4,   8},
        /* 5 */ {5,  5,  8,   8,  8,  8,  0,   8},
        /* 6 */ {6,  6,  8,   8,  6,  0,  7,   8},
        /* 7 */ {6,  6,  8,   8,  8,  0,  7,   8},
        /* 8 */ {8,  8,  8,   8,  8,  8,  8,   8},
        };

        return trans[curr_state][input];
    }

}}}
