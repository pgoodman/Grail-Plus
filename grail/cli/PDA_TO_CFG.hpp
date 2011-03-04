/*
 * PDA_TO_CFG.hpp
 *
 *  Created on: Mar 3, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_CLI_PDA_TO_CFG_HPP_
#define FLTL_CLI_PDA_TO_CFG_HPP_

#include <cstdio>

#include "grail/include/CommandLineOptions.hpp"

#include "grail/algorithm/PDA_TO_CFG.hpp"

#include "grail/include/io/fread_pda.hpp"
#include "grail/include/io/fprint_pda.hpp"
#include "grail/include/io/fprint_cfg.hpp"

namespace grail { namespace cli {

    template <typename AlphaT>
    class PDA_TO_CFG {
    public:

        static const char * const TOOL_NAME;

        static void declare(CommandLineOptions &opt, bool in_help) throw() {
            if(!in_help) {
                opt.declare_min_num_positional(1);
                opt.declare_max_num_positional(1);
            }
        }

        static void help(void) throw() {
            //  "  | |                              |                                             |"
            printf(
                "  %s:\n"
                "    Converts a non-deterministic pushdown automaton (PDA) into a context-free\n"
                "    grammar (CFG).\n\n"
                "  basic use options for %s:\n"
                "    <file>                         read in a CFG from <file>. If \"stdin\"\n"
                "                                   is given then input will be read from the\n"
                "                                   terminal. Typing a new line, followed by\n"
                "                                   Control-D (^D) will close stdin.\n\n",
                TOOL_NAME, TOOL_NAME
            );
        }

        static int main(CommandLineOptions &options) throw() {

            using fltl::CFG;
            using fltl::PDA;

            // run the tool
            option_type file(options[0U]);
            const char *file_name(file.value());

            FILE *fp(0);

            if(0 == strncmp("stdin", file_name, 6)) {
                fp = stdin;
            } else {
                fp = fopen(file_name, "r");
            }

            if(0 == fp) {

                options.error(
                    "Unable to open file containing pushdown-automaton "
                    "for reading."
                );
                options.note("File specified here:", file);

                return 1;
            }

            CFG<AlphaT> cfg;
            PDA<AlphaT> pda;

            if(!io::fread(fp, pda, file_name)) {
                return 1;
            }

            algorithm::PDA_TO_CFG<AlphaT>::run(pda, cfg);

            fclose(fp);

            //io::fprint(stdout, cfg);
            io::fprint(stdout, pda);

            return 1;
        }
    };

    template <typename AlphaT>
    const char * const PDA_TO_CFG<AlphaT>::TOOL_NAME("pda-to-cfg");
}}

#endif /* FLTL_CLI_PDA_TO_CFG_HPP_ */
