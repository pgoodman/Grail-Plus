/*
 * main.cpp
 *
 *  Created on: Sep 14, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#include <cstdio>

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
    for(unsigned i(0); i < 1000; ++i) {
        len += (S + a + prod.symbols() + prod.symbols() + a).length();
        len += (((S + a + prod.symbols() + prod.symbols() + a)) == ((S + a + prod.symbols() + prod.symbols() + a)));
        len += (((S + a)) == ((S + a)));

        cfg.add_production(S, buffer.clear() << a << S);
        cfg.add_production(S, buffer.clear() << a);
        cfg.add_production(S, buffer.clear() << a << b << c);
    }

    printf("allocated len = %u\n", len);

    return 0;
}

