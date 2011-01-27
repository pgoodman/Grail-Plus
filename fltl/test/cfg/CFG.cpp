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

        CFG<char>::sym_t epsilon(cfg.epsilon());
        CFG<char>::sym_t epsilon_prime;

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
        CFG<char>::sym_t epsilon(cfg.epsilon());

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

    void test_productions(void) throw() {

        CFG<char> cfg;
        (void) cfg;
    }

}}}
