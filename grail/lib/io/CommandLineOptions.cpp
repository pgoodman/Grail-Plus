/*
 * CommandLineOptions.cpp
 *
 *  Created on: Feb 15, 2011
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

#ifndef FLTL_COMMANDLINEOPTIONS_CPP_
#define FLTL_COMMANDLINEOPTIONS_CPP_

#include "grail/include/io/CommandLineOptions.hpp"

/// this code is a bit of a mishmash

namespace grail { namespace io {

    namespace opt {

        // static array for holding short command line options
        CommandLineOption *short_options[52] = {'\0'};

        helper::CStringMap<CommandLineOption *> long_options;

        const char * const EMPTY_VALUE("");

        CommandLineOption *last_positional_indexed(0);
        unsigned last_positional_index(0);
    }

    namespace detail {
        static int iskeychar(int c) throw() {
            return isalnum(c) || '-' == c;
        }
        static const char *eat_chars(const char *start, int(*pred)(int)) throw() {
            for(; 0 != start && pred(*start); ++start) { }
            return start;
        }
        static size_t alpha_to_offset(char ch) throw() {
            if(ch >= 'a') {
                return size_t(ch - 'a');
            }
            return size_t(ch - 'A');
        }
    }

    namespace diag {

        // the code for a diagnostic. when diagnostics occur, they are
        // referenced with the diagnostic codes.
        enum {
#define DIAG(ENUM_NAME,_) ENUM_NAME,
#include "grail/lib/diag/CommandLineOptions.inc"
#undef DIAG
            __null_diagnostic__
        };

        // bring in the pre-processed diagnostic messages
        static const char *diag_message[] = {
#define DIAG(_,TEXT) TEXT,
#include "grail/lib/diag/CommandLineOptions.inc"
#undef DIAG
        };
    }

    CommandLineOption::CommandLineOption(void) throw()
        : opt_argv(-1)
        , val_argv(0)
        , opt_begin(0)
        , val_begin(0)
        , val_end(0)
        , is_positional(false)
        , is_positional_candidate(true)
        , is_accounted_for(false)
        , next(0)
    { }

    CommandLineOptions &
    CommandLineOptions::operator=(const CommandLineOptions &) throw() {
        assert(false);
        return *this;
    }

    /// initialize the begin and end offsets for a command line option
    /// this attempt
    const char *CommandLineOption::init(
        const int val_argv_,
        const char *val_start
    ) throw() {
        val_begin = val_start;
        val_end = val_start;
        val_argv = val_argv_;

        char prev_char('\0');
        for(const char *end_next(val_end);
            0 != end_next && '\0' != *end_next;
            prev_char = *end_next, ++end_next) {

            if('-' == *end_next && isspace(prev_char)) {
                val_end = end_next;
                break;
            }

            if(!isspace(*end_next)) {
                val_end = end_next + 1;
            }
        }

        return val_end;
    }

    /// constructor
    CommandLineOptions::CommandLineOptions(
        const int argc_,
        const char **argv_
    ) throw()
        : argc(argc_)
        , argv(argv_)
        , first(0)
        , last(0)
        , has_errors(false)
        , disambiguated_positional(false)
        , num_positional(0)
    { }

    CommandLineOptions::~CommandLineOptions(void) throw() {
        for(CommandLineOption *curr(first), *next(0);
            0 != curr;
            curr = next) {

            next = curr->next;
            delete curr;
        }
        first = 0;
        last = 0;
    }

    CommandLineOption *CommandLineOptions::make_option(
        const int argv_offset,
        const char *first_char
    ) throw() {
        CommandLineOption *opt(new CommandLineOption);
        if(0 == first) {
            first = opt;
            last = opt;
        } else {
            last->next = opt;
            last = opt;
        }
        opt->opt_argv = argv_offset;
        opt->opt_begin = first_char;
        return opt;
    }

    /// parse the basic structure of command-line options
    bool CommandLineOptions::parse(void) throw() {

        //CommandLineOption *opt_to_fill(0);
        //bool opt_to_fill_is_long(false);
        size_t equals_offset(0);
        int equals_argv(-1);

        const char *first_char(&(argv[1][0]));
        const char *next_char(0);

        CommandLineOption *opt(0);
        CommandLineOption *last_karg(0);
        bool seen_equal(false);
        bool last_is_short(false);

        size_t offset(0);


        for(int i(1); i < argc; ) {

            first_char = detail::eat_chars(
                first_char,
                &isspace
            );

            offset = static_cast<size_t>(first_char - argv[i]);

            // empty arrgv slot
            if(0 == first_char || '\0' == *first_char) {
                first_char = &(argv[++i][0]);
                continue;

            // looks like some sort of keyword option
            } else if('-' == *first_char) {

                ++first_char;

                // long option
                if('-' == *first_char) {

                    ++first_char;

                    // long option doesn't start with an alphabetic character
                    if(!isalpha(*first_char)) {
                        return error(
                            diag::err_bad_long_option, i, offset + 2UL
                        );
                    }

                    next_char = detail::eat_chars(
                        first_char,
                        &detail::iskeychar
                    );

                    CommandLineOption *curr_opt(opt::long_options.get(
                        first_char,
                        next_char
                    ));

                    if(0 != curr_opt) {
                        error(diag::err_redefine_option, i, offset + 2UL);
                        note(
                            diag::note_prev_option_definition,
                            curr_opt
                        );
                        return false;
                    }

                    // add in the option
                    curr_opt = last_karg = make_option(i, first_char);
                    last_karg->is_positional_candidate = true;
                    seen_equal = false;
                    last_is_short = false;

                    opt::long_options.set(
                        first_char,
                        next_char,
                        curr_opt
                    );

                    // set up the state for the next pass
                    first_char = next_char;
                    next_char = 0;

                // short option
                } else if(isalpha(*first_char)) {

                    const size_t opt_offset(detail::alpha_to_offset(*first_char));

                    // the option already exists!
                    if(0 != opt::short_options[opt_offset]) {
                        error(diag::err_redefine_option, i, offset + 1UL);
                        note(
                            diag::note_prev_option_definition,
                            opt::short_options[opt_offset]
                        );
                        return false;
                    }

                    // make the option
                    opt::short_options[opt_offset] = last_karg = make_option(
                        i,
                        first_char
                    );

                    seen_equal = false;
                    last_is_short = true;

                    // set up the state for the next pass
                    ++first_char;
                    next_char = 0;

                // unknown
                } else {
                    return error(
                        diag::err_bad_first_option_char, i, offset + 1UL
                    );
                }

            // either positional or a value
            } else {
                if('=' == *first_char) {

                    equals_offset = offset;
                    equals_argv = i;

                    if(0 == last_karg) {
                        return error(
                            diag::err_equal_with_no_option, i, offset
                        );
                    } else if(last_is_short) {
                        return error(
                            diag::err_equals_with_short_option, i, offset
                        );
                    } else if(seen_equal) {
                        return error(
                            diag::err_too_many_eqs, i, offset
                        );
                    }

                    last_karg->is_positional_candidate = false;
                    seen_equal = true;
                    ++first_char;

                // positional
                } else if(0 == last_karg) {

                    opt = make_option(i, 0);
                    first_char = opt->init(i, first_char);
                    offset = static_cast<size_t>(first_char - argv[i]);
                    opt->is_positional = true;
                    opt->is_positional_candidate = true;
                    ++num_positional;

                    last_karg = 0;
                    seen_equal = false;
                    last_is_short = false;

                // looks like a value
                } else {
                    first_char = last_karg->init(i, first_char);
                    last_karg->is_positional = false;
                    last_karg = 0;
                    seen_equal = false;
                    last_is_short = false;
                }
            }
        }

        if(0 != last_karg && seen_equal) {
            return error(
                diag::err_equal_with_no_value,
                equals_argv, equals_offset
            );
        }

        return true;
    }

    /// report an error with the command-line arguments. this writes errors
    /// out in a Clang-like way, i.e. it tries to highlight and pinpoint the
    /// context of an error
    bool CommandLineOptions::error(
        const unsigned diag_id,
        const int argv_offset,
        const size_t err_offset
    ) throw() {
        has_errors = true;
        message(
            "error",
            FLTL_F_RED,
            diag::diag_message[diag_id],
            argv_offset,
            err_offset
        );
        return false;
    }

    bool CommandLineOptions::error(
        const char *error_message,
        ...
    ) throw() {
        has_errors = true;
        char buffer[2048] = {'\0'};

        va_list args;
        va_start(args, error_message);
        vsprintf(buffer, error_message, args);
        va_end(args);

        message(
            "error",
            FLTL_F_RED,
            buffer,
            -1,
            0
        );
        return false;
    }

    bool CommandLineOptions::error_simple(
        const char *error_message
    ) throw() {
        has_errors = true;
        message(
            "error",
            FLTL_F_RED,
            error_message,
            -1,
            0
        );
        return false;
    }

    void CommandLineOptions::error(
        const unsigned diag_id,
        CommandLineOption *loc
    ) throw() {
        has_errors = true;
        message(
            "error",
            FLTL_F_RED,
            diag::diag_message[diag_id],
            loc->opt_argv,
            static_cast<size_t>(loc->opt_begin - argv[loc->opt_argv])
        );
    }

    void CommandLineOptions::warning(
        const unsigned diag_id,
        CommandLineOption *loc
    ) throw() {
        message(
            "warning",
            FLTL_F_RED,
            diag::diag_message[diag_id],
            loc->opt_argv,
            static_cast<size_t>(loc->opt_begin - argv[loc->opt_argv])
        );
    }

    void CommandLineOptions::note(
        const unsigned diag_id,
        CommandLineOption *loc
    ) throw() {
        message(
            "note",
            FLTL_F_BLUE,
            diag::diag_message[diag_id],
            loc->opt_argv,
            static_cast<size_t>(loc->opt_begin - argv[loc->opt_argv])
        );
    }

    void CommandLineOptions::note(const char *diag) throw() {
        message(
            "note",
            FLTL_F_BLUE,
            diag,
            -1,
            0
        );
    }

    void CommandLineOptions::note(const char *diag, const option_type &opt) throw() {
        if(0 == opt.option) {
            note(diag);
        } else {
            size_t argv_offset(0);
            size_t offset(0);

            if(0 == opt.option->opt_begin) {

                argv_offset = opt.option->val_argv;
                offset = static_cast<size_t>(
                    opt.option->val_begin - argv[opt.option->val_argv]
                );

            } else {
                argv_offset = opt.option->opt_argv;
                offset = static_cast<size_t>(
                    opt.option->opt_begin - argv[opt.option->opt_argv]
                );
            }

            message(
                "note",
                FLTL_F_BLUE,
                diag,
                static_cast<int>(argv_offset),
                offset
            );
        }
    }

    /// report an error with the command-line arguments. this writes errors
    /// out in a Clang-like way, i.e. it tries to highlight and pinpoint the
    /// context of an error
    void CommandLineOptions::message(
        const char *type,
        const char *type_color,
        const char *msg,
        const int argv_offset,
        const size_t err_offset
    ) throw() {

        enum {
            BUFF_LEN = 128,
            ERROR_CONTEXT = 30,
            TYPE_LEN = 30,
            APPROX_LINE_SPLIT_POINT = 75
        };

        const size_t type_len(strlen(type));
        char error_buffer[BUFF_LEN];
        char padding[TYPE_LEN];

        assert(type_len < TYPE_LEN);

        memset(padding, 0, TYPE_LEN * sizeof(char));
        memset(error_buffer, '\0', BUFF_LEN * sizeof(char));
        memset(padding, ' ', type_len * sizeof(char));

        // print out the message type
        fprintf(stderr,
            FLTL_F_DEF FLTL_F_BOLD "%s%s: " FLTL_F_DEF,
            type_color, type
        );

        // print out the message onto however many lines are needed
        for(unsigned i(0), j(0); ; ++i, ++j) {
            if(msg[i] == ' ') {
                if((type_len + j + 2) >= APPROX_LINE_SPLIT_POINT) {
                    fprintf(stderr, "\n%s  ", padding);
                    j = 0;
                } else {
                    fprintf(stderr, " ");
                }
            } else if('\0' == msg[i]) {
                break;
            } else {
                fprintf(stderr, "%c", msg[i]);
            }
        }

        fprintf(stderr, FLTL_F_DEF "\n");

        if(0 > argv_offset) {
            fprintf(stderr, "\n");
            return;
        }

        size_t context(0);

        // fill in from right-to-left
        int start_char(static_cast<int>(err_offset));
        int context_offset(ERROR_CONTEXT);
        for(int i(argv_offset); 0 <= i; ) {
            for(int j(start_char); 0 <= j; --j, --context_offset) {
                if(0 > context_offset) {
                    goto fill_context_ltr;
                }

                ++context;
                error_buffer[context_offset] = argv[i][j];
            }

            if(0 > context_offset) {
                goto fill_context_ltr;
            } else if(--i < 0) {
                break;
            } else {
                ++context;
                error_buffer[context_offset] = ' ';
                --context_offset;
                start_char = static_cast<int>(strlen(argv[i])) - 1;
            }
        }

        // fill in from left-to-right
    fill_context_ltr:
        start_char = static_cast<int>(err_offset);
        context_offset = ERROR_CONTEXT;
        for(int i(argv_offset); i < argc; ++i, start_char = 0) {
            for(int j(start_char); '\0' != argv[i][j]; ++j, --context_offset) {
                if(0 > context_offset) {
                    goto print_context;
                }

                error_buffer[
                    ERROR_CONTEXT + (ERROR_CONTEXT - context_offset)
                ] = argv[i][j];
            }

            if(0 > context_offset) {
                goto print_context;
            } else {
                error_buffer[ERROR_CONTEXT + (ERROR_CONTEXT - context_offset)] = ' ';
                --context_offset;
            }
        }
    print_context:

        const size_t start(ERROR_CONTEXT - context + 1);
        if(0 != context && '\0' != error_buffer[start]) {
            --context;
            fprintf(stderr,
                "%s  %s\n%s  " FLTL_F_GREEN,
                padding, &(error_buffer[start]), padding
            );
            for(size_t i(0); i < context; ++i) {
                fprintf(stderr, "~");
            }
            fprintf(stderr, FLTL_F_BOLD "^" FLTL_F_DEF "\n\n");
        } else {
            fprintf(stderr, "\n");
        }
    }

    /// perform some post-checking on an option. this disambiguates values
    /// that are candidates for positional arguments, and warns about invalid
    /// values.
    void CommandLineOptions::check_option(
        CommandLineOption *opt,
        const opt::val_constraint_type vc
    ) throw() {

        if(opt::REQUIRES_VAL == vc) {
            if(0 == opt->val_begin) {
                error(diag::err_option_requires_val, opt);
                return;
            }
        } else if(opt::NO_VAL == vc && 0 != opt->val_begin) {

            // split the option into two
            if(opt->is_positional_candidate) {

                CommandLineOption *pos(new CommandLineOption);
                pos->opt_argv = opt->opt_argv;
                pos->val_argv = opt->val_argv;
                pos->val_begin = opt->val_begin;
                pos->val_end = opt->val_end;
                pos->is_positional = true;
                pos->is_positional_candidate = true;
                pos->is_accounted_for = false;

                opt->val_begin = 0;
                opt->val_end = 0;
                opt->is_positional_candidate = false;
                opt->is_accounted_for = true;

                pos->next = opt->next;
                opt->next = pos;

                ++num_positional;

            // there was an '=' sign, error
            } else {
                error(diag::err_option_no_val, opt);
            }
        }
        opt->is_accounted_for = true;
    }

    /// declare an option that can only be addressed by a long keyword form.
    option_type CommandLineOptions::declare(
        const char *long_opt_,
        const opt::key_constraint_type kc,
        const opt::val_constraint_type vc
    ) throw() {
        CommandLineOption *opt(opt::long_options.get(long_opt_));

        if(0 == opt) {
            if(opt::REQUIRED == kc) {
                char buffer[1024];
                memset(buffer, 0, 1024 * sizeof(char));
                sprintf(
                    buffer,
                    diag::diag_message[diag::err_required_long_option],
                    long_opt_
                );
                error_simple(buffer);
            }
        } else {
            check_option(opt, vc);
        }
        return operator[](long_opt_);
    }

    /// declare an option that can be addressed by either one of a short form
    /// or a long form
    option_type CommandLineOptions::declare(
        const char *long_opt_,
        char short_opt_,
        const opt::key_constraint_type kc,
        const opt::val_constraint_type vc
    ) throw() {

        CommandLineOption *long_opt(opt::long_options.get(long_opt_));
        CommandLineOption *short_opt(
            opt::short_options[detail::alpha_to_offset(short_opt_)]
        );

        // both are undefined
        if(0 == long_opt && 0 == short_opt) {

            if(opt::REQUIRED == kc) {
                char buffer[1024];
                memset(buffer, 0, 1024 * sizeof(char));
                sprintf(
                    buffer,
                    diag::diag_message[diag::err_required_long_or_short_option],
                    long_opt_,
                    short_opt_
                );
                error_simple(buffer);
            }

        // one is defined
        } else if(0 == long_opt) {
            opt::long_options.set(long_opt_, short_opt);
            check_option(short_opt, vc);

        // the other is defined
        } else if(0 == short_opt) {
            opt::short_options[detail::alpha_to_offset(short_opt_)] = long_opt;
            check_option(long_opt, vc);

        // both are defined
        } else {
            error(diag::err_redefine_option, long_opt);
            note(
                diag::note_prev_option_definition,
                short_opt
            );
        }

        char short_str[2];
        short_str[0] = short_opt_;
        short_str[1] = '\0';

        return operator[](short_str);
    }

    /// declare an option that only has a short-form
    option_type CommandLineOptions::declare(
        char short_opt_,
        const opt::key_constraint_type kc,
        const opt::val_constraint_type vc
    ) throw() {
        CommandLineOption *opt(
            opt::short_options[detail::alpha_to_offset(short_opt_)]
        );

        if(0 == opt) {
            if(opt::REQUIRED == kc) {
                char buffer[1024];
                memset(buffer, 0, 1024 * sizeof(char));
                sprintf(
                    buffer,
                    diag::diag_message[diag::err_required_short_option],
                    short_opt_
                );
                error_simple(buffer);
            }
        } else {
            check_option(opt, vc);
        }

        char short_str[2];
        short_str[0] = short_opt_;
        short_str[1] = '\0';

        return operator[](short_str);
    }

    /// run through the command line options and warn about
    void CommandLineOptions::disambiguate_undeclared(void) throw() {
        if(disambiguated_positional) {
            return;
        }

        disambiguated_positional = true;

        for(CommandLineOption *opt(first); 0 != opt; opt = opt->next) {
            if(opt->is_accounted_for || opt->is_positional) {
                continue;
            }

            warning(diag::warn_unknown_option, opt);

            if(0 != opt->opt_begin && 0 == opt->val_begin) {
                opt->is_positional_candidate = false;
                continue;
            }

            if(!opt->is_positional_candidate) {
                continue;
            }

            CommandLineOption *pos(new CommandLineOption);
            pos->opt_argv = opt->opt_argv;
            pos->val_argv = opt->val_argv;
            pos->val_begin = opt->val_begin;
            pos->val_end = opt->val_end;
            pos->is_positional = true;
            pos->is_positional_candidate = true;
            pos->is_accounted_for = false;

            opt->val_begin = 0;
            opt->val_end = 0;
            opt->is_positional_candidate = false;
            opt->is_accounted_for = false;

            pos->next = opt->next;
            opt->next = pos;

            ++num_positional;
        }
    }

    /// declare that this tool requires a minimum number of positional
    /// arguments. this should use used *after* declaring keyword arguments
    /// as keyword arguments disambiguate things that look like values to
    /// keyword arguments.
    void CommandLineOptions::declare_min_num_positional(unsigned x) {
        disambiguate_undeclared();
        if(num_positional < x) {

            char buffer[1024];
            memset(buffer, 0, 1024 * sizeof(char));
            sprintf(
                buffer,
                diag::diag_message[diag::err_too_few_positional_opts],
                x
            );
            error_simple(buffer);
        }
    }

    /// declare that this tool accepts no more than a certain number of
    /// positional arguments. this should use used *after* declaring keyword
    /// arguments as keyword arguments disambiguate things that look like
    /// values to keyword arguments.
    void CommandLineOptions::declare_max_num_positional(unsigned x) {
        disambiguate_undeclared();
        if(num_positional > x) {
            char buffer[1024];
            memset(buffer, 0, 1024 * sizeof(char));
            sprintf(
                buffer,
                diag::diag_message[diag::err_too_many_positional_opts],
                x
            );
            error_simple(buffer);
        }
    }

    bool CommandLineOptions::has_error(void) const throw() {
        return has_errors;
    }

    option_type CommandLineOptions::operator[](const char *str) throw() {
        if(has_errors) {
            return option_type(0);
        } else if(0 == str || '\0' == *str) {
            return option_type(0);
        }

        CommandLineOption *opt(0);
        if(isalpha(str[0]) && '\0' == str[1]) {
            opt = opt::short_options[detail::alpha_to_offset(str[0])];
        } else {
            opt = opt::long_options.get(str);
        }

        if(0 == opt || opt->is_positional) {
            return option_type(0);
        } else {
            return option_type(opt);
        }
    }

    /// access a positional option
    option_type CommandLineOptions::operator[](const unsigned offset) throw() {
        if(offset < num_positional) {
            CommandLineOption *opt(first);
            unsigned i(0);

            // we likely will scan positional arguments in the order that
            // they appear and so we remember where we were when we scanned
            // the last time
            if(0 != opt::last_positional_indexed
            && offset >= opt::last_positional_index) {
                opt = opt::last_positional_indexed;
                i = opt::last_positional_index;
            }

            for(; 0 != opt && i <= offset; opt = opt->next) {
                if(opt->is_positional) {
                    if(offset == i) {
                        opt::last_positional_indexed = opt;
                        opt::last_positional_index = i;
                        return option_type(opt);
                    }

                    ++i;
                }
            }
        }
        return option_type();
    }

    option_type::option_type(CommandLineOption *opt) throw()
        : option(opt)
        , c_str_val(0)
    { }
    option_type::option_type(void) throw()
        : option(0)
        , c_str_val(0)
    { }
    option_type::option_type(const option_type &that) throw()
        : option(that.option)
        , c_str_val(0)
    { }
    option_type::~option_type(void) throw() {
        if(0 != c_str_val && opt::EMPTY_VALUE != c_str_val) {
            delete [] c_str_val;
        }
        c_str_val = 0;
        option = 0;
    }
    option_type &option_type::operator=(const option_type &that) throw() {
        option = that.option;
        if(0 != c_str_val && opt::EMPTY_VALUE != c_str_val) {
            delete [] c_str_val;
        }
        c_str_val = 0;
        return *this;
    }

    bool option_type::is_valid(void) const throw() {
        return 0 != option;
    }

    bool option_type::has_value(void) const throw() {
        return 0 != option && 0 != option->val_begin && 0 != option->val_end;
    }

    const char *option_type::value(void) throw() {
        if(0 == c_str_val) {
            if(0 == option || 0 == option->val_begin) {
                c_str_val = opt::EMPTY_VALUE;
            } else {
                const size_t len(static_cast<size_t>(
                    option->val_end - option->val_begin
                ) + 1UL);

                char *c_str(new char[len]);
                memcpy(c_str, option->val_begin, len - 1);
                c_str[len - 1] = '\0';

                c_str_val = c_str;
            }
        }

        return c_str_val;
    }

    std::pair<const char *, const char *> option_type::raw_value(void) const throw() {
        if(0 == option) {
            return std::make_pair<const char *, const char *>(0,0);
        } else {
            return std::make_pair(option->val_begin, option->val_end);
        }
    }
}}

#endif /* FLTL_COMMANDLINEOPTIONS_CPP_ */
