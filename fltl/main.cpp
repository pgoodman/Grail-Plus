/*
 * main.cpp
 *
 *  Created on: Sep 14, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#include <cstdio>
#include <set>
#include <utility>

#include "fltl/lib/CFG.hpp"

#define ON 1

#if ON

#include "fltl/test/Test.hpp"
#include "fltl/test/cfg/CFG.hpp"

template <typename A>
void print_grammar(fltl::CFG<A> &);



/// print a context-free grammar
template <typename A>
void print_grammar(fltl::CFG<A> &cfg) throw() {
    using fltl::CFG;
    typename fltl::CFG<A>::production_type P;
    typename fltl::CFG<A>::variable_type S(cfg.get_start_variable());

    typename fltl::CFG<A>::generator_type prods(cfg.search(~P));
    typename fltl::CFG<A>::generator_type start_prods(cfg.search(
        ~P,
        S --->* cfg.__
    ));

    while(start_prods.match_next()) {
        cfg.debug(P);
    }

    while(prods.match_next()) {
        if(S != P.variable()) {
            cfg.debug(P);
        }
    }
}

#endif
int main(void) {

    fltl::test::run_tests();

    using fltl::CFG;

    unsigned j(0);

    for(unsigned i(0); i < 5000; ++i) {
        CFG<char> cfg;

        CFG<char>::term_t a(cfg.get_terminal('a'));
        CFG<char>::term_t b(cfg.get_terminal('b'));

        CFG<char>::var_t S(cfg.add_variable());
        CFG<char>::var_t A(cfg.add_variable());
        CFG<char>::var_t B(cfg.add_variable());

        /*
        cfg.add_production(S, A + S + A);
        cfg.add_production(S, a + B);

        cfg.add_production(A, B);
        cfg.add_production(A, S);

        cfg.add_production(B, b);
        cfg.add_production(B, cfg.epsilon());

        */

        cfg.add_production(S, A + S + A);
        cfg.add_production(S, a + B);
        cfg.add_production(S, A + S + S + A);
        cfg.add_production(S, A + B + a + S + A + S + A + B);

        cfg.add_production(A, B);
        cfg.add_production(A, S);
        cfg.add_production(A, S + S);
        cfg.add_production(A, S + a);
        cfg.add_production(A, B + B);

        cfg.add_production(B, b);
        cfg.add_production(B, a);
        cfg.add_production(B, A + A);
        cfg.add_production(B, B + A + B + A + B);
        cfg.add_production(B, cfg.epsilon());


        cfg.add_production(S, S);
        cfg.add_production(S, A);
        cfg.add_production(S, B);
        cfg.add_production(A, A);
        cfg.add_production(A, B);
        cfg.add_production(A, S);
        cfg.add_production(B, S);
        cfg.add_production(B, A);
        cfg.add_production(B, B);

        CFG_TO_CNF<char>::run(cfg);
        //print_grammar(cfg);

        j += cfg.num_productions();
    }

    printf("\n\nj = %u\n", j);

    return 0;
}
