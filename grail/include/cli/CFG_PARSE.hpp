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
#include <cstring>

#include "fltl/include/CFG.hpp"

#include "fltl/include/helper/Array.hpp"

#include "grail/include/io/CommandLineOptions.hpp"
#include "grail/include/io/fread_cfg.hpp"
#include "grail/include/io/fprint_parse_tree.hpp"
#include "grail/include/io/verbose.hpp"
#include "grail/include/io/UTF8FileLineBuffer.hpp"

#include "grail/include/cfg/compute_null_set.hpp"
#include "grail/include/cfg/compute_first_set.hpp"
#include "grail/include/cfg/ParseTree.hpp"

#include "grail/include/algorithm/CFG_PARSE_EARLEY.hpp"

namespace grail { namespace cli {

    template <typename AlphaT>
    class CFG_PARSE {
    public:

        typedef fltl::CFG<AlphaT> CFG;
        typedef typename CFG::terminal_type terminal_type;

        static const char * const TOOL_NAME;

        static void declare(io::CommandLineOptions &opt, bool in_help) throw() {

            opt.declare("predict", io::opt::OPTIONAL, io::opt::NO_VAL);

            io::option_type in(opt.declare(
                "stdin",
                io::opt::OPTIONAL,
                io::opt::NO_VAL
            ));

            if(!in_help) {
                if(in.is_valid()) {
                    opt.declare_min_num_positional(1);
                    opt.declare_max_num_positional(1);
                } else {
                    opt.declare_min_num_positional(2);
                    opt.declare_max_num_positional(2);
                }
            }
        }

        static void help(void) throw() {
            //  "  | |                              |                                             |"
            printf(
                "  %s:\n"
                "    Parses a token stream according to a context-free grammar (CFG).\n\n"
                "  basic use options for %s:\n"
                "    --predict                      compute the FIRST sets of all\n"
                "                                   variables. This computation can\n"
                "                                   take a long time for larger\n"
                "                                   grammars, but can also speed up\n"
                "                                   parsing.\n"
                "    --stdin                        Take the input tokens from standard input.\n"
                "                                   Each tokenmake clean should be separated by a new\n"
                "                                   line. Typing a new line followed by Ctrl-D\n"
                "                                   or Ctrl-Z will close stdin.\n"
                "    <file0>                        read in a CFG from <file>.\n"
                "    <file1>                        read in a newline-separated list of tokens\n"
                "                                   from <file1> if --stdin is not used.\n\n",
                TOOL_NAME, TOOL_NAME
            );
        }

        static int main(io::CommandLineOptions &options) throw() {

            // run the tool
            io::option_type file[2];
            const char *file_name[2] = {0};
            FILE *fp[2] = {0};

            file[0] = options[0U];
            file_name[0] = file[0].value();
            fp[0] = fopen(file_name[0], "r");

            if(0 == fp[0]) {
                options.error(
                    "Unable to open file containing context-free "
                    "grammar for reading."
                );
                options.note("File specified here:", file[0]);

                return 1;
            }

            file[1] = options["stdin"];
            if(file[1].is_valid()) {
                fp[1] = stdin;
                file_name[1] = "<stdin>";
            } else {
                file[1] = options[1U];
                file_name[1] = file[1].value();
                fp[1] = fopen(file_name[1], "r");
            }

            if(0 == fp[1]) {
                options.error(
                    "Unable to open file containing tokens to be parsed."
                );
                options.note("File specified here:", file[1]);

                if(0 != fp[0]) {
                    fclose(fp[0]);
                }

                return 1;
            }

            CFG cfg;
            int ret(0);

            if(io::fread(fp[0], cfg, file_name[0])) {

                std::vector<bool> is_nullable;
                std::vector<std::vector<bool> *> first_terminals;

                // fill the first and nullable sets
                io::verbose("Computing NULL set of variables...\n");
                cfg::compute_null_set(cfg, is_nullable);

                bool use_first_sets(false);
                if(options["predict"].is_valid()) {
                    io::verbose("Computing FIRST set of variables...\n");
                    use_first_sets = true;
                    cfg::compute_first_set(cfg, is_nullable, first_terminals);
                }

                io::verbose("Parsing...\n");

                io::UTF8FileLineBuffer<1024U> reader(fp[1]);

                if(algorithm::CFG_PARSE_EARLEY<AlphaT, 1024U>::run(
                    cfg,
                    is_nullable,
                    use_first_sets,
                    first_terminals,
                    reader
                )) {
                    printf("Yes.\n");
                } else {
                    printf("No.\n");
                }

                // clean out the first set
                for(unsigned i(0); i < first_terminals.size(); ++i) {
                    if(0 != first_terminals[i]) {
                        delete (first_terminals[i]);
                        first_terminals[i] = 0;
                    }
                }

            } else {
                ret = 1;
            }

            fclose(fp[0]);
            fclose(fp[1]);

            return ret;
        }
    };

    template <typename AlphaT>
    const char * const CFG_PARSE<AlphaT>::TOOL_NAME("cfg-parse");

}}

#endif /* FLTL_CFG_PARSE_HPP_ */
