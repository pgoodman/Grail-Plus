/*
 * main.cpp
 *
 *  Created on: Sep 14, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#include "src/include/cfg/Grammar.hpp"

#include "src/include/mpl/Sum.hpp"

int main(void) {

    cftl::Grammar<char> grammar;
    cftl::Sum<int, unsigned, char, unsigned char, float, double> number;

    (void) grammar;
    (void) number;

    return 0;
}
