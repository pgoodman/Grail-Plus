/*
 * main.cpp
 *
 *  Created on: Feb 7, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_MAIN_CPP_
#define FLTL_MAIN_CPP_


#include <map>
#include <list>
#include <string>

#include "fltl/test/Test.hpp"
#include "fltl/test/cfg/CFG.hpp"

#include "grail/algorithm/CFG_REMOVE_EPSILON.hpp"
#include "grail/algorithm/CFG_REMOVE_UNITS.hpp"
#include "grail/algorithm/CFG_TO_CNF.hpp"

#include "grail/include/CommandLineOptions.hpp"


int main(const int argc, const char **argv) throw() {

    grail::CommandLineOptions options(argc, argv);

    if(options.parse()) {

        using namespace grail;

        options.declare("help", 'h', opt::OPTIONAL, opt::NO_VAL);
        options.declare("tool", opt::OPTIONAL, opt::REQUIRES_VAL);

        return 0;
    } else {
        return 1;
    }
}

#endif /* FLTL_MAIN_CPP_ */
