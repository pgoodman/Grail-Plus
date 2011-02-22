/*
 * fread_cfg.hpp
 *
 *  Created on: Feb 19, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_FREAD_CFG_HPP_
#define FLTL_FREAD_CFG_HPP_

#include "fltl/lib/CFG.hpp"

#include "grail/include/io/error.hpp"
#include "grail/include/io/fread.hpp"
#include "grail/include/io/UTF8FileBuffer.hpp"

namespace grail { namespace io {

    namespace cfg {
        typedef enum {
            T_TERMINAL,
            T_SYMBOL,
            T_BEGIN_SINGLE_LINE_RELATION,
            T_BEGIN_MULTILINE_RELATION,
            T_EXTEND_MULTILINE_RELATION,
            T_END_RELATION,
            T_NEW_LINE,
            T_END,
            T_ERROR
        } token_type;

        typedef enum {
            S_EOF,
            S_UNKNOWN_ESCAPE,
            S_TOO_LONG,
            S_ACCEPT,
            S_FAIL
        } terminal_state;

        enum {
            BUFFER_SIZE = 4096,
            SCRATCH_SIZE = 1024,
            UTF8_LEN = 5
        };

        bool is_symbol_codepoint(const char * const cp) throw();

        terminal_state find_symbol(
            UTF8FileBuffer<BUFFER_SIZE> &buffer,
            char *scratch,
            const char * const scratch_end
        ) throw();

        /// look for a string in the input stream; assumes what we're looking
        /// for has at least one non-null character in it. this also assumes
        /// that we're looking for ASCII, and so we can ignore the rest of
        /// the codepoint
        template <const bool LOOK_FOR_ERRORS>
        static bool find_next(
            UTF8FileBuffer<cfg::BUFFER_SIZE> &buffer,
            const char *close
        ) throw() {
            char ch('\0');
            const char *curr(close);

            for(;;) {
                ch = *(buffer.read());
                if(LOOK_FOR_ERRORS && '\0' == ch) {
                    return false;
                } else if(*curr != ch) {
                    curr = close;
                } else {
                    if('\0' == *++curr) {
                        return true;
                    }
                }
            }
            return false;
        }

        /// try to find something that needs to be balanced; assumes non-null
        /// ascii
        template <const bool LOOK_FOR_ERRORS>
        static bool find_balanced(
            UTF8FileBuffer<cfg::BUFFER_SIZE> &buffer,
            const char open,
            const char close
        ) throw() {
            char ch('\0');
            unsigned num(1); // called after we've already found the opening

            for(;;) {
                ch = *(buffer.read());
                if(LOOK_FOR_ERRORS && '\0' == ch) {
                    return false;
                } else if(open == ch) {
                    ++num;
                } else if(close == ch) {
                    // done
                    if(0 == --num) {
                        return true;
                    }
                }
            }
            return false;
        }

        /// fill up the buffer with a token
        template <const bool LOOK_FOR_ERRORS>
        static terminal_state find_terminal(
            UTF8FileBuffer<cfg::BUFFER_SIZE> &buffer,
            char *scratch,
            const char * const scratch_end,
            const char delim
        ) throw() {

            // we have already read in delim, go look for it and collect what we
            // find into scratch
            unsigned num_delims(1);
            unsigned num_delims_seen(0);
            const char *codepoint(0);
            char *mark(0);
            char ch('\0');

            // try to figure out how many delimiter characters we are using
            for(;;) {
                codepoint = buffer.read();
                ch = *codepoint;

                if(delim == ch) {
                    ++num_delims;
                } else {
                    goto handle_next_char;
                }
            }

            for(;;) {
            load_next_char:
                codepoint = buffer.read();
                ch = *codepoint;

            handle_next_char:

                if(LOOK_FOR_ERRORS && scratch >= scratch_end) {
                    return cfg::S_TOO_LONG;
                }

                // looks like an escape sequence
                if('\\' == ch) {
                    codepoint = buffer.read();
                    ch = *codepoint;

                    if(LOOK_FOR_ERRORS && '\0' == ch) {
                        return cfg::S_EOF;
                    }

                    switch(ch) {
                    case 's': *scratch++ = ' '; break;
                    case 't': *scratch++ = '\t'; break;
                    case 'n': *scratch++ = '\n'; break;
                    case 'r': *scratch++ = '\r'; break;
                    case '\\': *scratch++ = '\\'; break;
                    case '"': *scratch++ = '"'; break;
                    case '\'': *scratch++ = '\''; break;

                    default:
                        if(LOOK_FOR_ERRORS) {
                            return cfg::S_UNKNOWN_ESCAPE;
                        }
                    }

                // found a delimiter, might be the end of the string, or might
                // be an embedding of the delimiter in the string assuming
                // num_delims > 1
                } else if(delim == ch) {
                    mark = scratch;
                    *scratch++ = ch;
                    num_delims_seen = 1;

                    for(; num_delims_seen < num_delims; ) {
                        codepoint = buffer.read();
                        strcpy(scratch, codepoint);
                        scratch += buffer.byte_length();

                        if(LOOK_FOR_ERRORS) {
                            if(scratch >= scratch_end) {
                                return cfg::S_TOO_LONG;
                            } else if('\0' == *codepoint) {
                                return cfg::S_EOF;
                            }
                        }

                        if(*codepoint == delim) {
                            ++num_delims_seen;
                        } else {
                            goto load_next_char;
                        }
                    }

                    mark[scratch - mark - num_delims] = '\0';
                    return cfg::S_ACCEPT;

                // error, we got to the end of the file before the end of the
                // string
                } else if(LOOK_FOR_ERRORS && '\0') {
                    return cfg::S_EOF;

                // copy the codepoint into scratch
                } else {
                    strcpy(scratch, codepoint);
                    scratch += buffer.byte_length();
                }
            }

            // shouldn't even get down here
            return cfg::S_FAIL;
        }

        /// try to fill up scratch with a symbol
        template <const bool LOOK_FOR_ERRORS>
        cfg::terminal_state find_symbol(
            UTF8FileBuffer<cfg::BUFFER_SIZE> &buffer,
            char *scratch,
            const char * const scratch_end
        ) throw() {
            const char *codepoint(0);
            for(;;) {
                codepoint = buffer.read();
                if(cfg::is_symbol_codepoint(codepoint)) {
                    strcpy(scratch, codepoint);
                    scratch += buffer.byte_length();

                    if(LOOK_FOR_ERRORS && scratch >= scratch_end) {
                        return cfg::S_TOO_LONG;
                    }
                } else {
                    buffer.unread();
                    *scratch = '\0';
                    return cfg::S_ACCEPT;
                }
            }

            return cfg::S_FAIL;
        }

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

            unsigned temp_line(0);
            unsigned temp_col(0);
            cfg::terminal_state sym_state(cfg::S_FAIL);

        read_first_char:
            for(;;) {
                codepoint = buffer.read();
                ch = *codepoint;

                switch(ch) {
                case '\0': return cfg::T_END;

                // start of the first production related to a variable
                case ':': return cfg::T_BEGIN_MULTILINE_RELATION;
                case '|': return cfg::T_EXTEND_MULTILINE_RELATION;
                case ';': return cfg::T_END_RELATION;
                case '-':
                    codepoint = buffer.read();
                    ch = *codepoint;

                    if('>' == ch) {
                        return cfg::T_BEGIN_SINGLE_LINE_RELATION;
                    } else if(LOOK_FOR_ERRORS && '\0' == ch) {
                        error(
                            file_name, buffer.line(), buffer.column(),
                            "Expected '>' as part of a '->' for identifying "
                            "the relation of a single-line production (in the "
                            "style of the Natural Language Toolkit) but instead "
                            "the file abruptly ended!"
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

                    sym_state = cfg::find_terminal<LOOK_FOR_ERRORS>(
                        buffer,
                        scratch,
                        scratch_end,
                        ch
                    );

                    if(LOOK_FOR_ERRORS) {
                        switch(sym_state) {

                        case cfg::S_EOF:
                            error(
                                file_name, buffer.line(), buffer.column(),
                                "File ended before terminal string ended. "
                                "Terminal string started on line %u, column %u.",
                                temp_line, temp_col
                            );
                            return cfg::T_ERROR;

                        case cfg::S_UNKNOWN_ESCAPE:
                            error(
                                file_name, buffer.line(), buffer.column(),
                                "Unrecognized escape sequence '\\%s' in "
                                "terminal string.",
                                buffer.reread()
                            );
                            return cfg::T_ERROR;

                        case cfg::S_TOO_LONG:
                            error(
                                file_name, temp_line, temp_col,
                                "This terminal string is tool long. The maximum "
                                "length of a terminal string is %u.",
                                cfg::SCRATCH_SIZE - cfg::UTF8_LEN
                            );
                            return cfg::T_ERROR;

                        case cfg::S_FAIL:
                            error(
                                file_name, temp_line, temp_col,
                                "An unknown failure occurred while parsing this "
                                "terminal string. The last parsed input position "
                                "was line %u, column %u.",
                                buffer.line(), buffer.column()
                            );
                            return cfg::T_ERROR;

                        case cfg::S_ACCEPT: break;
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

                        const bool find_close(cfg::find_next<LOOK_FOR_ERRORS>(
                            buffer, ":}"
                        ));

                        if(LOOK_FOR_ERRORS && !find_close) {
                            error(
                                file_name, buffer.line(), buffer.column(),
                                "Expected ':}' as a closing to match the opening "
                                "'{:' from line %u, column %u.",
                                temp_line, temp_col
                            );
                            return cfg::T_ERROR;
                        }

                    } else if(LOOK_FOR_ERRORS && '\0' == ch) {
                        error(
                            file_name, buffer.line(), buffer.column(),
                            "Expected something after the '{' (e.g. inline "
                            "code) but instead the file abruptly ended!"
                        );
                        return cfg::T_ERROR;

                    // code block following C-language conventions of balanced
                    // braces, chew braces until they balance!
                    } else {
                        const bool find_close(cfg::find_balanced<LOOK_FOR_ERRORS>(
                            buffer, '{', '}'
                        ));

                        if(LOOK_FOR_ERRORS && !find_close) {
                            error(
                                file_name, buffer.line(), buffer.column(),
                                "Expected to find a balanced number of '{' and "
                                "'}' starting from line %u, column %u.",
                                temp_line, temp_col
                            );
                            return cfg::T_ERROR;
                        }
                    }

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

                        const bool find_close(cfg::find_next<LOOK_FOR_ERRORS>(
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

                // possible comment
                case '/':
                    codepoint = buffer.read();
                    ch = *codepoint;

                    // C-style comments
                    if('*' == ch) {

                        for(;;) {
                            codepoint = buffer.read();
                            ch = *codepoint;

                            if('\0' == ch) {

                            } else if('*' == ch) {

                            }
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

                // Python-style comments
                case '#':
                ignore_line:
                    for(;;) {
                        codepoint = buffer.read();
                        ch = *codepoint;
                        if('\0' == ch) {
                            return cfg::T_END;
                        } else if('\n' == ch) {
                            goto read_first_char;
                        }
                    }
                    break;

                // new line
                case '\n':
                    return cfg::T_NEW_LINE;

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
                        sym_state = cfg::find_symbol<LOOK_FOR_ERRORS>(
                            buffer,
                            scratch + buffer.byte_length(),
                            scratch_end
                        );

                        if(LOOK_FOR_ERRORS) {
                            switch(sym_state) {

                            case cfg::S_TOO_LONG:
                                error(
                                    file_name, temp_line, temp_col,
                                    "This symbol string is tool long. The maximum "
                                    "length of a symbol string is %u.",
                                    cfg::SCRATCH_SIZE - cfg::UTF8_LEN
                                );
                                return cfg::T_ERROR;

                            case cfg::S_FAIL:
                                error(
                                    file_name, temp_line, temp_col,
                                    "An unknown failure occurred while parsing this "
                                    "symbol string. The last parsed input position "
                                    "was line %u, column %u.",
                                    buffer.line(), buffer.column()
                                );
                                return cfg::T_ERROR;

                            default: break;
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

    template <typename AlphaT>
    bool fread(
        FILE *ff,
        fltl::lib::CFG<AlphaT> &cfg,
        const char * const file_name
    ) throw() {

        if(0 == ff) {
            return false;
        }

        (void) cfg;

        cfg::token_type tt(cfg::T_END);
        UTF8FileBuffer<cfg::BUFFER_SIZE> buffer(ff);

        // extra space is given to the scratch space to allow short overruns
        char scratch[cfg::SCRATCH_SIZE + 20] = {'\0'};
        char *scratch_end(&(scratch[cfg::SCRATCH_SIZE - 1]));

        // pre-process; this goes and looks for syntax errors and tries
        // to build up a map of all of the variables.
        unsigned num_tokens(0);
        for(;;) {
            scratch[0] = '\0';
            tt = cfg::get_token<true>(buffer, scratch, scratch_end, file_name);

            if(cfg::T_END == tt) {
                break;
            } else if(cfg::T_ERROR == tt) {
                return false;
            } else {
                ++num_tokens;
            }
        }

        printf("num_tokens = %u\n\n", num_tokens);

        // go back to the start of the file
        buffer.reset();

        /*
        for(tt = cfg::get_token(buffer, scratch, scratch_end);
            T_ERROR != tt ;
            tt = cfg::get_token(buffer, scratch, scratch_end)) {

            printf("token %u\n", static_cast<unsigned>(tt));

            if(T_END == tt) {
                break;
            }
        }*/

        return cfg::T_END == tt;
    }

}}

#endif /* FLTL_FREAD_CFG_HPP_ */
