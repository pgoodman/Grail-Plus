/*
 * CFG_REMOVE_UNITS.hpp
 *
 *  Created on: Feb 16, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_CLI_CFG_REMOVE_UNITS_HPP_
#define FLTL_CLI_CFG_REMOVE_UNITS_HPP_

#include "grail/include/CommandLineOptions.hpp"

#include "grail/algorithm/CFG_REMOVE_UNITS.hpp"

namespace grail { namespace cli {

    template <typename AlphaT>
    class CFG_REMOVE_UNITS {
    public:

        static const char * const TOOL_NAME;

        static void declare(CommandLineOptions &options) throw() {
            (void) options;
        }

        static int main(CommandLineOptions &options) throw() {
            (void) options;
            return 0;
        }
    };

    template <typename AlphaT>
    const char * const CFG_REMOVE_UNITS<AlphaT>::TOOL_NAME("cfg-remove-epsilon");
}}

#endif /* FLTL_CLI_CFG_REMOVE_UNITS_HPP_ */
