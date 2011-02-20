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

#include "grail/include/io/fprint.hpp"
#include "grail/include/io/fprint_cfg.hpp"

namespace grail { namespace cli {

    template <typename AlphaT>
    class CFG_TO_CNF {
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
                "    Converts a context-free grammar (CFG) into Chomsky Normal Form. If the\n"
                "    CFG generates the empty string then the only epsilon production in the\n"
                "    CFG will be that of the start variable.\n\n"
                "  basic use options for cfg-to-cnf:\n"
                "    <file>                         read in a CFG from <file>.\n\n",
                TOOL_NAME
            );
        }

        static int main(CommandLineOptions &options) throw() {

            using fltl::lib::CFG;

            // run the tool
            option_type file(options[0U]);
            printf("opening file: %s\n", file.value());

            CFG<char> cfg;

            CFG<char>::term_t a(cfg.get_terminal('a'));
            CFG<char>::term_t b(cfg.get_terminal('b'));

            CFG<char>::var_t S(cfg.add_variable());
            CFG<char>::var_t A(cfg.add_variable());
            CFG<char>::var_t B(cfg.add_variable());

            cfg.add_production(S, A + S + A);
            cfg.add_production(S, a + B);
            cfg.add_production(S, A + S + S + A);
            cfg.add_production(S, A + B + a + S + A + S + A + B);

            cfg.add_production(A, B);
            cfg.add_production(A, S);
            cfg.add_production(A, S + S);
            cfg.add_production(A, S + a);
            cfg.add_production(A, B + B);

            cfg.add_production(B, b);
            cfg.add_production(B, a);
            cfg.add_production(B, A + A);
            cfg.add_production(B, B + A + B + A + B);
            cfg.add_production(B, cfg.epsilon());


            cfg.add_production(S, S);
            cfg.add_production(S, A);
            cfg.add_production(S, B);
            cfg.add_production(A, A);
            cfg.add_production(A, B);
            cfg.add_production(A, S);
            cfg.add_production(B, S);
            cfg.add_production(B, A);
            cfg.add_production(B, B);


            algorithm::CFG_TO_CNF<AlphaT>::run(cfg);
            io::fprint(stdout, cfg);

            return 0;
        }
    };

    template <typename AlphaT>
    const char * const CFG_TO_CNF<AlphaT>::TOOL_NAME("cfg-to-cnf");
}}

#endif /* FLTL_CLI_CFG_TO_CNF_HPP_ */
