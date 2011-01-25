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

        FLTL_TEST_ASSERT(S == S, "Variables are equivalent to themselves (1).");
        FLTL_TEST_ASSERT(!(S != S), "Variables are equivalent to themselves (2).");
        FLTL_TEST_ASSERT(S == S_sym, "Variables are equivalent to themselves (3).");
        FLTL_TEST_ASSERT(S_sym == S, "Variables are equivalent to themselves (4).");
        FLTL_TEST_ASSERT(!(S != S_sym), "Variables are equivalent to themselves (5).");
        FLTL_TEST_ASSERT(!(S_sym != S), "Variables are equivalent to themselves (6).");

        FLTL_TEST_ASSERT(S != T, "Distinct variables are not equivalent (1).");
        FLTL_TEST_ASSERT(!(S == T), "Distinct variables are not equivalent (2).");

        FLTL_TEST_ASSERT(S != a, "Variables and terminals are not equivalent (lhs; 1).");
        FLTL_TEST_ASSERT(!(S == a), "Variables and terminals are not not equivalent (lhs; 2).");

        FLTL_TEST_ASSERT(a != S, "Variables and terminals are not equivalent (rhs; 1).");
        FLTL_TEST_ASSERT(!(a == S), "Variables and terminals are not not equivalent (rhs; 2).");

        FLTL_TEST_ASSERT(a == a, "Terminals are equivalent to themselves (1).");
        FLTL_TEST_ASSERT(!(a != a), "Terminals are equivalent to themselves (2).");
        FLTL_TEST_ASSERT(a == a_sym, "Terminals are equivalent to themselves (3).");
        FLTL_TEST_ASSERT(!(a != a_sym), "Terminals are equivalent to themselves (4).");
        FLTL_TEST_ASSERT(a_sym == a, "Terminals are equivalent to themselves (5).");
        FLTL_TEST_ASSERT(!(a != a_sym), "Terminals are equivalent to themselves (6).");
        FLTL_TEST_ASSERT(a == a_prime, "Terminals are equivalent to themselves (7).");
        FLTL_TEST_ASSERT(!(a != a_prime), "Terminals are equivalent to themselves (8).");

        FLTL_TEST_ASSERT(a != b, "Distinct terminals are not equivalent (1).");
        FLTL_TEST_ASSERT(!(a == b), "Distinct terminals are not equivalent (2).");
        FLTL_TEST_ASSERT(b != a, "Distinct terminals are not equivalent (3).");
        FLTL_TEST_ASSERT(!(b == a), "Distinct terminals are not equivalent (4).");

        FLTL_TEST_ASSERT(epsilon == epsilon, "Epsilon is equivalent to itself (1).");
        FLTL_TEST_ASSERT(!(epsilon != epsilon), "Epsilon is equivalent to itself (2).");
        FLTL_TEST_ASSERT(epsilon == epsilon_prime, "Epsilon is equivalent to itself (3).");
        FLTL_TEST_ASSERT(!(epsilon != epsilon_prime), "Epsilon is equivalent to itself (4).");
        FLTL_TEST_ASSERT(epsilon_prime == epsilon, "Epsilon is equivalent to itself (5).");
        FLTL_TEST_ASSERT(!(epsilon_prime != epsilon), "Epsilon is equivalent to itself (6).");
        FLTL_TEST_ASSERT(epsilon_prime == epsilon_prime, "Epsilon is equivalent to itself (7).");
        FLTL_TEST_ASSERT(!(epsilon_prime != epsilon_prime), "Epsilon is equivalent to itself (8).");
    }

    void test_productions(void) throw() {

    }

}}}
