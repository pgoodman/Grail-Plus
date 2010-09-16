/*
 * main.cpp
 *
 *  Created on: Sep 14, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#include <iostream>

#include "src/include/mpl/Sequence.hpp"
#include "src/include/stl/Sum.hpp"
#include "src/include/cfg/Grammar.hpp"

typedef cftl::Sum<char, int> sum_t;

int main(void) {

    cftl::cfg::Grammar<char> grammar;

    cftl::Sum<char, int> number('f');
    number = 10;
    number = -10;
    number = 'g';

    (void) grammar;
    (void) number;

    number = (unsigned char) 10;

    typedef cftl::Sequence<int, char, float>::Select<sum_t>::type_t select_false_t;
    typedef cftl::Sequence<int, char, float>::Select<char>::type_t select_true_t;

    std::cout << "sizeof sum_t: " << sizeof(sum_t) << '\n'
              << "sizeof sum_t *: " << sizeof(sum_t *) << '\n'
              << "sizeof int *: " << sizeof(int *) << '\n'
              << "sizeof void: " << cftl::SizeOf<void>::VALUE << '\n'
              << "sizeof select_false_t: " << cftl::SizeOf<select_false_t>::VALUE  << '\n'
              << "sizeof select_true_t: " << cftl::SizeOf<select_true_t>::VALUE << '\n';

    std::cout << "value: " << cftl::get<int>(number) << '\n';
    std::cout << "stream: " << number << '\n';

    number = 'a';

    std::cout << "stream: " << number << '\n';

    return 0;
}
