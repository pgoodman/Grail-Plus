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

    //fltl::test::run_tests();

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

    CFG<char>::prod_t P;
    CFG<char>::var_t V;
    CFG<char>::sym_str_t prefix;
    CFG<char>::sym_str_t suffix;
    CFG<char>::generator_t productions(cfg.search(~P, cfg._ --->* cfg._ + cfg.__));

    CFG<char>::pattern_t prod_with_var(cfg._ --->* cfg.__ + ~V + cfg.__);
    CFG<char>::pattern_t nullable_prod(cfg._ --->* cfg.epsilon());
    CFG<char>::pattern_t prod_starts_with_ab(cfg._ --->* a + b + cfg.__);
    CFG<char>::pattern_t prod_is_splittable(cfg._ --->* ~prefix + prefix);

    CFG<char>::generator_t null_productions(cfg.search(~P, nullable_prod));

    unsigned j(0);
    for(unsigned i(0); i < 1000000; ++i, productions.rewind(), null_productions.rewind()) {

        for(; productions.match_next(); ) {

            if(prod_with_var.match(P)) {
                ++j;
            }

            if(prod_starts_with_ab.match(P)) {
                ++j;
            }

            if(prod_is_splittable.match(P)) {
                ++j;
            }
        }

        for(; null_productions.match_next(); ) {
            ++j;
        }
    }

    printf("j = %u\n", j);

    return 0;
}
