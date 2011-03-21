/*
 * fread_cfg.hpp
 *
 *  Created on: Feb 19, 2011
 *      Author: Peter Goodman
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

#ifndef FLTL_FREAD_CFG_HPP_
#define FLTL_FREAD_CFG_HPP_

#include <cstring>
#include <stdint.h>

#include "fltl/include/CFG.hpp"

#include "grail/include/io/error.hpp"
#include "grail/include/io/fread.hpp"
#include "grail/include/io/verbose.hpp"
#include "grail/include/io/UTF8FileBuffer.hpp"

#include "grail/include/io/detail/find_balanced.hpp"
#include "grail/include/io/detail/find_next.hpp"
#include "grail/include/io/detail/find_string.hpp"
#include "grail/include/io/detail/find_symbol.hpp"

namespace grail { namespace io {

    namespace cfg {
        typedef enum {
            T_TERMINAL = 0,
            T_SYMBOL = 1,
            T_BEGIN_SINGLE_LINE_RELATION = 2,
            T_BEGIN_MULTILINE_RELATION = 3,
            T_EXTEND_MULTILINE_RELATION = 4,
            T_END_RELATION = 5,
            T_NEW_LINE = 6,
            T_END = 7,
            T_ERROR = 8
        } token_type;

        enum {
            STATE_INITIAL = 0,
            STATE_FINAL = 3,
            STATE_SINK = 8,
            STATE_SEEN_PRODUCTION_VARIABLE = 1,
            STATE_EXTEND_OR_CAT_MULTILINE = 6,
            STATE_CAT_SINGLE_LINE = 5,
            STATE_DONE_PRODUCTION = 7
        };

        enum {
            BUFFER_SIZE = 4096,
            SCRATCH_SIZE = 1024,
            UTF8_LEN = 5
        };

        bool is_symbol_codepoint(const char * const cp) throw();
        bool is_numeric_codepoint(const char * const cp) throw();

        uint8_t next_state(uint8_t curr_state, token_type input) throw();

        /// tokenize a file as if it contained
        template <const bool LOOK_FOR_ERRORS>
        static cfg::token_type get_token(
            UTF8FileBuffer<cfg::BUFFER_SIZE> &buffer,
            char *scratch,
            const char * const scratch_end,
            const char * const file_name
        ) throw() {

            (void) scratch;
            (void) scratch_end;

            const char *codepoint(0);
            char ch('\0');
            char prev_ch('\0');

            unsigned temp_line(0);
            unsigned temp_col(0);
            detail::symbol_state_type sym_state(detail::SYMBOL_FAIL);
            detail::string_state_type str_state(detail::STRING_FAIL);

            for(;;) {

                codepoint = buffer.read();
                ch = *codepoint;

                switch(ch) {
                case '\0': return cfg::T_END;

                // start of the first production related to a variable
                case ':': return cfg::T_BEGIN_MULTILINE_RELATION;
                case '|': return cfg::T_EXTEND_MULTILINE_RELATION;
                case ';': return cfg::T_END_RELATION;

                // NLTK-style production
                case '=':
                case '-':
                    codepoint = buffer.read();
                    prev_ch = ch;
                    ch = *codepoint;

                    if('>' == ch) {
                        return cfg::T_BEGIN_SINGLE_LINE_RELATION;
                    } else if(LOOK_FOR_ERRORS && '\0' == ch) {
                        error(
                            file_name, buffer.line(), buffer.column(),
                            "Expected '>' as part of a '%c>' for identifying "
                            "the relation of a single-line production (in the "
                            "style of the Natural Language Toolkit) but instead "
                            "the file abruptly ended!",
                            prev_ch
                        );
                        return cfg::T_ERROR;
                    } else if(LOOK_FOR_ERRORS) {
                        error(
                            file_name, buffer.line(), buffer.column(),
                            "Expected '>' as part of a '->' for identifying "
                            "the relation of a single-line production (in the "
                            "style of the Natural Language Toolkit) but got "
                            "'%s' instead.",
                            codepoint
                        );
                        return cfg::T_ERROR;
                    }
                    break;

                // terminal
                case '\'':
                case '"':

                    if(LOOK_FOR_ERRORS) {
                        temp_line = buffer.line();
                        temp_col = buffer.column();
                    }

                    str_state = detail::find_string<
                        cfg::BUFFER_SIZE,
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
                                "File ended before terminal string ended. "
                                "Terminal string started on line %u, column %u.",
                                temp_line, temp_col
                            );
                            return cfg::T_ERROR;

                        case detail::STRING_UNKNOWN_ESCAPE:
                            error(
                                file_name, buffer.line(), buffer.column(),
                                "Unrecognized escape sequence '\\%s' in "
                                "terminal string.",
                                buffer.reread()
                            );
                            return cfg::T_ERROR;

                        case detail::STRING_TOO_LONG:
                            error(
                                file_name, temp_line, temp_col,
                                "This terminal string is tool long. The maximum "
                                "length of a terminal string is %u.",
                                cfg::SCRATCH_SIZE - cfg::UTF8_LEN
                            );
                            return cfg::T_ERROR;

                        case detail::STRING_FAIL:
                            error(
                                file_name, temp_line, temp_col,
                                "An unknown failure occurred while parsing this "
                                "terminal string. The last parsed input position "
                                "was line %u, column %u.",
                                buffer.line(), buffer.column()
                            );
                            return cfg::T_ERROR;

                        case detail::STRING_ACCEPT: break;
                        }
                    }
                    return cfg::T_TERMINAL;

                case '{':

                    if(LOOK_FOR_ERRORS) {
                        temp_line = buffer.line();
                        temp_col = buffer.column();
                    }

                    codepoint = buffer.read();
                    ch = *codepoint;

                    // java CUP-like code block
                    if(':' == ch) {

                        if(LOOK_FOR_ERRORS) {
                            const bool find_close(detail::find_next<
                                cfg::BUFFER_SIZE,
                                LOOK_FOR_ERRORS
                            >(
                                buffer, ":}"
                            ));

                            if(!find_close) {

                                error(
                                    file_name, buffer.line(), buffer.column(),
                                    "Expected ':}' as a closing to match the "
                                    "opening '{:' from line %u, column %u.",
                                    temp_line, temp_col
                                );
                                return cfg::T_ERROR;
                            }
                        }

                    // code block following C-language conventions of balanced
                    // braces, chew braces until they balance!
                    } else if(LOOK_FOR_ERRORS) {

                        if('\0' == ch) {
                            error(
                                file_name, buffer.line(), buffer.column(),
                                "Expected something after the '{' (e.g. "
                                "inline code) but instead the file abruptly "
                                "ended!"
                            );
                            return cfg::T_ERROR;
                        }

                        const bool find_close(detail::find_balanced<
                            cfg::BUFFER_SIZE,
                            LOOK_FOR_ERRORS
                        >(
                            buffer, '{', '}'
                        ));

                        if(!find_close) {
                            error(
                                file_name, buffer.line(), buffer.column(),
                                "Expected to find a balanced number of "
                                "'{' and '}' starting from line %u, "
                                "column %u.",
                                temp_line, temp_col
                            );
                            return cfg::T_ERROR;
                        }
                    }
                    break;

                // yacc-like thing
                case '%':

                    if(LOOK_FOR_ERRORS) {
                        temp_line = buffer.line();
                        temp_col = buffer.column();
                    }

                    codepoint = buffer.read();
                    ch = *codepoint;

                    // yacc section delimiter, ignore the line just to be safe
                    if('%' == ch) {
                        goto ignore_line;

                    // yacc code block
                    } else if('{' == ch) {

                        const bool find_close(detail::find_next<
                            cfg::BUFFER_SIZE,
                            LOOK_FOR_ERRORS
                        >(
                            buffer, "%}"
                        ));

                        if(LOOK_FOR_ERRORS && !find_close) {
                            error(
                                file_name, buffer.line(), buffer.column(),
                                "Expected '%%}' as a closing to match the opening "
                                "'%%{' from line %u, column %u.",
                                temp_line, temp_col
                            );
                            return cfg::T_ERROR;
                        }

                    } else if(LOOK_FOR_ERRORS && '\0' == ch) {

                        error(
                            file_name, buffer.line(), buffer.column(),
                            "It looks like you were trying to specify a "
                            "Yacc-like special symbol that starts with a '%%' "
                            "but instead the file ended abruptly!"
                        );

                        return cfg::T_ERROR;

                    // if it looks like a yacc prefix then wipe out the line
                    } else {
                        goto ignore_line;
                    }
                    break;

                // possible comment
                case '/':

                    if(LOOK_FOR_ERRORS) {
                        temp_line = buffer.line();
                        temp_col = buffer.column();
                    }

                    codepoint = buffer.read();
                    ch = *codepoint;

                    // C-style comments
                    if('*' == ch) {

                        const bool find_close(detail::find_next<
                            cfg::BUFFER_SIZE,
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
                            return cfg::T_ERROR;
                        }

                    // C++-style comments, ignore the rest of the line
                    } else if('/' == ch) {
                        goto ignore_line;

                    // unexpected end of file
                    } else if(LOOK_FOR_ERRORS && '\0' == ch) {

                        error(
                            file_name, buffer.line(), buffer.column(),
                            "I found a '/' and so was expecting either a "
                            "'/*' or a '//' as a way of beginning a comment "
                            "block. Instead, the file abruptly ended!"
                        );
                        return cfg::T_ERROR;

                    // invalid comment style
                    } else if(LOOK_FOR_ERRORS) {
                        error(
                            file_name, buffer.line(), buffer.column(),
                            "I found a '/' and so was expecting either a "
                            "'/*' or a '//' as a way of beginning a comment "
                            "block. Instead, you gave me '/%s'.",
                            codepoint
                        );
                        return cfg::T_ERROR;
                    }
                    break;

                // Python-style comments
                case '#':
                ignore_line:
                    detail::find_next<cfg::BUFFER_SIZE,LOOK_FOR_ERRORS>(buffer, "\n");
                    return cfg::T_NEW_LINE;

                // new line
                case '\n':
                    return cfg::T_NEW_LINE;

                // auto-generated variable
                case '$':
                    scratch[0] = '$';
                    scratch[1] = '\0';
                    detail::find_symbol<
                        cfg::BUFFER_SIZE,
                        LOOK_FOR_ERRORS
                    >(
                        buffer,
                        &(scratch[1]),
                        scratch_end,
                        cfg::is_numeric_codepoint
                    );

                    if(LOOK_FOR_ERRORS && '\0' == scratch[0]) {
                        error(
                            file_name, buffer.line(), buffer.column(),
                            "I found a '$' and so was expecting a sequence "
                            "of zero-or-more digits following the '$'. This "
                            "type of symbol is used to represent auto-"
                            "generated variable names by Grail+."
                        );
                        return cfg::T_ERROR;
                    }

                    return cfg::T_SYMBOL;

                default:

                    if(isspace(ch)) {
                        continue;

                    // the start symbol
                    } else if(cfg::is_symbol_codepoint(codepoint)) {

                        if(LOOK_FOR_ERRORS) {
                            temp_line = buffer.line();
                            temp_col = buffer.column();
                        }

                        strcpy(scratch, codepoint);
                        sym_state = detail::find_symbol<
                            cfg::BUFFER_SIZE,
                            LOOK_FOR_ERRORS
                        >(
                            buffer,
                            scratch + buffer.byte_length(),
                            scratch_end,
                            cfg::is_symbol_codepoint
                        );

                        if(LOOK_FOR_ERRORS) {
                            switch(sym_state) {

                            case detail::SYMBOL_TOO_LONG:
                                error(
                                    file_name, temp_line, temp_col,
                                    "This symbol string is tool long. The maximum "
                                    "length of a symbol string is %u.",
                                    cfg::SCRATCH_SIZE - cfg::UTF8_LEN
                                );
                                return cfg::T_ERROR;

                            case detail::SYMBOL_FAIL:
                                error(
                                    file_name, temp_line, temp_col,
                                    "An unknown failure occurred while parsing this "
                                    "symbol string. The last parsed input position "
                                    "was line %u, column %u.",
                                    buffer.line(), buffer.column()
                                );
                                return cfg::T_ERROR;

                            case detail::SYMBOL_ACCEPT:
                            default:
                                break;
                            }
                        }

                        return cfg::T_SYMBOL;

                    // unacceptable thing
                    } else if(LOOK_FOR_ERRORS) {
                        error(
                            file_name, buffer.line(), buffer.column(),
                            "Unexpected UTF-8 codepoint found: '%s'.",
                            codepoint
                        );
                        return cfg::T_ERROR;
                    }
                }
            }

            return cfg::T_ERROR;
        }
    }

    /// read in a context free grammar from a file
    template <typename AlphaT>
    bool fread(
        FILE *ff,
        fltl::CFG<AlphaT> &CFG,
        const char * const file_name
    ) throw() {

        if(0 == ff) {
            return false;
        }

        io::verbose("Reading CFG from '%s'...\n", file_name);

        cfg::token_type tt(cfg::T_END);
        UTF8FileBuffer<cfg::BUFFER_SIZE> buffer(ff);

        // extra space is given to the scratch space to allow short overruns
        char scratch[cfg::SCRATCH_SIZE + 20] = {'\0'};
        char *scratch_end(&(scratch[cfg::SCRATCH_SIZE - 1]));

        // pre-process; this goes and looks for syntax errors and tries
        // to build up a map of all of the variables.
        uint8_t prev_state(cfg::STATE_INITIAL);
        uint8_t state(cfg::STATE_INITIAL);
        typename fltl::CFG<AlphaT>::alphabet_type terminal;

        for(unsigned line(0), col(0);;) {

            line = buffer.line();
            col = buffer.column();

            scratch[0] = '\0';
            tt = cfg::get_token<true>(buffer, scratch, scratch_end, file_name);
            prev_state = state;
            state = cfg::next_state(state, tt);

            if(cfg::T_ERROR == tt) {
                return false;

            // add in the terminals
            } else if(cfg::T_TERMINAL == tt) {
                fltl::CFG<AlphaT>::traits_type::unserialize(
                    scratch,
                    terminal
                );
                CFG.get_terminal(terminal);
            }

            switch(state) {
            case cfg::STATE_FINAL:
                goto parsed_successfully;
            case cfg::STATE_SINK:

                switch(tt) {
                case cfg::T_BEGIN_SINGLE_LINE_RELATION:
                    strcpy(scratch, "->");
                    break;
                case cfg::T_BEGIN_MULTILINE_RELATION:
                    strcpy(scratch, ":");
                    break;
                case cfg::T_EXTEND_MULTILINE_RELATION:
                    strcpy(scratch, "|");
                    break;
                case cfg::T_END_RELATION:
                    strcpy(scratch, ";");
                    break;
                case cfg::T_NEW_LINE:
                    strcpy(scratch, "\\n");
                    break;
                case cfg::T_END:
                    strcpy(scratch, "<EOF>");
                    break;

                case cfg::T_ERROR:
                case cfg::T_SYMBOL:
                case cfg::T_TERMINAL:
                default:
                    break;
                }

                error(
                    file_name, buffer.line(), buffer.column(),
                    "Unexpected symbol found with value '%s'. Note: "
                    "previous state of parsing automaton was %u.",
                    scratch, prev_state
                );

                return false;

            // record that a symbol is a variable
            case cfg::STATE_SEEN_PRODUCTION_VARIABLE:
                if(0 == strcmp(scratch, "epsilon")) {
                    error(
                        file_name, line, col,
                        "Cannot re-define meta-variable epsilon."
                    );
                    return false;
                }
                CFG.get_variable(scratch);
                break;
            }
        }
    parsed_successfully:

        // go back to the start of the file
        buffer.reset();
        typename fltl::CFG<AlphaT>::production_builder_type prod_buffer;
        typename fltl::CFG<AlphaT>::variable_type var;

        for(state = cfg::STATE_INITIAL; ;) {

            scratch[0] = '\0';
            tt = cfg::get_token<false>(buffer, scratch, scratch_end, file_name);
            prev_state = state;
            state = cfg::next_state(state, tt);

            switch(state) {

            case cfg::STATE_CAT_SINGLE_LINE:
                goto add_symbol;

            case cfg::STATE_EXTEND_OR_CAT_MULTILINE:
                if(cfg::T_EXTEND_MULTILINE_RELATION != tt) {
                    goto add_symbol;
                }
                /* fall-through */
            case cfg::STATE_DONE_PRODUCTION:
                if(cfg::STATE_CAT_SINGLE_LINE == prev_state
                || cfg::STATE_EXTEND_OR_CAT_MULTILINE == prev_state) {
                    CFG.add_production(var, prod_buffer);
                    prod_buffer.clear();
                }
                break;

            case cfg::STATE_FINAL:
                goto done_parsing;

            case cfg::STATE_SEEN_PRODUCTION_VARIABLE:
                var = CFG.get_variable(scratch);
                break;
            }

            continue;

        add_symbol:
            if(cfg::T_TERMINAL == tt) {
                fltl::CFG<AlphaT>::traits_type::unserialize(
                    scratch,
                    terminal
                );
                prod_buffer.append(CFG.get_terminal(terminal));
            } else if(cfg::T_SYMBOL == tt) {
                if(0 != strcmp(scratch, "epsilon")) {
                    prod_buffer.append(CFG.get_variable_symbol(scratch));
                }
            }
        }

    done_parsing:

        io::verbose("    %u variables,\n", CFG.num_variables());
        io::verbose("    %u productions,\n", CFG.num_productions());
        io::verbose("    %u terminals,\n", CFG.num_terminals());
        io::verbose("    %u variable terminals.\n", CFG.num_variable_terminals());

        return true;
    }

}}

#endif /* FLTL_FREAD_CFG_HPP_ */
