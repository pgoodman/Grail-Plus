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
#include "grail/include/io/verbose.hpp"

#include "grail/include/cfg/compute_null_set.hpp"
#include "grail/include/cfg/compute_first_set.hpp"

#include "grail/include/algorithm/CFG_PARSE_EARLEY.hpp"

namespace grail { namespace cli {

    template <typename AlphaT>
    class CFG_PARSE {
    public:

        typedef fltl::CFG<AlphaT> CFG;
        typedef typename CFG::terminal_type terminal_type;

        static const char * const TOOL_NAME;

        static void declare(io::CommandLineOptions &opt, bool in_help) throw() {

            opt.declare("first", io::opt::OPTIONAL, io::opt::NO_VAL);

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
                "    --first                        compute the first sets of all\n"
                "                                   variables. This computation can\n"
                "                                   take a long time for larger\n"
                "                                   grammars, but can also speed up\n"
                "                                   parsing.\n"
                "    <file>                         read in a CFG from <file>.\n\n",
                TOOL_NAME, TOOL_NAME
            );
        }

        static int main(io::CommandLineOptions &options) throw() {

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

            CFG cfg;
            int ret(0);

            io::verbose("Reading file '%s'...\n", file_name);
            if(io::fread(fp, cfg, file_name)) {

                std::vector<bool> is_nullable;
                std::vector<std::vector<bool> *> first_terminals;

                // fill the first and nullable sets
                io::verbose("Computing NULL set of variables...\n");
                cfg::compute_null_set(cfg, is_nullable);

                bool use_first_sets(false);
                if(options["first"].is_valid()) {
                    io::verbose("Computing FIRST set of variables...\n");
                    use_first_sets = true;
                    cfg::compute_first_set(cfg, is_nullable, first_terminals);
                }

                io::verbose("Parsing...\n");
                algorithm::CFG_PARSE_EARLEY<AlphaT>::run(
                    cfg,
                    is_nullable,
                    use_first_sets,
                    first_terminals
                );

                // clean out the first set
                for(unsigned i(0); i < first_terminals.size(); ++i) {
                    if(0 != first_terminals[i]) {
                        delete first_terminals[i];
                        first_terminals[i] = 0;
                    }
                }

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
