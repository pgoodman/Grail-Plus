/*
 * CommandLineOptions.hpp
 *
 *  Created on: Feb 15, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_COMMANDLINEOPTIONS_HPP_
#define FLTL_COMMANDLINEOPTIONS_HPP_

#include <cctype>
#include <cstdio>
#include <cassert>
#include <cstring>
#include <map>
#include <utility>
#include <string>

#include "fltl/include/preprocessor/COLOR.hpp"

namespace grail {

    namespace opt {
        typedef enum {
            REQUIRED,
            OPTIONAL
        } key_constraint_type;

        typedef enum {
            REQUIRES_VAL,
            NO_VAL,
            OPTIONAL_VAL
        } val_constraint_type;
    }

    // forward declaration
    class CommandLineOptions;

    /// represents a single command line option value
    class CommandLineOption {
    private:

        friend class CommandLineOptions;

        int opt_argv;
        int val_argv;
        const char *opt_begin;
        const char *val_begin;
        const char *val_end;
        bool is_positional;
        bool is_positional_candidate;
        CommandLineOption *next;

        void init(const int, const char *) throw();

        CommandLineOption(void) throw();
    };

    /// container and parser for command-line options
    class CommandLineOptions {
    private:

        const int argc;
        const char **argv;

        // the simple options for the 52 alphabetic characters
        CommandLineOption *short_options[52];
        CommandLineOption *first;
        CommandLineOption *last;

        std::map<std::string, CommandLineOption *> long_options;

        bool has_errors;

        /// report an error with the command-line arguments. this writes errors
        /// out in a Clang-like way, i.e. it tries to highlight and pinpoint the
        /// context of an error
        void message(
            const char *type,
            const char *type_color,
            const char *msg,
            const int argv_offset,
            const size_t err_offset
        ) throw();

        /// report an error with the command-line arguments. this writes errors
        /// out in a Clang-like way, i.e. it tries to highlight and pinpoint the
        /// context of an error
        bool error(
            const unsigned diag,
            const int argv_offset,
            const size_t err_offset
        ) throw();

        bool error(
            const char *diag
        ) throw();

        void error(
            const unsigned diag,
            CommandLineOption *loc
        ) throw();

        /// report an error with the command-line arguments. this writes errors
        /// out in a Clang-like way, i.e. it tries to highlight and pinpoint the
        /// context of an error
        void note(
            const unsigned diag,
            CommandLineOption *loc
        ) throw();

        CommandLineOption *make_option(const int, const char *) throw();
        void check_option(CommandLineOption *option, opt::val_constraint_type vc) throw();

    public:

        CommandLineOptions(const int argc_, const char **argv_) throw();
        ~CommandLineOptions(void) throw();

        bool parse(void) throw();

        void declare(const char *long_opt, opt::key_constraint_type kc, opt::val_constraint_type vc) throw();
        void declare(const char *long_opt, char short_opt, opt::key_constraint_type kc, opt::val_constraint_type vc) throw();
        void declare(char short_opt, opt::key_constraint_type kc, opt::val_constraint_type vc) throw();
    };
}

#endif /* FLTL_COMMANDLINEOPTIONS_HPP_ */
