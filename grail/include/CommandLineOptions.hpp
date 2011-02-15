/*
 * CommandLineOptions.hpp
 *
 *  Created on: Feb 15, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_COMMANDLINEOPTIONS_HPP_
#define FLTL_COMMANDLINEOPTIONS_HPP_

#include <cctype>
#include <cstdio>
#include <cassert>
#include <cstring>

#include "fltl/include/preprocessor/COLOR.hpp"

namespace grail {

    class CommandLineOptions {
    private:

        const int argc;
        const char **argv;

        /// report an error with the command-line arguments. this writes errors
        /// out in a Clang-like way, i.e. it tries to highlight and pinpoint the
        /// context of an error
        bool error(
            const char *error,
            const char **argv,
            const int argv_offset,
            const size_t err_offset
        ) throw();

    public:

        CommandLineOptions(const int argc_, const char **argv_) throw();

        bool parse(void) throw();
    };
}

#endif /* FLTL_COMMANDLINEOPTIONS_HPP_ */
