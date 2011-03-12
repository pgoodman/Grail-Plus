/*
 * CFG_PARSE.hpp
 *
 *  Created on: Mar 10, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_CFG_PARSE_HPP_
#define FLTL_CFG_PARSE_HPP_

#include <set>
#include <vector>

#include "fltl/include/CFG.hpp"

#include "fltl/include/helper/Array.hpp"

#include "grail/include/io/CommandLineOptions.hpp"
#include "grail/include/io/fread_cfg.hpp"

#include "grail/include/cfg/compute_null_set.hpp"

#include "grail/include/algorithm/CFG_PARSE_EARLEY.hpp"

namespace grail { namespace cli {

    template <typename AlphaT>
    class CFG_PARSE {
    public:

        static const char * const TOOL_NAME;

        static void declare(io::CommandLineOptions &opt, bool in_help) throw() {
            if(!in_help) {
                opt.declare_min_num_positional(1);
                opt.declare_max_num_positional(1);
            }
        }

        static void help(void) throw() {
            //  "  | |                              |                                             |"
            printf(
                "  %s:\n"
                "    Parses a token stream according to a context-free grammar (CFG).\n\n"
                "  basic use options for %s:\n"
                "    <file>                         read in a CFG from <file>.\n\n",
                TOOL_NAME, TOOL_NAME
            );
        }

        static int main(io::CommandLineOptions &options) throw() {

            using fltl::CFG;

            // run the tool
            io::option_type file;
            const char *file_name(0);

            FILE *fp(0);

            file = options[0U];
            file_name = file.value();
            fp = fopen(file_name, "r");

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

            printf("reading...\n");
            if(io::fread(fp, cfg, file_name)) {

                std::vector<bool> is_nullable;

                // fill the first and nullable sets
                printf("computing null set...\n");
                cfg::compute_null_set(cfg, is_nullable);
                printf("parsing...\n");
                algorithm::CFG_PARSE_EARLEY<AlphaT>::run(
                    cfg,
                    is_nullable
                );

            } else {
                ret = 1;
            }

            fclose(fp);

            return ret;
        }
    };

    template <typename AlphaT>
    const char * const CFG_PARSE<AlphaT>::TOOL_NAME("cfg-parse");

}}

#endif /* FLTL_CFG_PARSE_HPP_ */
