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

        static void declare(CommandLineOptions &) throw() {

        }

        static int main(CommandLineOptions &options) throw() {
            option_type help(options['h']);

            // display help information
            if(help.is_valid()) {

                //  "  | |                              |                                             |"
                printf(
                    "  %s:\n"
                    "    Remove epsilon productions from a context-free grammar (CFG). If the CFG\n"
                    "    generates the empty string then the only remaining epsilon will be related\n"
                    "    to the start variable.\n\n"
                    "  basic use options for cfg-to-cnf:\n"
                    "    <file>                         read in a CFG from <file>.\n\n",

                    TOOL_NAME
                );

            // run the tool
            } else {

            }

            return 0;
        }
    };

    template <typename AlphaT>
    const char * const CFG_REMOVE_EPSILON<AlphaT>::TOOL_NAME("cfg-remove-epsilon");
}}


#endif /* FLTL_CLI_CFG_REMOVE_EPSILON_HPP_ */
