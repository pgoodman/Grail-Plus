/*
 * main.cpp
 *
 *  Created on: Sep 14, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#include <cstdio>

#include "fltl/test/Test.hpp"
#include "fltl/test/cfg/CFG.hpp"

int main(void) {

    fltl::test::run_tests();

    return 0;
}

#if 0

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

    if((S + a) != (S + a)) {
        printf("0 not equal, expected equal\n");
    }

    if((a + S) != (a + S)) {
        printf("1 not equal, expected equal\n");
    }

    if((S + a) != (a + S)) {
        printf("2 not equal, expected not equal\n");
    }

    if((S + S) != (S + S)) {
        printf("3 not equal, expected equal\n");
    }
    return 0;
}
#endif
#if 0
    unsigned len[4] = {0}, t;
    for(unsigned i(0); i < 1000000; ++i) {
        /*t = (S + a + prod.symbols() + prod.symbols() + a).length();
        len[0] += t;
        len[3] += t;
        t = (((S + a + prod.symbols() + prod.symbols() + a)) == ((S + a + prod.symbols() + prod.symbols() + a)));
        len[1] += t;
        len[3] += t;
        t = (((S + a)) == ((S + a)));
        len[2] += t;
        len[3] += t;*/

        if((S + a) != (S + a)) {
            printf("not equal\n");
        }

        cfg.add_production(S, buffer.clear() << a << S);
        cfg.add_production(S, buffer.clear() << a);
        cfg.add_production(S, buffer.clear() << a << b << c);
    }

    printf("allocated %u %u %u = %u\n", len[0], len[1], len[2], len[3]);
#endif


