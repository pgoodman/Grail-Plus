/*
 * main.cpp
 *
 *  Created on: Sep 14, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#include <cstdio>

#include "fltl/lib/CFG.hpp"
#include "fltl/test/Test.hpp"
#include "fltl/test/cfg/CFG.hpp"
#if 0
template <typename A>
void convert_to_cnf(fltl::lib::CFG<A> &cfg) throw() {

    using fltl::lib::CFG;

    // add a new start variable
    CFG<A>::variable_type old_start_var(cfg.get_start_variable());
    CFG<A>::variable_type new_start_var(cfg.add_variable());
    cfg.set_start_variable(new_start_var);
    cfg.add_production(new_start_var, old_start_var);

    // go find epsilon rules
    CFG<A>::variable_type A;
    CFG<A>::production_type null_prod;
    CFG<A>::variable_type B;
    CFG<A>::production_type prod_with_nullable_var;

    // generator that will find all null productions
    CFG<A>::generator_type epsilon_rules(cfg.search(
        ~null_prod,
        ~A --->* cfg.epsilon())
    );

    // generator that will find all productions with at
    // least one A on their RHS
    CFG<A>::generator_type rules_with_A_on_rhs(cfg.search(
        ~prod_with_nullable_var,
        ~B --->* cfg.__ + A + cfg.__
    ));

    // go find each production A -> RHS_A where RHS_A is epsilon
    for(; epsilon_rules.match_next(); rules_with_A_on_rhs.rewind()) {

        // go find each production B -> RHS_B where A is in the RHS_B
        for(; rules_with_A_on_rhs.match_next(); ) {
            cfg.remove_production(prod_with_nullable_var);

            CFG<A>::symbol_string_type str(prod_with_nullable_var.symbols());

            // count the number of A's in RHS_B, this way we can determine
            // how many different productions we will need to generate
            const unsigned num_As(0);
            for(unsigned i(0); i < str.length(); ++i) {
                if(A == str.at(i)) {
                    ++num_As;
                }
            }

            assert(32 >= num_As);
        }
    }
}

template <typename A>
void print_grammar(fltl::lib::CFG<A> &cfg) throw() {
    using fltl::lib::CFG;
}
#endif

int main(void) {

    fltl::test::run_tests();
#if 0
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
#endif
    return 0;
}
