/*
 * main.cpp
 *
 *  Created on: Sep 14, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */
/*
#include <iostream>

#include "fltl/include/mpl/Sequence.hpp"
#include "fltl/include/mpl/SizeOf.hpp"
#include "fltl/include/mpl/Unit.hpp"
//#include "fltl/include/mpl/Query.hpp"
//#include "fltl/include/mpl/Expr.hpp"


//#include "fltl/include/stl/Variant.hpp"
//#include "fltl/include/stl/Tuple.hpp"
//#include "fltl/include/stl/Grammar.hpp"

#include "fltl/include/trait/PolyadicOperator.hpp"
*/

#include "fltl/lib/CFG.hpp"

int main(void) {

    using fltl::lib::CFG;
    using fltl::mpl::query;

    CFG<char> cfg;

    FLTL_CFG_LET_NON_TERM(var);
    FLTL_CFG_LET_TERM(term);

    CFG<char>::variable_type S(cfg.add_variable());
    CFG<char>::terminal_type a(cfg.get_terminal('a'));
    CFG<char>::terminal_type b(cfg.get_terminal('b'));
    CFG<char>::terminal_type c(cfg.get_terminal('c'));

    (void) S;
    (void) a;
    (void) b;
    (void) c;
    (void) var;
    (void) term;

    CFG<char>::production_builder_type buffer;
    CFG<char>::prod_t prod = cfg.add_production(S, buffer.clear() << a << S << a);

    cfg.add_production(S, buffer.clear() << a << S);
    cfg.add_production(S, buffer.clear() << a);
    cfg.add_production(S, buffer.clear() << a << b << c);

    // duplicates
    cfg.add_production(S, buffer.clear() << a << b << c);
    cfg.add_production(S, buffer.clear() << a << S);

    printf("prod.length() = %u\n", prod.length());
    printf("prod.variable() == prod.symbol(1) = %d\n", prod.variable() == prod.symbol(1));

    unsigned len(0);
    for(unsigned i(0); i < 1000000; ++i) {
        len += (S + a + prod.symbols() + prod.symbols() + a).length();
    }

    printf("allocated len = %u\n", len);

    return 0;
}

#if 0
//#include "fltl/lib/ContextFreeGrammar.hpp"

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

#if 1
typedef struct { int x; } sum_t;
typedef struct { char a; long y; } Foo;

std::ostream &operator<<(std::ostream &os, const Foo &f) {
    return os << "Foo(" << f.a << ", " << f.y << ')';
}

typedef mpl::Sequence<int, char, float>::Select<sum_t>::type select_false_t;
typedef mpl::Sequence<int, char, float>::Select<float>::type select_true_t;
#endif

int main(void) {

    using namespace mpl::query;
    using namespace mpl::query::var;

    //using namespace mpl::query::op;
    /*
    typedef lib::ContextFreeGrammar<term_t, non_term_t> cfg_t;

    cfg_t grammar;

    Let<non_term_t, 0> V0("V0");
    Let<non_term_t, 1> V1("V1");
    Let<term_t, 2> alpha("alpha");
    Let<term_t, 3> beta("beta");
    */
    /*
    in<cfg_t>::where(
        fix<cfg_t>(alpha, beta) [
            (V0 ->* (alpha * beta))
         && (V0 ->* beta)
         && (V0 ->* alpha)
        ]
    );*/

    //from<cfg_t>::where(V0 ->* (alpha + beta) && V0 ->* beta);

    //(void) builder;
    //(void) grammar;
#if 1
    std::cout << "sizeof vector<char>: " << sizeof(std::vector<char>) << '\n';
    std::cout << "sizeof tuple<char, char>: " << sizeof(stl::Tuple<char, char>) << '\n';
    //std::cout << "sizeof cfg_t: " << sizeof(cfg_t) << '\n';
#endif
#if 1
    stl::Variant<char, int> number('f');
    number = 10;
    number = -10;
    number = 'g';

    //(void) grammar;
    (void) number;

    number = (unsigned char) 10;



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

    if(number.has_type<int>()) {
        std::cout << "it contains an int!\n";
    } else if(number.has_type<char>()) {
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
#endif
