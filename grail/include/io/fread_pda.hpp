/*
 * fread_pda.hpp
 *
 *  Created on: Mar 1, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_FREAD_PDA_HPP_
#define FLTL_FREAD_PDA_HPP_

#include <cctype>
#include <cstring>
#include <cstdlib>
#include <map>
#include <stdint.h>

#include "fltl/include/PDA.hpp"

#include "grail/include/io/error.hpp"
#include "grail/include/io/fread.hpp"
#include "grail/include/io/UTF8FileBuffer.hpp"
#include "grail/include/io/verbose.hpp"

#include "grail/include/io/detail/find_next.hpp"
#include "grail/include/io/detail/find_string.hpp"
#include "grail/include/io/detail/find_symbol.hpp"

namespace grail { namespace io {

    namespace pda {

        typedef enum {
            T_START = 0,
            T_START_SET = 1,
            T_FINAL = 2,
            T_FINAL_SET = 3,
            T_SLASH = 4,
            T_COMMA = 5,
            T_NEW_LINE = 6,
            T_INPUT_SYMBOL = 7,
            T_STACK_SYMBOL = 8,
            T_STATE = 9,
            T_END = 10,
            T_ERROR
        } token_type;

        enum {
            BUFFER_SIZE = 4096,
            SCRATCH_SIZE = 1024,
            UTF8_LEN = 5
        };

        enum {
            STATE_START = 0,
            STATE_SINK = 12,
            STATE_FINAL = 3,
            STATE_ADDED_NFA_TRANS = 8,
            STATE_ADDED_NO_PUSH = 11,
            STATE_ADDED_ANY = 5,
            STATE_SEEN_START_SET = 4,
            STATE_SEEN_FINAL_SET = 7,
            STATE_SEEN_COMMA = 9
        };

        bool is_symbol_codepoint(const char * const cp) throw();
        bool is_numeric_codepoint(const char * const cp) throw();

        uint8_t next_state(uint8_t curr_state, token_type input) throw();

        /// tokenize a file as if it contained
        template <const bool LOOK_FOR_ERRORS>
        static token_type get_token(
            UTF8FileBuffer<BUFFER_SIZE> &buffer,
            char *scratch,
            const char * const scratch_end,
            const char * const file_name
        ) throw() {

            (void) scratch;
            (void) scratch_end;

            const char *codepoint(0);
            char ch('\0');

            unsigned temp_line(0);
            unsigned temp_col(0);

            detail::symbol_state_type sym_state(detail::SYMBOL_FAIL);
            detail::string_state_type str_state(detail::STRING_FAIL);

            for(;;) {
                codepoint = buffer.read();
                ch = *codepoint;

                switch(ch) {
                case '\0': return T_END;

                // terminal
                case '\'':
                case '"':

                    if(LOOK_FOR_ERRORS) {
                        temp_line = buffer.line();
                        temp_col = buffer.column();
                    }

                    str_state = detail::find_string<
                        BUFFER_SIZE,
                        LOOK_FOR_ERRORS
                    >(
                        buffer,
                        scratch,
                        scratch_end,
                        ch
                    );

                    if(LOOK_FOR_ERRORS) {
                        switch(str_state) {

                        case detail::STRING_EARLY_EOF:
                            error(
                                file_name, buffer.line(), buffer.column(),
                                "File ended before input symbol string "
                                "ended. Terminal string started on line %u, "
                                "column %u.",
                                temp_line, temp_col
                            );
                            return T_ERROR;

                        case detail::STRING_UNKNOWN_ESCAPE:
                            error(
                                file_name, buffer.line(), buffer.column(),
                                "Unrecognized escape sequence '\\%s' in "
                                "input symbol string.",
                                buffer.reread()
                            );
                            return T_ERROR;

                        case detail::STRING_TOO_LONG:
                            error(
                                file_name, temp_line, temp_col,
                                "This input symbol string is tool long. The "
                                "maximum length of an input symbol string "
                                "is %u.",
                                SCRATCH_SIZE - UTF8_LEN
                            );
                            return T_ERROR;

                        case detail::STRING_FAIL:
                            error(
                                file_name, temp_line, temp_col,
                                "An unknown failure occurred while parsing "
                                "this input symbol string. The last parsed "
                                "input position was line %u, column %u.",
                                buffer.line(), buffer.column()
                            );
                            return T_ERROR;

                        case detail::STRING_ACCEPT: break;
                        }
                    }
                    return T_INPUT_SYMBOL;

                case ',':
                    return T_COMMA;

                // signal for final state
                case '-':
                    codepoint = buffer.read();
                    ch = *codepoint;
                    if('|' == ch) {
                        return T_FINAL_SET;
                    } else if(LOOK_FOR_ERRORS) {
                        error(
                            file_name, buffer.line(), buffer.column(),
                            "Expected a '|' as part of a '-|' that signifies "
                            "an accept state. Instead, I got '-%s'.",
                            codepoint
                        );
                        return T_ERROR;
                    }
                    break;

                // signal for start state
                case '|':
                    codepoint = buffer.read();
                    ch = *codepoint;
                    if('-' == ch) {
                        return T_START_SET;
                    } else if(LOOK_FOR_ERRORS) {
                        error(
                            file_name, buffer.line(), buffer.column(),
                            "Expected a '-' as part of a '|-' that signifies "
                            "a start state. Instead, I got '|%s'.",
                            codepoint
                        );
                        return T_ERROR;
                    }
                    break;

                // start or final declaration
                case '(':

                    if(LOOK_FOR_ERRORS) {
                        temp_line = buffer.line();
                        temp_col = buffer.column();
                    }

                    sym_state = detail::find_symbol<
                        BUFFER_SIZE,
                        LOOK_FOR_ERRORS
                    >(
                        buffer,
                        scratch,
                        scratch_end,
                        is_symbol_codepoint
                    );

                    codepoint = buffer.read();
                    ch = *codepoint;

                    if(0 == strcmp(scratch, "START") && ')' == ch) {
                        return T_START;
                    } else if(0 == strcmp(scratch, "FINAL") && ')' == ch) {
                        return T_FINAL;
                    } else if(LOOK_FOR_ERRORS) {
                        error(
                            file_name, temp_line, temp_col,
                            "Expected to get '(START)' or '(FINAL)', "
                            "instead I got '(%s%s'.",
                            scratch, codepoint
                        );
                        return T_ERROR;
                    }

                    break;

                // could be a C++-style comment or a slash that separates
                // the pop and push symbols
                case '/':
                    codepoint = buffer.read();
                    ch = *codepoint;

                    if('/' == ch) {
                        goto ignore_line;
                    } else if('*' == ch) {
                        const bool find_close(detail::find_next<
                            BUFFER_SIZE,
                            LOOK_FOR_ERRORS
                        >(
                            buffer, "*/"
                        ));

                        if(LOOK_FOR_ERRORS && !find_close) {
                            error(
                                file_name, buffer.line(), buffer.column(),
                                "Expected '*/' as a closing to match the "
                                "opening '/*' C-style comment block from "
                                "line %u, column %u.",
                                temp_line, temp_col
                            );
                            return T_ERROR;
                        }
                    } else {
                        buffer.unread();
                        return T_SLASH;
                    }
                    break;

                // Python-style comments
                case '#':
                ignore_line:
                    detail::find_next<BUFFER_SIZE,LOOK_FOR_ERRORS>(buffer, "\n");
                    return T_NEW_LINE;

                // new line
                case '\n':
                    return T_NEW_LINE;

                // auto-generated stack symbol
                case '$':
                    scratch[0] = '$';
                    scratch[1] = '\0';
                    detail::find_symbol<
                        BUFFER_SIZE,
                        LOOK_FOR_ERRORS
                    >(
                        buffer,
                        &(scratch[1]),
                        scratch_end,
                        is_numeric_codepoint
                    );

                    if(LOOK_FOR_ERRORS && '\0' == scratch[0]) {
                        error(
                            file_name, buffer.line(), buffer.column(),
                            "I found a '$' and so was expecting a sequence "
                            "of zero-or-more digits following the '$'. This "
                            "type of symbol is used to represent auto-"
                            "generated stack symbols by Grail+."
                        );
                        return T_ERROR;
                    }

                    return T_STACK_SYMBOL;

                default:
                    if(isspace(ch)) {
                        continue;

                    // looks like a state
                    } else if(isdigit(ch)) {
                        scratch[0] = ch;
                        scratch[1] = '\0';

                        detail::find_symbol<
                            BUFFER_SIZE,
                            LOOK_FOR_ERRORS
                        >(
                            buffer,
                            &(scratch[1]),
                            scratch_end,
                            is_numeric_codepoint
                        );

                        return T_STATE;

                    // the start symbol
                    } else if(is_symbol_codepoint(codepoint)) {

                        if(LOOK_FOR_ERRORS) {
                            temp_line = buffer.line();
                            temp_col = buffer.column();
                        }

                        strcpy(scratch, codepoint);
                        sym_state = detail::find_symbol<
                            BUFFER_SIZE,
                            LOOK_FOR_ERRORS
                        >(
                            buffer,
                            scratch + buffer.byte_length(),
                            scratch_end,
                            is_symbol_codepoint
                        );

                        if(LOOK_FOR_ERRORS) {
                            switch(sym_state) {

                            case detail::SYMBOL_TOO_LONG:
                                error(
                                    file_name, temp_line, temp_col,
                                    "This symbol string is tool long. The maximum "
                                    "length of a symbol string is %u.",
                                    SCRATCH_SIZE - UTF8_LEN
                                );
                                return T_ERROR;

                            case detail::SYMBOL_FAIL:
                                error(
                                    file_name, temp_line, temp_col,
                                    "An unknown failure occurred while parsing this "
                                    "symbol string. The last parsed input position "
                                    "was line %u, column %u.",
                                    buffer.line(), buffer.column()
                                );
                                return T_ERROR;

                            default: break;
                            }
                        }

                        return T_STACK_SYMBOL;

                    // unacceptable thing
                    } else if(LOOK_FOR_ERRORS) {
                        error(
                            file_name, buffer.line(), buffer.column(),
                            "Unexpected UTF-8 codepoint found: '%s'.",
                            codepoint
                        );
                        return T_ERROR;
                    }

                    break;
                }
            }

            return T_ERROR;
        }
    }

    /// read in a context free grammar from a file
    template <typename AlphaT>
    bool fread(
        FILE *ff,
        fltl::PDA<AlphaT> &PDA,
        const char * const file_name
    ) throw() {

        if(0 == ff) {
            return false;
        }

        io::verbose("Reading PDA from '%s'...\n", file_name);

        pda::token_type tt(pda::T_END);
        UTF8FileBuffer<pda::BUFFER_SIZE> buffer(ff);

        // extra space is given to the scratch space to allow short overruns
        char scratch[pda::SCRATCH_SIZE + 20] = {'\0'};
        char *scratch_end(&(scratch[pda::SCRATCH_SIZE - 1]));

        unsigned num_start_states(0);
        unsigned long start_state_val(0);

        uint8_t state(pda::STATE_START);
        uint8_t prev_state(pda::STATE_SINK);

        for(;;) {
            tt = pda::get_token<true>(buffer, scratch, scratch_end, file_name);

            if(pda::T_ERROR == tt) {
                return false;
            }

            prev_state = state;
            state = pda::next_state(state, tt);

            // looking at the start state
            if(pda::T_STATE == tt && pda::STATE_SEEN_START_SET == prev_state) {
                if(1 == ++num_start_states) {
                    start_state_val = strtoul(scratch, 0, 10);
                }
            }

            switch(state) {
            case pda::STATE_FINAL:
                goto checked_syntax;

            case pda::STATE_SINK:
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
            typename fltl::PDA<AlphaT>::state_type
        > state_map;

        // special case so we don't add in needless epsilon transitions
        if(1 == num_start_states) {
            state_map[start_state_val] = PDA.get_start_state();
        }

        buffer.reset();
        state = pda::STATE_START;
        prev_state = pda::STATE_SINK;
        uint8_t prev_prev_state(pda::STATE_SINK);

        typename fltl::PDA<AlphaT>::state_type seen_states[2];
        typename fltl::PDA<AlphaT>::state_type *next_seen_state(
            &(seen_states[0])
        );
        typename fltl::PDA<AlphaT>::symbol_type seen_symbols[3];
        typename fltl::PDA<AlphaT>::symbol_type *next_seen_symbol(
            &(seen_symbols[0])
        );

        typename fltl::PDA<AlphaT>::alphabet_type sym;

        unsigned long state_id;

        // re-parse without error checking
        for(;;) {
            tt = pda::get_token<false>(buffer, scratch, scratch_end, file_name);

            prev_prev_state = prev_state;
            prev_state = state;
            state = pda::next_state(state, tt);

            switch(tt) {

            case pda::T_INPUT_SYMBOL:
                fltl::PDA<AlphaT>::traits_type::unserialize(scratch, sym);
                *next_seen_symbol = PDA.get_alphabet_symbol(sym);
                ++next_seen_symbol;
                break;

            case pda::T_STACK_SYMBOL:
                if(0 == strcmp(scratch, "epsilon")) {
                    *next_seen_symbol = PDA.epsilon();
                } else {
                    *next_seen_symbol = PDA.get_stack_symbol(scratch);
                }
                ++next_seen_symbol;
                break;

            case pda::T_STATE:
                state_id = strtoul(scratch, 0, 10);
                if(0 == state_map.count(state_id)) {
                    state_map[state_id] = PDA.add_state();
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

                // we need to add a transition that doesn't push
                if(pda::STATE_ADDED_NO_PUSH == prev_state) {

                    // it doesn't pop either
                    if(pda::STATE_SEEN_COMMA == prev_prev_state) {
                        goto add_nfa_trans;
                    }

                    // it pops one symbol
                    PDA.add_transition(
                        seen_states[0],
                        seen_symbols[0],
                        seen_symbols[1],
                        PDA.epsilon(),
                        seen_states[1]
                    );

                // add a transition that doesn't pop or push
                } else if(pda::STATE_ADDED_NFA_TRANS == prev_state) {
                add_nfa_trans:
                    PDA.add_transition(
                        seen_states[0],
                        seen_symbols[0],
                        PDA.epsilon(),
                        PDA.epsilon(),
                        seen_states[1]
                    );
                }

                next_seen_state = &(seen_states[0]);
                next_seen_symbol = &(seen_symbols[0]);
                prev_state = pda::STATE_START;
                prev_prev_state = pda::STATE_START;
                break;

            // one of several types of transitions
            case pda::STATE_ADDED_ANY:

                // start state
                if(pda::STATE_SEEN_START_SET == prev_state) {
                    PDA.add_start_state(seen_states[0]);

                // final state
                } else if(pda::STATE_SEEN_FINAL_SET == prev_state) {
                    PDA.add_accept_state(seen_states[0]);

                // we are pushing a symbol on, figure out if we're popping
                // as well
                } else if(pda::STATE_ADDED_NO_PUSH == prev_state) {

                    // not popping
                    if(pda::STATE_SEEN_COMMA == prev_prev_state) {

                        PDA.add_transition(
                            seen_states[0],
                            seen_symbols[0],
                            PDA.epsilon(),
                            seen_symbols[1],
                            seen_states[1]
                        );

                    // popping and pushing
                    } else {
                        PDA.add_transition(
                            seen_states[0],
                            seen_symbols[0],
                            seen_symbols[1],
                            seen_symbols[2],
                            seen_states[1]
                        );
                    }
                }

                prev_state = pda::STATE_SINK;

                break;

            default: break;
            }
        }

    done:

        io::verbose("    %u states,\n", PDA.num_states());
        io::verbose("        %u accept states,\n", PDA.num_accept_states());
        io::verbose("    %u transitions,\n", PDA.num_transitions());
        io::verbose("    %u symbols.\n", PDA.num_symbols());

        return true;
    }

}}

#endif /* FLTL_FREAD_PDA_HPP_ */
