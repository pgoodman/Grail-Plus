/*
 * CommandLineOptions.cpp
 *
 *  Created on: Feb 15, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_COMMANDLINEOPTIONS_CPP_
#define FLTL_COMMANDLINEOPTIONS_CPP_

#include "grail/include/CommandLineOptions.hpp"

/// this code is a bit of a mishmash

namespace grail {

    namespace opt {

        // static array for holding short command line options
        CommandLineOption *short_options[52] = {'\0'};

        CStringMap<CommandLineOption *> long_options;

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

        CommandLineOption *opt_to_fill(0);
        bool opt_to_fill_is_long(false);
        const char *first_char(0);
        size_t equals_offset(0);
        int equals_argv(-1);

        for(int i(1); i < argc; ++i) {

            first_char = detail::eat_chars(
                &(argv[i][0]),
                &isspace
            );
            size_t offset(static_cast<size_t>(first_char - argv[i]));

        process_first_char:

            // all spaces
            if('\0' == *first_char) {
                continue;

            // keyword argument
            } else if('-' == *first_char) {

                if(0 != opt_to_fill
                && !opt_to_fill->is_positional_candidate) {
                    return error(
                        diag::err_equal_with_no_value,
                        equals_argv, equals_offset
                    );
                }

                opt_to_fill = 0;

                // long option
                if('-' == first_char[1]) {

                    if(!isalpha(first_char[2])) {
                        return error(diag::err_bad_long_option, i, offset + 2UL);
                    }

                    // go find the next non-keyword-like character
                    const char *next_char(detail::eat_chars(
                        &(first_char[2]),
                        &detail::iskeychar
                    ));

                    CommandLineOption *&opt(opt::long_options.get(
                        first_char + 2,
                        next_char
                    ));

                    if(0 != opt) {
                        error(diag::err_redefine_option, i, offset + 2UL);
                        note(
                            diag::note_prev_option_definition,
                            opt
                        );
                        return false;
                    }

                    // add in the option
                    opt = opt_to_fill = make_option(i, first_char + 2);
                    first_char = next_char;
                    opt_to_fill_is_long = true;
                    offset = static_cast<size_t>(next_char - argv[i]);
                    goto process_first_char;

                // short option
                } else if(isalpha(first_char[1])) {

                    // go fin the next non-space character
                    const char *next_char(detail::eat_chars(
                        &(first_char[2]),
                        &isspace
                    ));

                    const size_t opt(detail::alpha_to_offset(first_char[1]));

                    // the option already exists!
                    if(0 != opt::short_options[opt]) {
                        error(diag::err_redefine_option, i, offset + 1UL);
                        note(
                            diag::note_prev_option_definition,
                            opt::short_options[opt]
                        );
                        return false;
                    }

                    opt::short_options[opt] = make_option(
                        i,
                        first_char + 1
                    );

                    // short option followed by a space, the value of this
                    // option should be stored in the next cell, assuming it
                    // exists.
                    if('\0' == first_char[2]) {

                        opt_to_fill_is_long = false;
                        opt_to_fill = opt::short_options[opt];

                    // short option using an '='
                    } else if('=' == first_char[2]) {
                        return error(
                            diag::err_equals_with_short_option,
                            i, offset + 2UL
                        );

                    // the value of this option immediately follows the
                    // letter, possibly preceded by some whitespace
                    } else {
                        const char *end_char(opt::short_options[opt]->init(
                            i, next_char
                        ));

                        // go look for an '=' in this short option
                        for(const char *curr(next_char);
                            0 != curr && curr < end_char;
                            ++curr) {
                            if('=' == *curr && '\\' != *(curr - 1)) {
                                return error(
                                    diag::err_equals_with_short_option,
                                    i, static_cast<size_t>(curr - argv[i])
                                );
                            }
                        }

                        first_char = end_char;
                        offset = static_cast<size_t>(first_char - argv[i]);
                        goto process_first_char;
                    }

                // error, bad option type
                } else {
                    return error(
                        diag::err_bad_first_option_char, i, offset + 1UL
                    );
                }

            // looks like a long option to fill in
            } else if('=' == *first_char) {

                equals_argv = i;
                equals_offset = offset;

                if(0 == opt_to_fill) {
                    return error(
                        diag::err_equal_with_no_option, i, offset
                    );
                } else if(!opt_to_fill_is_long) {
                    return error(
                        diag::err_equals_with_short_option, i, offset
                    );
                }

                // we found and '=', so the user was explicit in that this
                // wasn't a positional argument
                opt_to_fill->is_positional_candidate = false;

                ++first_char;
                offset = static_cast<unsigned>(first_char - argv[i]);

                goto process_dangling;

            // we need to fill in an option
            } else {
            process_dangling:

                if(0 != opt_to_fill) {

                    const char *next_char(
                        detail::eat_chars(first_char, &isspace)
                    );

                    // dangling equals
                    if(0 == next_char || '\0' == *next_char) {
                        opt_to_fill->is_positional_candidate = false;
                        continue;

                    } else if('-' == *next_char) {

                        // missing value with an '=' specified
                        if(!opt_to_fill->is_positional_candidate) {
                            return error(
                                diag::err_equal_with_no_value,
                                equals_argv, equals_offset
                            );

                        // no value missing, process the thing as an option
                        } else {
                            first_char = next_char;
                            offset = static_cast<size_t>(first_char - argv[i]);
                        }

                    // we found the value
                    } else {

                        first_char = opt_to_fill->init(i, next_char);
                        offset = static_cast<unsigned>(first_char - argv[i]);
                    }

                // positional argument
                } else {
                    opt_to_fill = make_option(i, 0);
                    first_char = opt_to_fill->init(i, first_char);
                    offset = static_cast<size_t>(first_char - argv[i]);
                    opt_to_fill->is_positional = true;
                    ++num_positional;
                }

                opt_to_fill = 0;
                goto process_first_char;
            }
        }

        if(0 != opt_to_fill
        && !opt_to_fill->is_positional_candidate) {
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
            message(
                "note",
                FLTL_F_BLUE,
                diag,
                opt.option->opt_argv,
                static_cast<size_t>(opt.option->opt_begin - argv[opt.option->opt_argv])
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
        printf(FLTL_F_DEF);
        printf(
            FLTL_F_BOLD "%s%s: " FLTL_F_DEF FLTL_F_BOLD,
            type_color, type
        );

        // print out the message onto however many lines are needed
        for(unsigned i(0), j(0); ; ++i, ++j) {
            if(msg[i] == ' ') {
                if((type_len + j + 2) >= APPROX_LINE_SPLIT_POINT) {
                    printf("\n%s  ", padding);
                    j = 0;
                } else {
                    printf(" ");
                }
            } else if('\0' == msg[i]) {
                break;
            } else {
                printf("%c", msg[i]);
            }
        }

        printf(FLTL_F_DEF "\n");

        if(0 > argv_offset) {
            printf("\n");
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
            printf(
                "%s  %s\n%s  " FLTL_F_GREEN,
                padding, &(error_buffer[start]), padding
            );
            for(size_t i(0); i < context; ++i) {
                printf("~");
            }
            printf(FLTL_F_BOLD "^" FLTL_F_DEF "\n\n");
        } else {
            printf("\n");
        }
    }

    /// perform some post-checking on an option. this disambiguates values
    /// that are candidates for positional arguments, and warns about invalid
    /// values.
    void CommandLineOptions::check_option(
        CommandLineOption *opt,
        const opt::val_constraint_type vc
    ) throw() {
        if(opt::REQUIRES_VAL == vc && 0 == opt->val_begin) {
            error(diag::err_option_requires_val, opt);
        } else if(opt::NO_VAL == vc && 0 != opt->val_begin) {
            if(opt->is_positional_candidate) {
                ++num_positional;
                opt->is_positional = true;
                opt->opt_begin = 0;
                opt->is_accounted_for = false;
            } else {
                error(diag::err_option_no_val, opt);
            }
        } else {
            opt->is_accounted_for = true;
        }
    }

    /// declare an option that can only be addressed by a long keyword form.
    option_type CommandLineOptions::declare(
        const char *long_opt_,
        const opt::key_constraint_type kc,
        const opt::val_constraint_type vc
    ) throw() {
        CommandLineOption *&opt(opt::long_options.get(long_opt_));
        if(0 == opt) {
            if(opt::REQUIRED == kc) {
                char buffer[1024];
                memset(buffer, 0, 1024 * sizeof(char));
                sprintf(
                    buffer,
                    diag::diag_message[diag::err_required_long_option],
                    long_opt_
                );
                error(buffer);
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

        CommandLineOption *&long_opt(opt::long_options.get(long_opt_));
        CommandLineOption *&short_opt(
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
                error(buffer);
            }

        // one is defined
        } else if(0 == long_opt) {
            long_opt = short_opt;
            check_option(short_opt, vc);

        // the other is defined
        } else if(0 == short_opt) {
            short_opt = long_opt;
            check_option(long_opt, vc);

        // both are defined
        } else {
            error(diag::err_redefine_option, long_opt);
            note(
                diag::note_prev_option_definition,
                short_opt
            );
        }

        return operator[](short_opt_);
    }

    /// declare an option that only has a short-form
    option_type CommandLineOptions::declare(
        char short_opt,
        const opt::key_constraint_type kc,
        const opt::val_constraint_type vc
    ) throw() {
        CommandLineOption *&opt(
            opt::short_options[detail::alpha_to_offset(short_opt)]
        );

        if(0 == opt) {
            if(opt::REQUIRED == kc) {
                char buffer[1024];
                memset(buffer, 0, 1024 * sizeof(char));
                sprintf(
                    buffer,
                    diag::diag_message[diag::err_required_short_option],
                    short_opt
                );
                error(buffer);
            }
        } else {
            check_option(opt, vc);
        }
        return operator[](short_opt);
    }

    /// declare that this tool requires a minimum number of positional
    /// arguments. this should use used *after* declaring keyword arguments
    /// as keyword arguments disambiguate things that look like values to
    /// keyword arguments.
    void CommandLineOptions::declare_min_num_positional(unsigned x) {
        if(num_positional < x) {
            char buffer[1024];
            memset(buffer, 0, 1024 * sizeof(char));
            sprintf(
                buffer,
                diag::diag_message[diag::err_too_few_positional_opts],
                x
            );
            error(buffer);
        }
    }

    /// declare that this tool accepts no more than a certain number of
    /// positional arguments. this should use used *after* declaring keyword
    /// arguments as keyword arguments disambiguate things that look like
    /// values to keyword arguments.
    void CommandLineOptions::declare_max_num_positional(unsigned x) {
        if(num_positional > x) {
            char buffer[1024];
            memset(buffer, 0, 1024 * sizeof(char));
            sprintf(
                buffer,
                diag::diag_message[diag::err_too_many_positional_opts],
                x
            );
            error(buffer);
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
}

#endif /* FLTL_COMMANDLINEOPTIONS_CPP_ */
