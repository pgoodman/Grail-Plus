/*
 * main.cpp
 *
 *  Created on: Sep 14, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#include <iostream>

#include "fltl/include/mpl/Sequence.hpp"
#include "fltl/include/mpl/SizeOf.hpp"
#include "fltl/include/mpl/Unit.hpp"
#include "fltl/include/mpl/Query.hpp"
#include "fltl/include/mpl/Expr.hpp"

#include "fltl/include/stl/Variant.hpp"
#include "fltl/include/stl/Tuple.hpp"
#include "fltl/include/stl/Grammar.hpp"

#include "fltl/include/trait/PolyadicOperator.hpp"

#include "fltl/lib/ContextFreeGrammar.hpp"

using namespace fltl;

typedef enum {
    _a, _b, _c, _d
} term_t;

typedef enum {
    EPSILON=0,
    START,
    FIRST,
    REST
} non_term_t;

int main(void) {

    using namespace mpl::query;
    using namespace mpl::query::var;

    //using namespace mpl::query::op;

    typedef lib::ContextFreeGrammar<term_t, non_term_t> cfg_t;

    cfg_t grammar;

    Let<non_term_t, 0> V0("V0");
    Let<non_term_t, 1> V1("V1");
    Let<term_t, 2> alpha("alpha");
    Let<term_t, 3> beta("beta");

    // notes:
    //      - LHS and RHS of ->* bind variables
    //      - invalid:  (_0 != _1 && (_0 ->* (_1 & _2))) because _0 and _1
    //        are not bound until ->*.
    from<cfg_t>::where(
        ((V0 ->* (alpha * beta)) >> (V0 ->* beta)) && (V1 ->* alpha)
    );



    //context_free_grammar_t::RuntimeProductionBuilder builder;

    //grammar.addProduction(EPSILON);

    //(void) builder;
    (void) grammar;

    std::cout << "sizeof cfg_t: " << sizeof(cfg_t) << '\n';

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
