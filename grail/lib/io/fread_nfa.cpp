/*
 * fread_nfa.cpp
 *
 *  Created on: Mar 8, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
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
