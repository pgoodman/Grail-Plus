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

#include "fltl/include/PDA.hpp"

#include "grail/include/io/error.hpp"
#include "grail/include/io/fread.hpp"
#include "grail/include/io/UTF8FileBuffer.hpp"

#include "grail/include/io/detail/find_next.hpp"
#include "grail/include/io/detail/find_string.hpp"
#include "grail/include/io/detail/find_symbol.hpp"

namespace grail { namespace io {

    namespace pda {

        typedef enum {
            T_START,
            T_START_SET,
            T_FINAL,
            T_FINAL_SET,
            T_SLASH,
            T_COMMA,
            T_NEWLINE,
            T_INPUT_SYMBOL,
            T_STACK_SYMBOL,
            T_END,
            T_ERROR
        } token_type;

        /// tokenize a file as if it contained
        template <const bool LOOK_FOR_ERRORS>
        static token_type get_token(
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

            detail::symbol_state_type sym_state(detail::SYMBOL_FAIL);
            detail::string_state_type str_state(detail::STRING_FAIL);

        read_first_char:
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
                                "File ended before input symbol string "
                                "ended. Terminal string started on line %u, "
                                "column %u.",
                                temp_line, temp_col
                            );
                            return cfg::T_ERROR;

                        case detail::STRING_UNKNOWN_ESCAPE:
                            error(
                                file_name, buffer.line(), buffer.column(),
                                "Unrecognized escape sequence '\\%s' in "
                                "input symbol string.",
                                buffer.reread()
                            );
                            return cfg::T_ERROR;

                        case detail::STRING_TOO_LONG:
                            error(
                                file_name, temp_line, temp_col,
                                "This input symbol string is tool long. The "
                                "maximum length of an input symbol string "
                                "is %u.",
                                cfg::SCRATCH_SIZE - cfg::UTF8_LEN
                            );
                            return cfg::T_ERROR;

                        case detail::STRING_FAIL:
                            error(
                                file_name, temp_line, temp_col,
                                "An unknown failure occurred while parsing "
                                "this input symbol string. The last parsed "
                                "input position was line %u, column %u.",
                                buffer.line(), buffer.column()
                            );
                            return cfg::T_ERROR;

                        case detail::STRING_ACCEPT: break;
                        }
                    }
                    return T_INPUT_SYMBOL;
                }
            }
        }
    }

    /// read in a context free grammar from a file
    template <typename AlphaT>
    bool fread(
        FILE *ff,
        fltl::PDA<AlphaT> &PDA,
        const char * const file_name
    ) throw() {

    }

}}

#endif /* FLTL_FREAD_PDA_HPP_ */
