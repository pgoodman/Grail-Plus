/*
 * NFA_DOMINATORS.hpp
 *
 *  Created on: Feb 8, 2012
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef Grail_Plus_NFA_DOMINATORS_HPP_
#define Grail_Plus_NFA_DOMINATORS_HPP_

#include <deque>
#include <set>
#include <set>

#include "fltl/include/NFA.hpp"

#include "grail/include/io/CommandLineOptions.hpp"
#include "grail/include/io/fread_nfa.hpp"
#include "grail/include/io/fprint_nfa.hpp"
#include "grail/include/io/verbose.hpp"

#include "grail/include/cli/NFA_TO_DOT.hpp"

namespace grail { namespace cli {

    template <typename AlphaT>
    class NFA_DOMINATORS {
    public:

        FLTL_NFA_USE_TYPES(fltl::NFA<AlphaT>);

        static const char * const TOOL_NAME;

        static void declare(io::CommandLineOptions &opt, bool in_help) throw() {
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
                "    Computes the dominator tree of the NFA, where the nodes of the tree are\n"
                "    states in the NFA. Currently, this ignores transition conditions altogether.\n\n"
                "  basic use options for %s:\n"
                "    <file>                         read in an NFA from <file>.\n"
                "    --dot                          output the NFA as a DOT digraph.\n\n",
                TOOL_NAME, TOOL_NAME
            );
        }

        typedef typename std::set<state_type>::iterator state_iterator;

        /// compute the immediate dominator of a state
        static state_type immediate_dominator(
            std::set<state_type> *dominators,
            state_type to_state
        ) throw() {
            std::set<state_type> doms(dominators[to_state.number()]);
            doms.erase(to_state);

            while(1U < doms.size()) {
                state_iterator it(doms.begin());
                state_type first(*it);
                state_type second(*++it);

                std::set<state_type> &first_doms(dominators[first.number()]);

                // second does not dominate first, i.e. first dominates second
                if(0U == first_doms.count(second)) {
                    doms.erase(first);
                } else {
                    doms.erase(second);
                }
            }

            return *(doms.begin());
        }

        /// intersect sets of states
        static void common_states(
            state_iterator first1, state_iterator last1,
            state_iterator first2, state_iterator last2,
            std::set<state_type> &result
        ) {
            while (first1 != last1 && first2 != last2) {
                if (*first1<*first2) {
                    ++first1;
                } else if (*first2<*first1) {
                    ++first2;
                } else {
                    result.insert(*first1);
                    ++first1;
                    ++first2;
                }
            }
        }

        /// compute the dominator relation for an NFA, then remove all of the
        /// NFA transitions and insert transitions consistent with the dominator
        /// tree.
        static void run(nfa_type &nfa) throw() {

            const unsigned num_states(nfa.num_states());

            std::set<state_type> *dominators(new std::set<state_type>[num_states]);
            std::deque<state_type> work_queue;

            state_type from_state, to_state, start_state(nfa.get_start_state());
            generator_type predecessors(nfa.search(~from_state, nfa._, to_state));
            generator_type successors(nfa.search(from_state, nfa._, ~to_state));
            generator_type states(nfa.search(~to_state));

            // get all states into a set
            std::set<state_type> all_states;
            for(; states.match_next(); ) {
                all_states.insert(to_state);
            }

            // initialize every dominator set to all dominators
            for(states.rewind(); states.match_next(); ) {
                dominators[to_state.number()] = all_states;
            }

            // set the termination conditions
            dominators[start_state.number()].clear();
            dominators[start_state.number()].insert(start_state);

            // compute the dominators
            work_queue.push_back(nfa.get_start_state());
            for(bool force_continue(true); !work_queue.empty(); force_continue = false) {
                to_state = work_queue.front();
                work_queue.pop_front();

                std::set<state_type> &result_dominators(dominators[to_state.number()]);
                const unsigned old_size(static_cast<unsigned>(result_dominators.size()));

                io::verbose("State %u (%s) has %u dominators\n", to_state.number(), nfa.get_name(to_state), old_size);

                // intersect the dominators of the predecessor states
                for(predecessors.rewind(); predecessors.match_next(); ) {
                    if(from_state == to_state) {
                        continue;
                    }

                    io::verbose("    Predecessor %u (%s)\n", from_state.number(), nfa.get_name(from_state));

                    std::set<state_type> to_dominators(result_dominators);
                    std::set<state_type> &from_dominators(dominators[from_state.number()]);

                    io::verbose("        to_dominators = %u\n", static_cast<unsigned>(to_dominators.size()));
                    io::verbose("        from_dominators = %u\n", static_cast<unsigned>(from_dominators.size()));

                    result_dominators.clear();
                    common_states(
                        to_dominators.begin(), to_dominators.end(),
                        from_dominators.begin(), from_dominators.end(),
                        result_dominators
                    );

                    result_dominators.insert(to_state);
                }

                // ensure that the dominator relation is reflexive
                result_dominators.insert(to_state);

                // we shrank this dominator set; enqueue the successors of this
                // state
                if(force_continue || old_size > result_dominators.size()) {
                    from_state = to_state;

                    for(successors.rewind(); successors.match_next(); ) {
                        if(from_state == to_state) {
                            continue;
                        }

                        io::verbose("    Enqueuing %u (%s)\n", to_state.number(), nfa.get_name(to_state));
                        work_queue.push_back(to_state);
                    }

                    io::verbose(
                        "    New number of dominators: %u\n",
                        static_cast<unsigned>(result_dominators.size())
                    );
                }
            }

            // kill all transitions
            transition_type trans;
            generator_type transitions(nfa.search(~trans));
            for(; transitions.match_next(); ) {
                nfa.remove_transition(trans);
            }

            // add transitions for the dominator tree
            for(states.rewind(); states.match_next(); ) {
                if(nfa.get_start_state() == to_state) {
                    continue;
                }

                from_state = immediate_dominator(dominators, to_state);
                nfa.add_transition(from_state, nfa.epsilon(), to_state);
            }

            delete [] dominators;
        }

        static int main(io::CommandLineOptions &options) throw() {

            // run the tool
            io::option_type file(options[0U]);
            io::option_type dot(options["dot"]);
            const char *file_name(file.value());
            FILE *fp(fopen(file_name, "r"));

            if(0 == fp) {
                options.error(
                    "Unable to open file containing non-deterministic finite "
                    "automaton for reading."
                );
                options.note("File specified here:", file);
                return 1;
            }

            nfa_type nfa;
            if(!io::fread(fp, nfa, file_name)) {
                return 1;
            }

            run(nfa);

            // output
            if(dot.is_valid()) {
                NFA_TO_DOT<AlphaT>::print(stdout, nfa);
            } else {
                io::fprint(stdout, nfa);
            }

            return 0;
        }
    };

    template <typename AlphaT>
    const char * const NFA_DOMINATORS<AlphaT>::TOOL_NAME("nfa-dominators");
}}


#endif /* Grail_Plus_NFA_DOMINATORS_HPP_ */
