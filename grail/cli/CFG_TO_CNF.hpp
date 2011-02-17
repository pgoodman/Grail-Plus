/*
 * cfg_to_cnf.hpp
 *
 *  Created on: Feb 16, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_CLI_CFG_TO_CNF_HPP_
#define FLTL_CLI_CFG_TO_CNF_HPP_

#include "grail/include/CommandLineOptions.hpp"

#include "grail/algorithm/CFG_TO_CNF.hpp"

namespace grail { namespace cli {

    template <typename AlphaT>
    class CFG_TO_CNF {
    public:

        static const char * const TOOL_NAME;

        static void declare(CommandLineOptions &) throw() {

        }

        static int main(CommandLineOptions &options) throw() {

            option_type help(options['h']);

            // display help information
            if(help.is_valid()) {

                //  "                                   |                                             |"
                printf(
                    "  %s:\n"
                    "    Converts a context-free grammar (CFG) into Chomsky Normal Form. If the\n"
                    "    CFG generates the empty word then the only epsilon production in the\n"
                    "    CFG will be that of the start variable.\n\n"
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
    const char * const CFG_TO_CNF<AlphaT>::TOOL_NAME("cfg-to-cnf");
}}

#endif /* FLTL_CLI_CFG_TO_CNF_HPP_ */
