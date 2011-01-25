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

    void test_equivalence_relations(void) throw() {

        using fltl::lib::CFG;

        CFG<char> cfg;

        CFG<char>::var_t S(cfg.add_variable());
        CFG<char>::var_t T(cfg.add_variable());

        CFG<char>::term_t a(cfg.get_terminal('a'));
        CFG<char>::term_t b(cfg.get_terminal('b'));
        CFG<char>::term_t a_prime(cfg.get_terminal('a'));

        CFG<char>::sym_t epsilon(cfg.epsilon());
        CFG<char>::sym_t epsilon_prime;

        CFG<char>::sym_t S_sym(S);
        CFG<char>::sym_t a_sym(a);

        FLTL_TEST_EQUAL(S,S);
        FLTL_TEST_EQUAL(S, S_sym);
        FLTL_TEST_EQUAL(S_sym, S);
        FLTL_TEST_NOT_EQUAL(S, T);
        FLTL_TEST_NOT_EQUAL(S, a);
        FLTL_TEST_NOT_EQUAL(a, S);

        FLTL_TEST_EQUAL(a, a);
        FLTL_TEST_EQUAL(a, a_sym);
        FLTL_TEST_EQUAL(a_sym, a);
        FLTL_TEST_EQUAL(a, a_prime);
        FLTL_TEST_EQUAL(a_prime, a);

        FLTL_TEST_NOT_EQUAL(a, b);
        FLTL_TEST_NOT_EQUAL(b, a);

        FLTL_TEST_EQUAL(epsilon, epsilon);
        FLTL_TEST_EQUAL(epsilon, epsilon_prime);
        FLTL_TEST_EQUAL(epsilon_prime, epsilon);
        FLTL_TEST_EQUAL(epsilon_prime, epsilon_prime);

        FLTL_TEST_EQUAL((a + a), (a + a));
        FLTL_TEST_EQUAL((a + b), (a + b));
        FLTL_TEST_EQUAL((b + a), (b + a));
        FLTL_TEST_EQUAL((S + S), (S + S));
        FLTL_TEST_EQUAL((S + epsilon), (S + epsilon));
        FLTL_TEST_EQUAL((epsilon + S), (S + epsilon));
        FLTL_TEST_EQUAL((S + epsilon), (epsilon + S));
        FLTL_TEST_EQUAL(S, (S + epsilon));
        FLTL_TEST_EQUAL(S, (epsilon + S));
        FLTL_TEST_EQUAL((S + epsilon), S);
        FLTL_TEST_EQUAL((epsilon + S), S);

        FLTL_TEST_NOT_EQUAL((a + a), (a + b));
        FLTL_TEST_NOT_EQUAL((a + a), (b + a));
        FLTL_TEST_NOT_EQUAL((a + b), (a + a));
        FLTL_TEST_NOT_EQUAL((b + a), (a + a));
        FLTL_TEST_NOT_EQUAL((a + S), (S + a));
        FLTL_TEST_NOT_EQUAL((S + a), (a + S));
    }

    void test_productions(void) throw() {

    }

}}}
