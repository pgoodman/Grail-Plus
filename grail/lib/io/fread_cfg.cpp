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

}}}
