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
#include "grail/include/io/fread_nfa.hpp"
#include "grail/include/io/fprint_pda.hpp"

namespace grail { namespace cli {

    template <typename AlphaT>
    class PDA_INTERSECT_NFA {
    public:

        static const char * const TOOL_NAME;

        static void declare(CommandLineOptions &opt, bool in_help) throw() {
            if(!in_help) {
                opt.declare_min_num_positional(2);
                opt.declare_max_num_positional(2);
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
            option_type file[2];
            const char *file_name[2] = {0};
            FILE *fp[2] = {0};

            file[0] = options[0U];
            file[1] = options[1U];

            file_name[0] = file[0].value();
            file_name[1] = file[1].value();

            fp[0] = fopen(file_name[0], "r");
            fp[1] = fopen(file_name[1], "r");

            if(0 == fp[0]) {
                options.error(
                    "Unable to open file containing pushdown-automaton "
                    "for reading."
                );
                options.note("File specified here:", file[0]);

                if(0 != fp[1]) {
                    fclose(fp[1]);
                }

                return 1;

            } else if(0 == fp[1]) {
                options.error(
                    "Unable to open file containing non-deterministic finite "
                    "automaton for reading."
                );
                options.note("File specified here:", file[1]);

                if(0 != fp[0]) {
                    fclose(fp[0]);
                }

                return 1;
            }

            PDA<AlphaT> pda;
            NFA<AlphaT> nfa;
            PDA<AlphaT> out;
            int ret(0);

            if(io::fread(fp[0], pda, file_name[0])
            && io::fread(fp[1], nfa, file_name[1])) {

                algorithm::PDA_INTERSECT_NFA<AlphaT>::run(pda, nfa, out);
                io::fprint(stdout, out);

            } else {
                ret = 1;
            }

            fclose(fp[0]);
            fclose(fp[1]);

            return ret;
        }
    };

    template <typename AlphaT>
    const char * const PDA_INTERSECT_NFA<AlphaT>::TOOL_NAME("pda-intersect-nfa");
}}

#endif /* FLTL_CLI_PDA_INTERSECT_NFA_HPP_ */
