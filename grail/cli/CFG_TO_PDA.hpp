/*
 * CFG_TO_PDA.hpp
 *
 *  Created on: Feb 27, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_CLI_CFG_TO_PDA_HPP_
#define FLTL_CLI_CFG_TO_PDA_HPP_

#include <cstdio>

#include "grail/include/CommandLineOptions.hpp"

#include "grail/algorithm/CFG_TO_PDA.hpp"

#include "grail/include/io/fread_cfg.hpp"
#include "grail/include/io/fprint_pda.hpp"

namespace grail { namespace cli {

    template <typename AlphaT>
    class CFG_TO_PDA {
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
                "    Converts a context-free grammar (CFG) into a Non-deterministic Pushdown\n"
                "    Automaton (PDA).\n\n"
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
            using fltl::PDA;

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
            PDA<AlphaT> pda;
            int ret(0);

            if(io::fread(fp, cfg, file_name)) {
                if(0 != cfg.num_variable_terminals()) {
                    options.error(
                        "There is at least one variable terminal "
                        "in the grammar file."
                    );
                    options.note("File specified here:", file);
                    ret = 1;
                } else {
                    algorithm::CFG_TO_PDA<AlphaT>::run(cfg, pda);
                    io::fprint(stdout, pda);
                }
            } else {
                ret = 1;
            }

            fclose(fp);

            return ret;
        }
    };

    template <typename AlphaT>
    const char * const CFG_TO_PDA<AlphaT>::TOOL_NAME("cfg-to-pda");
}}

#endif /* FLTL_CLI_CFG_TO_PDA_HPP_ */
