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

#include <cstdio>
#include <map>
#include <set>
#include <utility>

#include "fltl/include/CFG.hpp"
#include "fltl/include/NFA.hpp"

#include "grail/include/io/fread_cfg.hpp"
#include "grail/include/io/fprint_nfa.hpp"
#include "grail/include/io/verbose.hpp"

#include "grail/include/cli/NFA_TO_DOT.hpp"

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
            opt.declare("label-states", io::opt::OPTIONAL, io::opt::NO_VAL);
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
                "    --dot                          output the NFA as a DOT digraph.\n"
                "    --label-states                 label the states instead of the transitions.\n\n",
                TOOL_NAME, TOOL_NAME
            );
        }

        /// represents a pair of variables, e.g. If A --> \alpha B \Beta then
        /// context=A and var=B.
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

        typedef std::set<variable_context> context_set;

        static void run(
            cfg_cfg_type &cfg,
            nfa_nfa_type &nfa,
            bool partition=false,
            bool label_states=true
        ) throw() {
            extract(cfg, nfa, partition, label_states);
        }

        /// extract the stack language from the CFG
        static void extract(
            cfg_cfg_type &cfg,
            nfa_nfa_type &nfa,
            bool partition,
            bool label_states
        ) throw() {
            char name_buff[1024] = {'\0'};

            std::map<variable_context, nfa_state_type> state_map;
            std::map<cfg_variable_type, context_set> contexts;

            cfg_variable_type L, R;
            cfg_symbol_string_type syms;
            cfg_generator_type cfg_vars(cfg.search((~L) --->* ~syms));

            // if we're not partitioning then the pre-emptively set the start
            // state to refer to the start variable of the grammar.
            if(!partition) {
                nfa_state_type start_state(nfa.get_start_state());
                cfg_variable_type start_var(cfg.get_start_variable());
                variable_context start_state_context;

                start_state_context.var = start_var;
                state_map[start_state_context] = start_state;

                if(label_states) {
                    nfa.set_name(start_state, cfg.get_name(start_var));
                }

                nfa.add_accept_state(start_state);
            }

            // go make all states
            for(; cfg_vars.match_next(); ) {
                for(unsigned i(0); i < syms.length(); ++i) {
                    if(!syms.at(i).is_variable()) {
                        continue;
                    }

                    R = syms.at(i);

                    variable_context ctx_var;
                    ctx_var.var = R;
                    if(partition) {
                        ctx_var.context = L;
                    }

                    nfa_state_type state;

                    // create the state
                    if(0U == state_map.count(ctx_var)) {
                        io::verbose("Added state for %s to %s\n", cfg.get_name(L), cfg.get_name(R));
                        state = nfa.add_state();
                        state_map[ctx_var] = state;

                        if(label_states) {
                            if(partition) {
                                sprintf(name_buff, "%s:%s", cfg.get_name(L), cfg.get_name(R));
                                nfa.set_name(state, name_buff);
                            } else {
                                nfa.set_name(state, cfg.get_name(R));
                            }
                        }

                    // we've already created this state
                    } else {
                        state = state_map[ctx_var];
                    }

                    contexts[L].insert(ctx_var);
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
                    if(partition) {
                        ctx_var.context = L;
                    }

                    context_set &related(contexts[R]);
                    typename context_set::iterator it(related.begin());

                    for(; it != related.end(); ++it) {
                        io::verbose("Added transition from %s to %s\n", cfg.get_name(R), cfg.get_name(it->var));

                        nfa_state_type from_state(state_map[ctx_var]);
                        nfa_state_type to_state(state_map[*it]);

                        nfa.add_transition(
                            from_state,
                            label_states ? nfa.epsilon() : nfa.get_symbol(cfg.get_name(it->var)),
                            to_state
                        );
                    }
                }

                // there are transitions from L to R, but there are no transitions
                // from L to R to something else.
                context_set &related(contexts[L]);
                if(!partition && 0U != related.size()) {

                    variable_context ctx_var;
                    ctx_var.var = L;

                    nfa_state_type from_state(state_map[ctx_var]);
                    typename context_set::iterator it(related.begin());

                    if(0U != nfa.num_transitions(from_state)) {
                        continue;
                    }

                    for(; it != related.end(); ++it) {
                        io::verbose("Added transition from %s to %s\n", cfg.get_name(R), cfg.get_name(it->var));

                        nfa_state_type to_state(state_map[*it]);

                        nfa.add_transition(
                            from_state,
                            label_states ? nfa.epsilon() : nfa.get_symbol(cfg.get_name(it->var)),
                            to_state
                        );
                    }
                }
            }

            io::verbose("Adding in start state and transitions.\n");

            // if we are partitioning, then we need to use the start state
            // to signal that parsing is being started
            if(partition) {

                nfa_state_type start_state(nfa.get_start_state());
                cfg_variable_type start_var(cfg.get_start_variable());

                nfa.set_start_state(start_state);
                nfa.add_accept_state(start_state);

                if(label_states) {
                    sprintf(name_buff, ":%s", cfg.get_name(start_var));
                    nfa.set_name(start_state, name_buff);
                }

                std::set<variable_context> &related(contexts[start_var]);
                typename std::set<variable_context>::iterator it(related.begin());

                for(; it != related.end(); ++it) {
                    nfa_state_type to_state(state_map[*it]);
                    nfa.add_transition(
                        start_state,
                        label_states ? nfa.epsilon() : nfa.get_symbol(cfg.get_name(it->var)),
                        to_state
                    );
                }
            }
        }

        static int main(io::CommandLineOptions &options) throw() {

            // run the tool
            io::option_type file(options[0U]);
            io::option_type partition(options["partition"]);
            io::option_type dot(options["dot"]);
            io::option_type label_states(options["label-states"]);

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

            extract(cfg, nfa, partition.is_valid(), label_states.is_valid());

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
