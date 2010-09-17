/*
 * main.cpp
 *
 *  Created on: Sep 14, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#include <iostream>

#include "src/include/mpl/Sequence.hpp"
#include "src/include/mpl/SizeOf.hpp"
#include "src/include/stl/Variant.hpp"
#include "src/include/stl/Tuple.hpp"
#include "src/include/cfg/Grammar.hpp"

typedef cftl::Variant<char, int> sum_t;

class Foo {
public:
    int x;
    float y;
    bool z;
    char a;

    Foo(void) : x(99), y(11.6), z(false), a('a') {
        std::cout << "initialized Foo! \n";
    }
};

std::ostream &operator<<(std::ostream &os, const Foo &foo) {
    return os << "Foo(x=" << foo.x << ", y=" << foo.y
              << ", z=" << foo.z << ", a=" << foo.a << ')';
}

int main(void) {

    cftl::cfg::Grammar<char> grammar;

    cftl::Variant<char, int> number('f');
    number = 10;
    number = -10;
    number = 'g';

    (void) grammar;
    (void) number;

    number = (unsigned char) 10;

    typedef cftl::Sequence<int, char, float>::Select<sum_t>::type_t select_false_t;
    typedef cftl::Sequence<int, char, float>::Select<float>::type_t select_true_t;

    std::cout << "sizeof sum_t: " << sizeof(sum_t) << '\n'
              << "sizeof sum_t *: " << sizeof(sum_t *) << '\n'
              << "sizeof int *: " << sizeof(int *) << '\n'
              << "sizeof void: " << cftl::SizeOf<void>::VALUE << '\n'
              << "sizeof select_false_t: " << cftl::SizeOf<select_false_t>::VALUE  << '\n'
              << "sizeof select_true_t: " << cftl::SizeOf<select_true_t>::VALUE << '\n';

    std::cout << "value: " << number.get<int>() << '\n';
    std::cout << "streamed variant: " << number << '\n';

    number = 'a';

    std::cout << "stream: " << number << '\n';

    if(number.hasType<int>()) {
        std::cout << "it contains an int!\n";
    } else if(number.hasType<char>()) {
        std::cout << "it contains a char!\n";
    }

    cftl::Tuple<int,Foo,bool,char> tuple;
    cftl::Tuple<int,Foo,bool,char> tuple2;

    tuple.get<3>() = '%';
    std::cout << "streamed tuple: " << tuple << '\n';
    tuple.get<1>().a = 'x';
    std::cout << "streamed tuple: " << tuple << '\n';
    std::cout << "using generic get 1: " << cftl::get<1>(tuple) << '\n';

    tuple = tuple2;

    std::cout << "streamed tuple after assignment: " << tuple << '\n';

    cftl::Tuple<int,char,double> tuple3(9, 'x', 10.99);
    std::cout << "streamed tuple3 value constructor: " << tuple3 << '\n';

    return 0;
}
