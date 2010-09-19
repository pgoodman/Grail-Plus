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
#include "src/include/mpl/Unit.hpp"
#include "src/include/stl/Grammar.hpp"

#include "src/include/trait/PolyadicOperator.hpp"

using namespace cftl;

typedef stl::Variant<char, int> sum_t;

class Foo {
public:
    int x;
    float y;
    bool z;
    char a;

    Foo(void) : x(99), y(11.6f), z(false), a('a') {
        std::cout << "initialized Foo! \n";
    }
};

std::ostream &operator<<(std::ostream &os, const Foo &foo) {
    return os << "Foo(x=" << foo.x << ", y=" << foo.y
              << ", z=" << foo.z << ", a=" << foo.a << ')';
}

typedef enum {
    _a, _b, _c, _d
} term_t;

typedef enum {
    EPSILON=0,
    START,
    FIRST,
    REST
} non_term_t;

/// operators for terminals / non-terminals
class catenation : public trait::PolyadicOperator<2> { };
class negation : public trait::PolyadicOperator<1> { };
class conjunction : public trait::PolyadicOperator<2> { };
class disjunction : public trait::PolyadicOperator<2> { };

/// context-free grammar type
typedef stl::Grammar<
    term_t,
    non_term_t,
    disjunction,
    mpl::Sequence<catenation>
> context_free_grammar_t;

/// boolean grammar type
typedef stl::Grammar<
    term_t,
    non_term_t,
    disjunction,
    mpl::Sequence<negation, conjunction>
> boolean_grammar_t;

int main(void) {

    context_free_grammar_t grammar;

    //context_free_grammar_t::RuntimeProductionBuilder builder;

    //grammar.addProduction(EPSILON);

    //(void) builder;
    (void) grammar;

#if 0
    stl::Variant<char, int> number('f');
    number = 10;
    number = -10;
    number = 'g';

    (void) grammar;
    (void) number;

    number = (unsigned char) 10;

    typedef mpl::Sequence<int, char, float>::Select<sum_t>::type_t select_false_t;
    typedef mpl::Sequence<int, char, float>::Select<float>::type_t select_true_t;

    std::cout << "sizeof sum_t: " << sizeof(sum_t) << '\n'
              << "sizeof sum_t *: " << sizeof(sum_t *) << '\n'
              << "sizeof int *: " << sizeof(int *) << '\n'
              << "sizeof void: " << mpl::SizeOf<void>::VALUE << '\n'
              << "sizeof select_false_t: " << mpl::SizeOf<select_false_t>::VALUE  << '\n'
              << "sizeof select_true_t: " << mpl::SizeOf<select_true_t>::VALUE << '\n';

    std::cout << "value: " << number.get<int>() << '\n';
    std::cout << "streamed variant: " << number << '\n';

    number = 'a';

    std::cout << "stream: " << number << '\n';

    if(number.hasType<int>()) {
        std::cout << "it contains an int!\n";
    } else if(number.hasType<char>()) {
        std::cout << "it contains a char!\n";
    }

    stl::Tuple<int,Foo,bool,char> tuple;
    stl::Tuple<int,Foo,bool,char> tuple2;

    tuple.get<3>() = '%';
    std::cout << "streamed tuple: " << tuple << '\n';
    tuple.get<1>().a = 'x';
    std::cout << "streamed tuple: " << tuple << '\n';
    std::cout << "using generic get 1: " << stl::get<1>(tuple) << '\n';

    tuple = tuple2;

    std::cout << "streamed tuple after assignment: " << tuple << '\n';

    stl::Tuple<bool,char,double> tuple3(9, 'x', 10.99);
    std::cout << "streamed tuple3 value constructor: " << tuple3 << '\n';
#endif
    return 0;
}
