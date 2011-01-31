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

    using fltl::lib::CFG;
    CFG<char> cfg;

    CFG<char>::term_t a(cfg.get_terminal('a'));
    CFG<char>::term_t b(cfg.get_terminal('b'));
    CFG<char>::term_t c(cfg.get_terminal('c'));
    CFG<char>::var_t S(cfg.add_variable());
    CFG<char>::var_t T(cfg.add_variable());
    CFG<char>::var_t U(cfg.add_variable());

    (void) c;

    cfg.add_production(S, S);
    cfg.add_production(S, a + b + S);
    cfg.add_production(S, S + a);
    cfg.add_production(U, U + b);
    cfg.add_production(S, a + b);
    cfg.add_production(S, a + b + b);
    cfg.add_production(T, a + a);
    cfg.add_production(T, a + a + a + a);
    cfg.add_production(U, cfg.epsilon());

    printf("searching for productions\n");
    CFG<char>::prod_t P;
    CFG<char>::var_t V;
    CFG<char>::sym_str_t prefix;
    CFG<char>::sym_str_t suffix;
    CFG<char>::gen_t productions(cfg.search(~P));

    for(; productions.bind_next(); ) {
        printf("found production: ");
        cfg.debug(P);

        if((P.variable() --->* ~prefix + ~V + ~suffix).destructuring_bind(P)) {
            printf("    first variable: ");
            cfg.debug(V);
        } else {
            printf("    no first variable\n");
        }

        if((P.variable() --->* cfg.epsilon()).destructuring_bind(P)) {
            printf("    nullable\n");
        }

        if((P.variable() --->* a + b + ~suffix).destructuring_bind(P)) {
            printf("    starts with \"a b\"\n");
        }

        if((P.variable() --->* ~prefix + prefix).destructuring_bind(P)) {
            printf("    copy & paste prefix: ");
            cfg.debug(prefix);
        }
    }

    printf("searching for variables\n");
    CFG<char>::gen_t variables(cfg.search(~V));

    while(variables.bind_next()) {
        cfg.debug(V);
    }

    printf("searching for terminals\n");
    CFG<char>::term_t t;
    CFG<char>::gen_t terminals(cfg.search(~t));

    while(terminals.bind_next()) {
        cfg.debug(t);
    }

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


