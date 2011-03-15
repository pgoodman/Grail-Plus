/*
 * fread_nfa.hpp
 *
 *  Created on: Mar 8, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_FREAD_NFA_HPP_
#define FLTL_FREAD_NFA_HPP_

#include <stdint.h>

#include "grail/include/io/fread.hpp"
#include "grail/include/io/fread_pda.hpp"
#include "grail/include/io/verbose.hpp"

#include "fltl/include/NFA.hpp"

namespace grail { namespace io {

    namespace nfa {
        uint8_t next_state(uint8_t curr_state, pda::token_type input) throw();

        enum {
            STATE_START = 0,
            STATE_FINAL = 3,
            STATE_SINK = 6
        };
    }

    /// read in a context free grammar from a file
    template <typename AlphaT>
    bool fread(
        FILE *ff,
        fltl::NFA<AlphaT> &NFA,
        const char * const file_name
    ) throw() {

        if(0 == ff) {
            return false;
        }

        io::verbose("Reading NFA from '%s'...\n", file_name);

        pda::token_type tt(pda::T_END);
        pda::token_type prev_tt(pda::T_END);

        UTF8FileBuffer<pda::BUFFER_SIZE> buffer(ff);

        // extra space is given to the scratch space to allow short overruns
        char scratch[pda::SCRATCH_SIZE + 20] = {'\0'};
        char *scratch_end(&(scratch[pda::SCRATCH_SIZE - 1]));

        unsigned num_start_states(0);
        unsigned long start_state_val(0);

        uint8_t state(nfa::STATE_START);
        uint8_t prev_state(nfa::STATE_START);

        unsigned line(0);
        unsigned col(0);

        for(;;) {
            prev_tt = tt;
            line = buffer.line();
            col = buffer.column();
            tt = pda::get_token<true>(buffer, scratch, scratch_end, file_name);

            if(pda::T_ERROR == tt) {
                return false;
            }

            prev_state = state;
            state = nfa::next_state(state, tt);

            // looking at the start state
            if(pda::T_STATE == tt && pda::T_START_SET == prev_tt) {
                if(1 == ++num_start_states) {
                    start_state_val = strtoul(scratch, 0, 10);
                }
            } else if(pda::T_STACK_SYMBOL == tt) {
                if(0 != strcmp(scratch, "epsilon")) {
                    error(file_name, line, col,
                        "Expected 'epsilon', but got '%s'",
                        scratch
                    );
                    return false;
                }
            }

            switch(state) {
            case nfa::STATE_FINAL:
                goto checked_syntax;

            case nfa::STATE_SINK:
                switch(tt) {
                case pda::T_START:
                    strcpy(scratch, "(START)");
                    break;
                case pda::T_START_SET:
                    strcpy(scratch, "|-");
                    break;
                case pda::T_FINAL:
                    strcpy(scratch, "(FINAL)");
                    break;
                case pda::T_FINAL_SET:
                    strcpy(scratch, "-|");
                    break;
                case pda::T_SLASH:
                    strcpy(scratch, "/");
                    break;
                case pda::T_COMMA:
                    strcpy(scratch, ",");
                    break;
                case pda::T_NEW_LINE:
                    strcpy(scratch, "\\n");
                    break;
                case pda::T_END:
                    strcpy(scratch, "<EOF>");
                    break;
                default: break;
                }

                error(
                    file_name, buffer.line(), buffer.column(),
                    "Unexpected symbol found with value '%s'. Note: "
                    "previous state of parsing automaton was %u.",
                    scratch, prev_state
                );
                return false;
            }
        }

    checked_syntax:

        // map unsigned longs to states
        std::map<
            unsigned long,
            typename fltl::NFA<AlphaT>::state_type
        > state_map;

        // special case so we don't add in needless epsilon transitions
        if(1 == num_start_states) {
            state_map[start_state_val] = NFA.get_start_state();
        }

        buffer.reset();
        state = pda::STATE_START;

        typename fltl::NFA<AlphaT>::state_type seen_states[2];
        typename fltl::NFA<AlphaT>::state_type *next_seen_state(
            &(seen_states[0])
        );
        typename fltl::NFA<AlphaT>::symbol_type seen_symbol;

        typename fltl::NFA<AlphaT>::alphabet_type sym;

        unsigned long state_id;

        // re-parse without error checking
        for(tt = pda::T_END;;) {

            prev_tt = tt;
            tt = pda::get_token<false>(buffer, scratch, scratch_end, file_name);

            state = nfa::next_state(state, tt);

            switch(tt) {

            case pda::T_INPUT_SYMBOL:
                fltl::NFA<AlphaT>::traits_type::unserialize(scratch, sym);
                seen_symbol = NFA.get_symbol(sym);
                break;

            case pda::T_STACK_SYMBOL:
                seen_symbol = NFA.epsilon();
                break;

            case pda::T_STATE:
                state_id = strtoul(scratch, 0, 10);
                if(0 == state_map.count(state_id)) {
                    state_map[state_id] = NFA.add_state();
                }
                *next_seen_state = state_map[state_id];
                ++next_seen_state;
                break;

            default: break;
            }

            switch(state) {
            case pda::STATE_FINAL:
                goto done;

            case pda::STATE_START:

                // add a final state
                if(pda::T_FINAL == tt) {

                    NFA.add_start_state(seen_states[0]);

                // add a start state
                } else if(pda::T_START_SET == prev_tt) {

                    NFA.add_accept_state(seen_states[0]);

                // add a transition
                } else if(pda::T_STATE == tt) {
                    NFA.add_transition(
                        seen_states[0],
                        seen_symbol,
                        seen_states[1]
                    );
                }

                next_seen_state = &(seen_states[0]);
                break;

            default: break;
            }
        }

    done:

        io::verbose("    %u states,\n", NFA.num_states());
        io::verbose("        %u accept states,\n", NFA.num_accept_states());
        io::verbose("    %u transitions,\n", NFA.num_transitions());
        io::verbose("    %u symbols.\n", NFA.num_symbols());

        return true;
    }

}}

#endif /* FLTL_FREAD_NFA_HPP_ */
