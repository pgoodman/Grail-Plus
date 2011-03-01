/*
 * CFG_REMOVE_EPSILON.hpp
 *
 *  Created on: Feb 16, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_CLI_CFG_REMOVE_EPSILON_HPP_
#define FLTL_CLI_CFG_REMOVE_EPSILON_HPP_

#include "grail/include/CommandLineOptions.hpp"

#include "grail/algorithm/CFG_REMOVE_EPSILON.hpp"

namespace grail { namespace cli {

    template <typename AlphaT>
    class CFG_REMOVE_EPSILON {
    public:

        static const char * const TOOL_NAME;

        static void declare(CommandLineOptions &opt, bool in_help) throw() {
            (void) opt;
            (void) in_help;
        }

        static void help(void) throw() {
            //  "  | |                              |                                             |"
            printf(
                "  %s:\n"
                "    Remove epsilon productions from a context-free grammar (CFG). If the CFG\n"
                "    generates the empty string then the only remaining epsilon will be related\n"
                "    to the start variable.\n\n"
                "  basic use options for %s:\n"
                "    <file>                         read in a CFG from <file>.\n\n",

                TOOL_NAME, TOOL_NAME
            );
        }

        static int main(CommandLineOptions &options) throw() {
            (void) options;
            // TODO
            return 0;
        }
    };

    template <typename AlphaT>
    const char * const CFG_REMOVE_EPSILON<AlphaT>::TOOL_NAME("cfg-remove-epsilon");
}}


#endif /* FLTL_CLI_CFG_REMOVE_EPSILON_HPP_ */
