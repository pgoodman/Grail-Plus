/*
 * PDA_INTERSECT_NFA.hpp
 *
 *  Created on: Mar 8, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_CLI_PDA_INTERSECT_NFA_HPP_
#define FLTL_CLI_PDA_INTERSECT_NFA_HPP_

#include <cstdio>

#include "grail/include/CommandLineOptions.hpp"

#include "grail/algorithm/PDA_INTERSECT_NFA.hpp"

#include "grail/include/io/fread_pda.hpp"
#include "grail/include/io/fprint_cfg.hpp"

namespace grail { namespace cli {

    template <typename AlphaT>
    class PDA_INTERSECT_NFA {
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
                "    Construct a pushdown automaton (PDA) that accepts the intersection\n"
                "    of the languages accepted by an input PDA and an input non-deterministic\n"
                "    finite automaton (NFA).\n\n"
                "  basic use options for %s:\n"
                "    <file0>                        read in a PDA from <file0>.\n"
                "    <file1>                        read in an NFA from <file1>.\n\n",
                TOOL_NAME, TOOL_NAME
            );
        }

        static int main(CommandLineOptions &options) throw() {

            using fltl::NFA;
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
                    "Unable to open file containing pushdown-automaton "
                    "for reading."
                );
                options.note("File specified here:", file);

                return 1;
            }

            PDA<AlphaT> pda;
            NFA<AlphaT> nfa;
            PDA<AlphaT> out;
            int ret(0);

            if(io::fread(fp, pda, file_name)) {
                algorithm::PDA_INTERSECT_NFA<AlphaT>::run(pda, nfa, out);

            } else {
                ret = 1;
            }

            fclose(fp);

            return ret;
        }
    };

    template <typename AlphaT>
    const char * const PDA_INTERSECT_NFA<AlphaT>::TOOL_NAME("pda-intersect-nfa");
}}

#endif /* FLTL_CLI_PDA_INTERSECT_NFA_HPP_ */
