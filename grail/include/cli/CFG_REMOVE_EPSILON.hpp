/*
 * CFG_REMOVE_EPSILON.hpp
 *
 *  Created on: May 6, 2012
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef Grail-Plus_CFG_REMOVE_EPSILON_HPP_
#define Grail-Plus_CFG_REMOVE_EPSILON_HPP_


#include <cstdio>

#include "grail/include/algorithm/CFG_REMOVE_EPSILON.hpp"

#include "grail/include/io/CommandLineOptions.hpp"
#include "grail/include/io/fread_cfg.hpp"
#include "grail/include/io/fprint_cfg.hpp"

namespace grail { namespace cli {

    template <typename AlphaT>
    class CFG_REMOVE_EPSILON {
    public:

        FLTL_CFG_USE_TYPES(fltl::CFG<AlphaT>);

        static const char * const TOOL_NAME;

        static void declare(io::CommandLineOptions &opt, bool in_help) throw() {
            io::option_type in(opt.declare("stdin", io::opt::OPTIONAL, io::opt::NO_VAL));
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
                "    Converts a context-free grammar (CFG) into one that has no epsilon\n"
                "    productions, except possibly in the starting production.\n\n"
                "  basic use options for %s:\n"
                "    --stdin                        Read a CFG from stdin. Typing a new\n"
                "                                   line followed by Ctrl-D or Ctrl-Z will\n"
                "                                   close stdin.\n"
                "    <file>                         read in a CFG from <file>.\n\n",
                TOOL_NAME, TOOL_NAME
            );
        }

        static int main(io::CommandLineOptions &options) throw() {

            // run the tool
            io::option_type file;
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

            cfg_type cfg;
            int ret(0);

            if(io::fread(fp, cfg, file_name)) {
                algorithm::CFG_REMOVE_EPSILON<AlphaT>::run(cfg);
                io::fprint(stdout, cfg);
            } else {
                ret = 1;
            }

            fclose(fp);

            return ret;
        }
    };

    template <typename AlphaT>
    const char * const CFG_REMOVE_EPSILON<AlphaT>::TOOL_NAME("cfg-remove-epsilon");
}}


#endif /* Grail-Plus_CFG_REMOVE_EPSILON_HPP_ */
