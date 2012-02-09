/*
 * CFG_STACK_LANG.hpp
 *
 *  Created on: Feb 8, 2012
 *      Author: petergoodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef Grail_Plus_CFG_STACK_LANG_HPP_
#define Grail_Plus_CFG_STACK_LANG_HPP_

#include "fltl/include/CFG.hpp"
#include "fltl/include/NFA.hpp"

#include "grail/include/io/fread_cfg.hpp"
#include "grail/include/io/fprint_nfa.hpp"
#include "grail/include/io/verbose.hpp"

#include "grail/include/cli/NFA_TO_DOT.hpp"

#include <map>
#include <set>
#include <utility>

namespace grail { namespace cli {

    template <typename AlphaT>
    class CFG_STACK_LANG {
    public:

        FLTL_CFG_USE_TYPES_PREFIX(fltl::CFG<AlphaT>, cfg);
        FLTL_NFA_USE_TYPES_PREFIX(fltl::NFA<AlphaT>, nfa);

        static const char * const TOOL_NAME;

        static void declare(io::CommandLineOptions &opt, bool in_help) throw() {
            opt.declare("partition", io::opt::OPTIONAL, io::opt::NO_VAL);
            opt.declare("dot", io::opt::OPTIONAL, io::opt::NO_VAL);
            if(!in_help) {
                opt.declare_min_num_positional(1);
                opt.declare_max_num_positional(1);
            }
        }

        static void help(void) throw() {
            //  "  | |                              |                                             |"
            printf(
                "  %s:\n"
                "    Computes the language of a top-down parsing stack for the inputted Context-\n"
                "    free Grammar (CFG).\n\n"
                "  basic use options for %s:\n"
                "    <file>                         read in a CFG from <file>.\n"
                "    --partition                    parition the set of states in terms of (L,R)\n"
                "                                   pairs where L --> * R * in the grammar, \n"
                "                                   instead of just mapping each variable to a\n"
                "                                   state in the NFA.\n"
                "    --dot                          output the NFA as a DOT digraph.\n\n",
                TOOL_NAME, TOOL_NAME
            );
        }

        struct variable_context {

            cfg_variable_type context;
            cfg_variable_type var;

            bool operator<(const variable_context &that) const throw() {
                if(context < that.context) {
                    return true;
                } else if(that.context < context) {
                    return false;
                } else {
                    return var < that.var;
                }
            }
        };

        static int main(io::CommandLineOptions &options) throw() {

            // run the tool
            io::option_type file(options[0U]);
            io::option_type partition(options["partition"]);
            io::option_type dot(options["dot"]);
            const char *file_name(file.value());

            FILE *fp(fopen(file_name, "r"));

            if(0 == fp) {
                options.error(
                    "Unable to open file containing context-free "
                    "grammar for reading."
                );
                options.note("File specified here:", file);

                return 1;
            }

            cfg_cfg_type cfg;
            nfa_nfa_type nfa;

            // can't bring in the cfg :(
            if(!io::fread(fp, cfg, file_name)) {
                options.error(
                    "Unable to read file containing context-free "
                    "grammar."
                );
                options.note("File specified here:", file);
                return 1;
            }

            io::verbose("Opened '%s'\n", file_name);

            std::map<variable_context, nfa_state_type> state_map;
            std::map<cfg_variable_type, std::set<variable_context> > contexts;

            cfg_variable_type L, R;
            cfg_symbol_string_type syms;
            cfg_generator_type cfg_vars(cfg.search((~L) --->* ~syms));

            // go make all states
            for(; cfg_vars.match_next(); ) {
                for(unsigned i(0); i < syms.length(); ++i) {
                    if(!syms.at(i).is_variable()) {
                        continue;
                    }

                    R = syms.at(i);

                    variable_context ctx_var;
                    ctx_var.var = R;
                    if(partition.is_valid()) {
                        ctx_var.context = L;
                    }

                    io::verbose("Added state for %s to %s\n", cfg.get_name(L), cfg.get_name(R));
                    nfa_state_type state(nfa.add_state());
                    state_map[ctx_var] = state;
                    contexts[L].insert(ctx_var);

                    nfa.set_name(state, cfg.get_name(R));
                }
            }

            // go add all transitions
            for(cfg_vars.rewind(); cfg_vars.match_next(); ) {
                for(unsigned i(0); i < syms.length(); ++i) {
                    if(!syms.at(i).is_variable()) {
                        continue;
                    }

                    R = syms.at(i);

                    variable_context ctx_var;
                    ctx_var.var = R;
                    if(partition.is_valid()) {
                        ctx_var.context = L;
                    }

                    std::set<variable_context> &related(contexts[R]);
                    typename std::set<variable_context>::iterator it(related.begin());

                    for(; it != related.end(); ++it) {
                        io::verbose("Added transition from %s to %s\n", cfg.get_name(R), cfg.get_name(it->var));

                        nfa_state_type from_state(state_map[ctx_var]);
                        nfa_state_type to_state(state_map[*it]);

                        nfa.add_transition(from_state, nfa.epsilon(), to_state);
                    }
                }
            }

            io::verbose("Adding in start state and transitions.\n");

            // add in a start state
            nfa_state_type start_state(nfa.add_state());
            nfa.set_start_state(start_state);
            nfa.add_accept_state(start_state);

            std::set<variable_context> &related(contexts[cfg.get_start_variable()]);
            typename std::set<variable_context>::iterator it(related.begin());

            for(; it != related.end(); ++it) {
                nfa_state_type to_state(state_map[*it]);
                nfa.add_transition(start_state, nfa.epsilon(), to_state);
            }

            // output
            io::verbose("Outputting...\n");
            if(dot.is_valid()) {
                NFA_TO_DOT<AlphaT>::print(stdout, nfa);
            } else {
                io::fprint(stdout, nfa);
            }

            return 0;
        }

    };

    template <typename AlphaT>
    const char * const CFG_STACK_LANG<AlphaT>::TOOL_NAME("cfg-stack-lang");
}}

#endif /* Grail_Plus_CFG_STACK_LANG_HPP_ */
