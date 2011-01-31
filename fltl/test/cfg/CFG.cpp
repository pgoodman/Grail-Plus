/*
 * CFG.cpp
 *
 *  Created on: Jan 24, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#include "fltl/test/cfg/CFG.hpp"

namespace fltl { namespace test { namespace cfg {

    using fltl::lib::CFG;

    void test_equivalence_relations(void) throw() {

        CFG<char> cfg;

        CFG<char>::var_t S(cfg.add_variable());
        CFG<char>::var_t T(cfg.add_variable());

        CFG<char>::term_t a(cfg.get_terminal('a'));
        CFG<char>::term_t b(cfg.get_terminal('b'));
        CFG<char>::term_t a_prime(cfg.get_terminal('a'));

        CFG<char>::sym_str_t epsilon(cfg.epsilon());
        CFG<char>::sym_str_t epsilon_prime;

        CFG<char>::sym_t S_sym(S);
        CFG<char>::sym_t a_sym(a);

        FLTL_TEST_EQUAL_REL(S,S);
        FLTL_TEST_EQUAL_REL(S, S_sym);
        FLTL_TEST_EQUAL_REL(S_sym, S);
        FLTL_TEST_NOT_EQUAL_REL(S, T);
        FLTL_TEST_NOT_EQUAL_REL(S, a);
        FLTL_TEST_NOT_EQUAL_REL(a, S);

        FLTL_TEST_EQUAL_REL(a, a);
        FLTL_TEST_EQUAL_REL(a, a_sym);
        FLTL_TEST_EQUAL_REL(a_sym, a);
        FLTL_TEST_EQUAL_REL(a, a_prime);
        FLTL_TEST_EQUAL_REL(a_prime, a);

        FLTL_TEST_NOT_EQUAL_REL(a, b);
        FLTL_TEST_NOT_EQUAL_REL(b, a);

        FLTL_TEST_EQUAL_REL(epsilon, epsilon);
        FLTL_TEST_EQUAL_REL(epsilon, epsilon_prime);
        FLTL_TEST_EQUAL_REL(epsilon_prime, epsilon);
        FLTL_TEST_EQUAL_REL(epsilon_prime, epsilon_prime);
        FLTL_TEST_EQUAL_REL(epsilon, (epsilon + epsilon));
        FLTL_TEST_EQUAL_REL((epsilon + epsilon), epsilon);
        FLTL_TEST_EQUAL_REL((epsilon + epsilon), (epsilon + epsilon));

        FLTL_TEST_EQUAL_REL((a + a), (a + a));
        FLTL_TEST_EQUAL_REL((a + b), (a + b));
        FLTL_TEST_EQUAL_REL((b + a), (b + a));
        FLTL_TEST_EQUAL_REL((S + S), (S + S));
        FLTL_TEST_EQUAL_REL((S + epsilon + S), (S + S));
        FLTL_TEST_EQUAL_REL((S + S), (S + epsilon + S));
        FLTL_TEST_EQUAL_REL((S + epsilon), (S + epsilon));
        FLTL_TEST_EQUAL_REL((epsilon + S), (S + epsilon));
        FLTL_TEST_EQUAL_REL((S + epsilon), (epsilon + S));
        FLTL_TEST_EQUAL_REL(S, (S + epsilon));
        FLTL_TEST_EQUAL_REL(S, (epsilon + S));
        FLTL_TEST_EQUAL_REL((S + epsilon), S);
        FLTL_TEST_EQUAL_REL((epsilon + S), S);
        FLTL_TEST_EQUAL_REL(((S + a) + S), (S + (a + S)));

        FLTL_TEST_EQUAL_REL((S + a + a + S), (S + a + a + S));
        FLTL_TEST_EQUAL_REL(((S + a) + a + S), (S + a + a + S));
        FLTL_TEST_EQUAL_REL((S + (a + a) + S), (S + a + a + S));
        FLTL_TEST_EQUAL_REL((S + a + (a + S)), (S + a + a + S));
        FLTL_TEST_EQUAL_REL(((S + a) + (a + S)), (S + a + a + S));
        FLTL_TEST_EQUAL_REL((S + ((a + a) + S)), (S + a + a + S));

        FLTL_TEST_NOT_EQUAL_REL((a + a), (a + b));
        FLTL_TEST_NOT_EQUAL_REL((a + a), (b + a));
        FLTL_TEST_NOT_EQUAL_REL((a + b), (a + a));
        FLTL_TEST_NOT_EQUAL_REL((b + a), (a + a));
        FLTL_TEST_NOT_EQUAL_REL((a + S), (S + a));
        FLTL_TEST_NOT_EQUAL_REL((S + a), (a + S));
    }

    void test_string_lengths(void) throw() {
        CFG<char> cfg;
        CFG<char>::var_t S(cfg.add_variable());
        CFG<char>::term_t a(cfg.get_terminal('a'));
        CFG<char>::sym_str_t epsilon(cfg.epsilon());

        FLTL_TEST_EQUAL(S.length(), 1);
        FLTL_TEST_EQUAL((epsilon + S).length(), 1);
        FLTL_TEST_EQUAL((S + epsilon).length(), 1);

        FLTL_TEST_EQUAL(a.length(), 1);
        FLTL_TEST_EQUAL((epsilon + a).length(), 1);
        FLTL_TEST_EQUAL((a + epsilon).length(), 1);

        FLTL_TEST_EQUAL(epsilon.length(), 0);
        FLTL_TEST_EQUAL((epsilon + epsilon).length(), 0);
        FLTL_TEST_EQUAL((epsilon + epsilon + epsilon).length(), 0);

        FLTL_TEST_EQUAL((S + S).length(), 2);
        FLTL_TEST_EQUAL((a + a).length(), 2);
        FLTL_TEST_EQUAL((S + a).length(), 2);
        FLTL_TEST_EQUAL((a + S).length(), 2);

        FLTL_TEST_EQUAL((S + S + epsilon).length(), 2);
        FLTL_TEST_EQUAL((a + a + epsilon).length(), 2);
        FLTL_TEST_EQUAL((S + a + epsilon).length(), 2);
        FLTL_TEST_EQUAL((a + S + epsilon).length(), 2);
    }

    void test_add_productions(void) throw() {

        CFG<char> cfg;

        FLTL_TEST_EQUAL(cfg.num_productions(), 0);
        FLTL_TEST_EQUAL(cfg.num_terminals(), 0);
        FLTL_TEST_EQUAL(cfg.num_variables(), 0);

        FLTL_TEST_DOC(CFG<char>::var_t S(cfg.add_variable()));

        FLTL_TEST_EQUAL(cfg.num_productions(), 0);
        FLTL_TEST_EQUAL(cfg.num_terminals(), 0);
        FLTL_TEST_EQUAL(cfg.num_variables(), 1);

        FLTL_TEST_DOC(cfg.add_production(S, cfg.epsilon()));

        FLTL_TEST_EQUAL(cfg.num_productions(), 1);
        FLTL_TEST_EQUAL(cfg.num_terminals(), 0);
        FLTL_TEST_EQUAL(cfg.num_variables(), 1);

        FLTL_TEST_DOC(CFG<char>::term_t a(cfg.get_terminal('a')));

        FLTL_TEST_EQUAL(cfg.num_productions(), 1);
        FLTL_TEST_EQUAL(cfg.num_terminals(), 1);
        FLTL_TEST_EQUAL(cfg.num_variables(), 1);

        FLTL_TEST_DOC(cfg.add_production(S, S));
        FLTL_TEST_DOC(cfg.add_production(S, a));
        FLTL_TEST_DOC(cfg.add_production(S, S + a));

        FLTL_TEST_EQUAL(cfg.num_productions(), 4);
        FLTL_TEST_EQUAL(cfg.num_terminals(), 1);
        FLTL_TEST_EQUAL(cfg.num_variables(), 1);

        CFG<char>::prod_builder_t builder;

        FLTL_TEST_DOC(cfg.add_production(S, builder.clear() << a << S));

        FLTL_TEST_EQUAL(cfg.num_productions(), 5);
        FLTL_TEST_EQUAL(cfg.num_terminals(), 1);
        FLTL_TEST_EQUAL(cfg.num_variables(), 1);

        FLTL_TEST_DOC(cfg.add_production(S, builder.clear() << a << S));
        FLTL_TEST_DOC(cfg.add_production(S, builder.clear() << S << a));
        FLTL_TEST_DOC(cfg.add_production(S, S + a));
        FLTL_TEST_DOC(cfg.add_production(S, a + S));

        FLTL_TEST_EQUAL(cfg.num_productions(), 5);
        FLTL_TEST_EQUAL(cfg.num_terminals(), 1);
        FLTL_TEST_EQUAL(cfg.num_variables(), 1);

        CFG<char>::prod_t p(cfg.add_production(S, builder.clear() << a << S));
        CFG<char>::prod_t p_prime(cfg.add_production(S, builder.clear() << a << S));
        CFG<char>::prod_t not_p(cfg.add_production(S, builder.clear() << S << a));

        FLTL_TEST_EQUAL_REL(p, p_prime);
        FLTL_TEST_NOT_EQUAL_REL(p, not_p);
        FLTL_TEST_NOT_EQUAL_REL(p_prime, not_p);

        CFG<char>::sym_str_t p_str(a + S);
        CFG<char>::sym_str_t not_p_str(S + a);

        FLTL_TEST_EQUAL_REL(p.symbols(), p_str);
        FLTL_TEST_NOT_EQUAL_REL(p.symbols(), not_p_str);

        FLTL_TEST_NOT_EQUAL_REL(not_p.symbols(), p_str);
        FLTL_TEST_EQUAL_REL(not_p.symbols(), not_p_str);
    }

    void test_extract_symbols(void) throw() {
        CFG<char> cfg;
        CFG<char>::var_t S(cfg.add_variable());
        CFG<char>::term_t a(cfg.get_terminal('a'));
        CFG<char>::sym_str_t epsilon(cfg.epsilon());

        CFG<char>::sym_str_t aS(a + S);
        CFG<char>::prod_t S_aS(cfg.add_production(S, a + S));

        FLTL_TEST_EQUAL(aS[0], a);
        FLTL_TEST_EQUAL(aS[1], S);
        FLTL_TEST_EQUAL(aS.at(0), a);
        FLTL_TEST_EQUAL(aS.at(1), S);
        FLTL_TEST_EQUAL(aS.substring(0, 2), aS);
        FLTL_TEST_EQUAL(aS.substring(1, 1), S);
        FLTL_TEST_EQUAL(aS.substring(0, 1), a);
        FLTL_TEST_EQUAL(aS.substring(0, 0), epsilon);
        FLTL_TEST_EQUAL(aS.substring(1, 0), epsilon);

        FLTL_TEST_EQUAL(S_aS.symbols(), aS);
        FLTL_TEST_EQUAL(S_aS.symbol_at(0), a);
        FLTL_TEST_EQUAL(S_aS.symbol_at(1), S);
        FLTL_TEST_EQUAL(S_aS.variable(), S);
    }
}}}
