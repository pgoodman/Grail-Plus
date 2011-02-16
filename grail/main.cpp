/*
 * main.cpp
 *
 *  Created on: Feb 7, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_MAIN_CPP_
#define FLTL_MAIN_CPP_

#include <string>

#include "fltl/test/Test.hpp"
#include "fltl/test/cfg/CFG.hpp"

#include "grail/algorithm/CFG_REMOVE_EPSILON.hpp"
#include "grail/algorithm/CFG_REMOVE_UNITS.hpp"
#include "grail/algorithm/CFG_TO_CNF.hpp"

#include "grail/include/CommandLineOptions.hpp"

enum {
    GRAIL_MAJOR_VERSION = 0,
    GRAIL_MINOR_VERSION = 2
};

static void help_header(const char *argv0) throw() {
    printf(
        "usage: %s [options] [input]\n\n"
        "  tool-selection option:\n"
        "    --tool=<name>                  use the Grail+ tool named <name>\n\n"
        "  basic use options for all Grail+ tools:\n"
        "    --help, -h                     show this message, along with any tool-specific\n"
        "                                   help\n"
        "    --test                         execute all test cases\n"
        "    --version                      show the version\n\n",
        argv0
    );
}

static void help_footer(void) throw() {
    printf(
        "  Grail+ is Copyright (C) 2011, Peter Goodman.\n\n"
        "  Bug reports, feedback, etc. to: peter.goodman@gmail.com\n\n"
    );
}

int main(const int argc, const char **argv) throw() {

    using namespace grail;
    typedef char alphabet_type;

    CommandLineOptions options(argc, argv);

    if(options.parse()) {

        option_type help(options.declare("help", 'h', opt::OPTIONAL, opt::NO_VAL));
        option_type tool(options.declare("tool", opt::OPTIONAL, opt::REQUIRES_VAL));
        option_type test(options.declare("test", opt::OPTIONAL, opt::NO_VAL));
        option_type version(options.declare("version", opt::OPTIONAL, opt::NO_VAL));

        if(options.has_error()) {
            return 1;
        }

        // show the current version
        if(version.is_valid()) {

            printf(
                "Grail+, version %u.%u, Copyright (C) 2011, Peter Goodman.\n",
                GRAIL_MAJOR_VERSION, GRAIL_MINOR_VERSION
            );

        // run through all test cases
        } else if(test.is_valid()) {

            fltl::test::run_tests();

        // no tool, no help
        } else if(!help.is_valid() && !tool.is_valid()) {
            options.error(
                "At least one of '--tool', '--help', or '-h' must be "
                "specified. Use '--help' or '-h' to see available "
                "command-line options."
            );
            return 1;

        // show top-level help
        } else if(!tool.is_valid() && help.is_valid()) {

            help_header(argv[0]);
            help_footer();

        // delegate to the tool
        } else if(tool.is_valid()) {

            if(help.is_valid()) {
                help_header(argv[0]);
            }



            if(help.is_valid()) {
                help_footer();
            }
        }

        return 0;
    }

    return 1;
}

#endif /* FLTL_MAIN_CPP_ */
