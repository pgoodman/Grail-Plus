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

#include <cstdio>

#include "grail/include/CommandLineOptions.hpp"

#include "grail/algorithm/CFG_TO_CNF.hpp"

#include "grail/include/io/fread_cfg.hpp"
#include "grail/include/io/fprint_cfg.hpp"

namespace grail { namespace cli {

    template <typename AlphaT>
    class CFG_TO_CNF {
    public:

        static const char * const TOOL_NAME;

        static void declare(CommandLineOptions &opt, bool in_help) throw() {
            option_type in(opt.declare("stdin", opt::OPTIONAL, opt::NO_VAL));
            if(!in_help) {
                if(in.is_valid()) {
                    opt.declare_max_num_positional(0);
                } else {
                    opt.declare_min_num_positional(1);
                    opt.declare_max_num_positional(1);
                }
            }
        }

        static void help(void) throw() {
            //  "  | |                              |                                             |"
            printf(
                "  %s:\n"
                "    Converts a context-free grammar (CFG) into Chomsky Normal Form. If the\n"
                "    CFG generates the empty string then the only epsilon production in the\n"
                "    CFG will be that of the start variable.\n\n"
                "  basic use options for %s:\n"
                "    --stdin                        Read a CFG from stdin. Typing a new,\n"
                "                                   line followed by Control-D (^D) will\n"
                "                                   close stdin.\n"
                "    <file>                         read in a CFG from <file>.\n\n",
                TOOL_NAME, TOOL_NAME
            );
        }

        static int main(CommandLineOptions &options) throw() {

            using fltl::CFG;

            // run the tool
            option_type file;
            const char *file_name(0);

            FILE *fp(0);

            if(options["stdin"].is_valid()) {
                file = options["stdin"];
                fp = stdin;
                file_name = "<stdin>";
            } else {
                file = options[0U];
                file_name = file.value();
                fp = fopen(file_name, "r");
            }

            if(0 == fp) {

                options.error(
                    "Unable to open file containing context-free "
                    "grammar for reading."
                );
                options.note("File specified here:", file);

                return 1;
            }

            CFG<AlphaT> cfg;

            if(!io::fread(fp, cfg, file_name)) {
                return 1;
            }

            /*
            printf("BEFORE:\n");
            printf("num variables = %u\n", cfg.num_variables());
            printf("num terminals = %u\n", cfg.num_terminals());
            printf("num productions = %u\n", cfg.num_productions());
            printf("num variable terminals = %u\n", cfg.num_variable_terminals());
            */

            algorithm::CFG_TO_CNF<AlphaT>::run(cfg);

            /*
            printf("\nAFTER:\n");
            printf("num variables = %u\n", cfg.num_variables());
            printf("num terminals = %u\n", cfg.num_terminals());
            printf("num productions = %u\n", cfg.num_productions());
            printf("num variable terminals = %u\n", cfg.num_variable_terminals());
            */
            io::fprint(stdout, cfg);

            fclose(fp);

            return 0;
        }
    };

    template <typename AlphaT>
    const char * const CFG_TO_CNF<AlphaT>::TOOL_NAME("cfg-to-cnf");
}}

#endif /* FLTL_CLI_CFG_TO_CNF_HPP_ */
