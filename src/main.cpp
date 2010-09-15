/*
 * main.cpp
 *
 *  Created on: Sep 14, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#include <iostream>

#include "src/include/cfg/Grammar.hpp"

#include "src/include/stl/Sum.hpp"

int main(void) {

    cftl::Grammar<char> grammar;
    cftl::Sum<int, unsigned, char, unsigned char, float, double> number;

    (void) grammar;
    (void) number;

#define ST cftl::Sum<int, unsigned int>

    std::cout << "total: " << sizeof(ST) << std::endl;
    std::cout << "storage: " << sizeof(ST::STORAGE_SIZE) << std::endl;
    std::cout << "max: " << sizeof(ST::MAX_OBJECT_SIZE) << std::endl;
    std::cout << "div: " << sizeof(ST::MOS_DIV) << std::endl;
    std::cout << "mod: " << sizeof(ST::MOS_MOD) << std::endl;
    std::cout << "padding: " << sizeof(ST::PADDING) << std::endl;
    std::cout << "size: " << sizeof(ST::MEMORY_SIZE) << std::endl;

    return 0;
}
