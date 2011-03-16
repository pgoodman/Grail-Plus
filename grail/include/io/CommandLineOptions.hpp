/*
 * CommandLineOptions.hpp
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

#ifndef FLTL_COMMANDLINEOPTIONS_HPP_
#define FLTL_COMMANDLINEOPTIONS_HPP_

#include <cctype>
#include <cstdio>
#include <cassert>
#include <cstring>
#include <cstdarg>
#include <cstdio>

#include "fltl/include/preprocessor/COLOR.hpp"
#include "fltl/include/preprocessor/VARG_CHECK_FORMAT.hpp"

#include "grail/include/helper/CStringMap.hpp"

/// this code is a bit of a mishmash

namespace grail { namespace io {

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
        friend class option_type;

        int opt_argv;
        int val_argv;
        const char *opt_begin;
        const char *val_begin;
        const char *val_end;
        bool is_positional;
        bool is_positional_candidate;
        bool is_accounted_for;
        CommandLineOption *next;

        const char *init(const int, const char *) throw();

        CommandLineOption(void) throw();
    };

    class option_type {
    private:
        friend class CommandLineOptions;

        const CommandLineOption *option;
        const char *c_str_val;

        option_type(CommandLineOption *) throw();

    public:

        option_type(void) throw();
        option_type(const option_type &that) throw();
        ~option_type(void) throw();
        option_type &operator=(const option_type &that) throw();

        bool is_valid(void) const throw();
        bool has_value(void) const throw();
        const char *value(void) throw();

        std::pair<const char *, const char *> raw_value(void) const throw();
    };

    /// container and parser for command-line options
    class CommandLineOptions {
    private:

        const int argc;
        const char **argv;

        // the simple options for the 52 alphabetic characters

        CommandLineOption *first;
        CommandLineOption *last;

        bool has_errors;
        bool disambiguated_positional;
        unsigned num_positional;

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

        void error(
            const unsigned diag,
            CommandLineOption *loc
        ) throw();

        void warning(
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
        void check_option(CommandLineOption *option, const opt::val_constraint_type vc) throw();

        void disambiguate_undeclared(void) throw();

        bool error_simple(const char *message) throw();

    public:

        CommandLineOptions(const int argc_, const char **argv_) throw();
        ~CommandLineOptions(void) throw();

        bool parse(void) throw();

        bool error(
            const char *diag,
            ...
        ) throw() FLTL_VARG_CHECK_FORMAT(2, 3);

        void note(
            const char *diag
        ) throw();

        void note(
            const char *diag,
            const option_type &
        ) throw();

        bool has_error(void) const throw();

        option_type declare(const char *long_opt, const opt::key_constraint_type kc, const opt::val_constraint_type vc) throw();
        option_type declare(const char *long_opt, char short_opt, const opt::key_constraint_type kc, const opt::val_constraint_type vc) throw();
        option_type declare(char short_opt, const opt::key_constraint_type kc, const opt::val_constraint_type vc) throw();

        void declare_min_num_positional(unsigned x);
        void declare_max_num_positional(unsigned x);

        option_type operator[](const char *) throw();

        option_type operator[](const unsigned) throw();
    };
}}

#endif /* FLTL_COMMANDLINEOPTIONS_HPP_ */
