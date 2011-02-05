/*
 * main.cpp
 *
 *  Created on: Sep 14, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#include <cstdio>
#include <set>

#include "fltl/lib/CFG.hpp"

#define ON 1

#if ON

#include "fltl/test/Test.hpp"
#include "fltl/test/cfg/CFG.hpp"

/// convert a context-free grammar to chomsky normal form.
template <typename AlphaT>
static void convert_to_cnf(fltl::lib::CFG<AlphaT> &cfg) throw() {

    if(0 == cfg.num_productions()) {
        return;
    }

    // take off the templates!
    typedef fltl::lib::CFG<AlphaT> CFG;
    typedef typename CFG::variable_type variable_type;
    typedef typename CFG::production_type production_type;
    typedef typename CFG::generator_type generator_type;
    typedef typename CFG::symbol_string_type symbol_string_type;
    typedef typename CFG::production_builder_type production_builder_type;

    // add a new start variable
    variable_type old_start_var(cfg.get_start_variable());
    variable_type new_start_var(cfg.add_variable());
    cfg.set_start_variable(new_start_var);
    cfg.add_production(new_start_var, old_start_var);

    // buffer for builder productions
    production_builder_type buffer;

    symbol_string_type str;

    variable_type A;
    variable_type B;

    // go look for all productions with three or more symbols on their RHS
    // and break them into pairs of productions
    production_type P;
    generator_type long_rules(cfg.search(
        ~P,
        (~A) --->* cfg._ + cfg._ + cfg._ + cfg.__
    ));
    for(; long_rules.match_next(); ) {

        cfg.remove_production(P);

        str = P.symbols();

        unsigned i(str.length() - 2);
        variable_type prev_new_var(cfg.add_variable());
        cfg.add_production(prev_new_var, str.substring(i, 2));

        for(--i; i > 0; --i) {
            variable_type new_var(cfg.add_variable());
            cfg.add_production(new_var, str.at(i) + prev_new_var);
            prev_new_var = new_var;
        }

        cfg.add_production(A, str.at(0) + prev_new_var);
    }

    production_type null_prod;
    production_type prod_with_nullable_var;

    // generator that will find all null productions
    generator_type epsilon_rules(cfg.search(
        ~null_prod,
        (~A) --->* cfg.epsilon())
    );

    // generator that will find all productions with at
    // least one A on their RHS
    generator_type rules_with_A_on_rhs(cfg.search(
        ~prod_with_nullable_var,
        (~B) --->* cfg.__ + A + cfg.__
    ));

    std::set<variable_type> removed_epsilon;

    // while there are still epsilon rules
    for(bool added_epsilon_rule(true); added_epsilon_rule; ) {

        added_epsilon_rule = false;
        epsilon_rules.rewind();

        // go find each production A -> RHS_A where RHS_A is epsilon
        for(; epsilon_rules.match_next(); ) {

            // ignore epsilon productions on the new start variable
            if(A == new_start_var) {
                continue;
            }

            cfg.remove_production(null_prod);
            removed_epsilon.insert(A);

            // go find each production B -> RHS_B where A is in the RHS_B
            for(rules_with_A_on_rhs.rewind();
                rules_with_A_on_rhs.match_next(); ) {

                str = prod_with_nullable_var.symbols();

                // remove an illegal production
                if(1 == str.length() && A == B) {
                    cfg.remove_production(prod_with_nullable_var);
                    continue;
                }

                // count the number of A's in RHS_B, this way we can determine
                // how many different productions we will need to generate
                unsigned uses_of_A_mask(~0U);
                const unsigned num_symbols(str.length());
                for(unsigned i(0); i < num_symbols; ++i) {
                    if(A == str.at(i)) {
                        uses_of_A_mask <<= 1U;
                    }
                }

                assert(0 != uses_of_A_mask);

                // uses_of_A_mask now represents a bitmask where each 1
                // represents a use of A in RHS_B. We can decrement uses_of_A_mask
                // and inspect its bits to figure out if we should put an
                // instance of A into a production; - 1 because we will leave
                // prod_with_nullable_var, which represents the mask with all 1's,
                // alone.
                uses_of_A_mask = (~uses_of_A_mask);

                do {

                    --uses_of_A_mask;

                    buffer.clear();

                    unsigned offset_of_A(0);

                    // for each symbol in this production, either add the symbol
                    // to the buffer if the symbol is not A, or add A to the
                    // buffer if the offset_of_A'th bit of uses_of_A_mask is 1.
                    for(unsigned i(0); i < num_symbols; ++i) {
                        if(A == str.at(i)) {
                            if(1 == (1 & (uses_of_A_mask >> offset_of_A))) {
                                buffer << A;
                            }

                            ++offset_of_A;
                        } else {
                            buffer << str.at(i);
                        }
                    }

                    if(0 == buffer.size()) {

                        if(0 == removed_epsilon.count(B)) {
                            cfg.add_production(B, cfg.epsilon());
                            added_epsilon_rule = true;
                        }

                    } else if(1 == buffer.size()) {
                        if(B != buffer.symbol_at(0)) {
                            cfg.add_production(B, buffer);
                        }
                    } else {
                        cfg.add_production(B, buffer);
                    }

                } while(0 != uses_of_A_mask);
            }
        }
    }

    production_type unit_production;
    production_type prod_related_to_B;

    // generator that will find all unit productions
    generator_type unit_rules(cfg.search(
        ~unit_production,
        (~A) --->* ~B
    ));

    // generator that will find all productions related to B
    generator_type rules_with_nonempty_rhs(cfg.search(B --->* ~str));

    /// go look for all unit productions of the form A --> B, remove the
    /// production, and for every RHS such that B --> str, add A --> str
    for(bool added_unit_rhs(true); added_unit_rhs; ) {

        added_unit_rhs = false;

        //printf("\n");

        for(; unit_rules.match_next(); ) {

            cfg.remove_production(unit_production);
            //printf("removed production: "); cfg.debug(unit_production);

            // don't follow into self-loops!
            if(A == B) {
                cfg.remove_production(unit_production);
                continue;
            }

            for(rules_with_nonempty_rhs.rewind();
                rules_with_nonempty_rhs.match_next(); ) {

                // make sure not to add in any self loops
                if(1 == str.length()) {
                    if(str.at(0).is_variable()) {
                        if(A != str.at(0)) {
                            cfg.add_production(A, str);
                            added_unit_rhs = true;
                        }
                    }
                } else {
                    cfg.add_production(A, str);
                }
            }
        }
    }

    // go look for productions with two symbols, if either of the symbols
    // are terminals, then replace them with variables
    generator_type pairs(cfg.search(~P, (~A) --->* cfg._ + cfg._));
    for(; pairs.match_next(); ) {
        str = P.symbols();

        const bool first_is_term(str.at(0).is_terminal());
        const bool second_is_term(str.at(0).is_terminal());

        // two terminals
        if(first_is_term && second_is_term) {
            cfg.remove_production(P);
            buffer.clear();
            B = cfg.add_variable();
            cfg.add_production(B, str.at(0));
            buffer << B;
            B = cfg.add_variable();
            cfg.add_production(B, str.at(1));
            buffer << B;
            cfg.add_production(A, buffer);

        // first symbol is a terminal
        } else if(first_is_term) {
            cfg.remove_production(P);
            B = cfg.add_variable();
            cfg.add_production(B, str.at(0));
            cfg.add_production(A, B + str.at(0));

        // second symbol is a terminal
        } else if(second_is_term) {
            cfg.remove_production(P);
            B = cfg.add_variable();
            cfg.add_production(B, str.at(1));
            cfg.add_production(A, str.at(0) + B);
        }
    }
}

/// print a variable
template <typename A>
void print_variable(
    fltl::lib::CFG<A> &cfg,
    typename fltl::lib::CFG<A>::variable_type &var
) throw() {
    (void) cfg;
    (void) var;
}

/// print a context-free grammar
template <typename A>
void print_grammar(fltl::lib::CFG<A> &cfg) throw() {
    using fltl::lib::CFG;
    typename fltl::lib::CFG<A>::production_type P;
    typename fltl::lib::CFG<A>::variable_type S(cfg.get_start_variable());

    typename fltl::lib::CFG<A>::generator_type prods(cfg.search(~P));
    typename fltl::lib::CFG<A>::generator_type start_prods(cfg.search(
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

#if ON
    fltl::test::run_tests();
#endif

    using fltl::lib::CFG;

    unsigned j(0);

    for(unsigned i(0); i < 500000; ++i) {
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

        //print_grammar(cfg);
        //printf("\n");

        convert_to_cnf(cfg);

        j += cfg.num_productions();
        //print_grammar(cfg);
    }

    printf("j = %u\n", j);

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
