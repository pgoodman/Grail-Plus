/*
 * CFG_TO_GNF.hpp
 *
 *  Created on: Mar 5, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_CLI_CFG_TO_GNF_HPP_
#define FLTL_CLI_CFG_TO_GNF_HPP_

#include <cstdio>

#include "grail/include/CommandLineOptions.hpp"

#include "grail/algorithm/CFG_TO_GNF.hpp"

#include "grail/include/io/fread_cfg.hpp"
#include "grail/include/io/fprint_cfg.hpp"

namespace grail { namespace cli {

    template <typename AlphaT>
    class CFG_TO_GNF {
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
                "    Converts a context-free grammar (CFG) into Greibach Normal Form. If the\n"
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
            int ret(0);

            if(io::fread(fp, cfg, file_name)) {
                algorithm::CFG_TO_GNF<AlphaT>::run(cfg);
                io::fprint(stdout, cfg);
            } else {
                ret = 1;
            }

            fclose(fp);

            return ret;
        }
    };

    template <typename AlphaT>
    const char * const CFG_TO_GNF<AlphaT>::TOOL_NAME("cfg-to-gnf");
}}

#endif /* FLTL_CLI_CFG_TO_GNF_HPP_ */
