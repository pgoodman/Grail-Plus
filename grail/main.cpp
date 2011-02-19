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

#include "grail/include/CommandLineOptions.hpp"
#include "grail/include/CStringMap.hpp"

#include "grail/cli/CFG_TO_CNF.hpp"
#include "grail/cli/CFG_REMOVE_UNITS.hpp"
#include "grail/cli/CFG_REMOVE_EPSILON.hpp"

namespace {

    enum {
        GRAIL_MAJOR_VERSION = 0,
        GRAIL_MINOR_VERSION = 2
    };

    static void help_header(const char *argv0) throw() {
        printf(
        //  "  | |                              |                                             |"
            "usage: %s [options] [input]\n\n"
            "  tool-selection option:\n"
            "    --tool=<name>                  use the Grail+ tool named <name>\n\n"
            "  basic use options for all Grail+ tools:\n"
            "    --help, -h                     show this message, along with any tool-\n"
            "                                   specific help\n"
            "    --test                         execute all test cases\n"
            "    --version                      show the version\n"
            "    --tools                        list all installed tools\n\n",
            argv0
        );
    }

    static void help_footer(void) throw() {
        printf(
        //  "                                   |                                             |"
            "  Grail+ is Copyright (C) 2011, Peter Goodman.\n\n"
            "  Bug reports, feedback, etc. to: peter.goodman@gmail.com\n\n"
        );
    }

    typedef int (cli_tool_type)(grail::CommandLineOptions &);
    typedef void (cli_decl_type)(grail::CommandLineOptions &, bool);
    typedef void (cli_help_type)(void);
    typedef char alphabet_type;

    // tool information
    struct ToolMeta {
    public:
        ToolMeta *next;
        cli_tool_type *tool_func;
        cli_decl_type *decl_func;
        cli_help_type *help_func;
        char const * const *name;
    };

    /// map holding the tools
    static ToolMeta *first_tool(0);
    static ToolMeta *last_tool(0);
    static grail::CStringMap<ToolMeta *> tools;

    // chain together command-line tools
    struct ToolAdder {
    public:
        ToolAdder(ToolMeta *meta) throw() {
            if(0 == last_tool) {
                first_tool = meta;
                last_tool = meta;
            } else {
                last_tool->next = meta;
                last_tool = meta;
            }
        }
    };

#ifdef GRAIL_DECLARE_TOOL
#undef GRAIL_DECLARE_TOOL
#endif

#define GRAIL_DECLARE_TOOL(tpl) \
    static ToolMeta cli__ ## tpl ## __meta = { \
        0, \
        grail::cli::tpl<alphabet_type>::main, \
        grail::cli::tpl<alphabet_type>::declare, \
        grail::cli::tpl<alphabet_type>::help, \
        &(grail::cli::tpl<alphabet_type>::TOOL_NAME) \
    }; \
    static ToolAdder cli__ ## tpl (&(cli__ ## tpl ## __meta));

#include "grail/Tools.hpp"

}

/// run the program
int main(const int argc, const char **argv) throw() {

    using namespace grail;

    // in the tools; this needs to be done *after* static initialization
    // because we are not guaranteed that the tool names will be initialized
    // by the time ToolAdder constructors are called.
    for(ToolMeta *meta(first_tool); 0 != meta; meta = meta->next) {
        tools.get(*(meta->name)) = meta;
    }

    CommandLineOptions options(argc, argv);

    if(options.parse()) {

        option_type help(options.declare("help", 'h', opt::OPTIONAL, opt::NO_VAL));
        option_type tool(options.declare("tool", opt::OPTIONAL, opt::REQUIRES_VAL));
        option_type list_tools(options.declare("tools", opt::OPTIONAL, opt::NO_VAL));
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

        // list out all installed tools
        } else if(list_tools.is_valid()) {
            printf(
                "The following tools are installed and can be accessed using the\n"
                "tool-selection option (--tool). To view tool-specific help, select\n"
                "a tool using (--tool) and use the help option (--help).\n\n"
            );
            for(ToolMeta *meta(first_tool); 0 != meta; meta = meta->next) {
                printf("  %s\n", *(meta->name));
            }
            printf("\n");

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

            ToolMeta *tool_meta(tools.get(tool.raw_value()));

            // can we dispatch to a tool?
            if(0 == tool_meta) {
                options.error("Unrecognized tool.");
                options.note("Tool was specified here.", tool);
                return 1;
            }

            // declare tool-specific command-line arguments
            tool_meta->decl_func(options, help.is_valid());

            // found an error through the tool's declarations
            if(options.has_error()) {
                return 1;
            }

            if(help.is_valid()) {
                help_header(argv[0]);
                tool_meta->help_func();
                help_footer();
            } else {
                return tool_meta->tool_func(options);
            }
        }

        return 0;
    }

    return 1;
}

#endif /* FLTL_MAIN_CPP_ */
