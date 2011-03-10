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

    using fltl::CFG;

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

    void test_remove_productions(void) throw() {
        CFG<char> cfg;
        CFG<char>::prod_t P;

        FLTL_TEST_DOC(CFG<char>::var_t S(cfg.add_variable()));
        FLTL_TEST_EQUAL(cfg.num_productions(), 0);

        FLTL_TEST_DOC(CFG<char>::generator_t gen(cfg.search(~P)));
        FLTL_TEST_ASSERT_FALSE(gen.match_next());

        FLTL_TEST_DOC(CFG<char>::prod_t P1(cfg.add_production(S, S)));
        FLTL_TEST_EQUAL(cfg.num_productions(), 1);

        FLTL_TEST_DOC(CFG<char>::prod_t P2(cfg.add_production(S, S + S)));
        FLTL_TEST_EQUAL(cfg.num_productions(), 2);

        FLTL_TEST_DOC(cfg.remove_production(P1));
        FLTL_TEST_EQUAL(cfg.num_productions(), 1);

        FLTL_TEST_DOC(cfg.remove_production(P2));
        FLTL_TEST_EQUAL(cfg.num_productions(), 0);
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
        FLTL_TEST_EQUAL(S_aS.symbols().at(0), a);
        FLTL_TEST_EQUAL(S_aS.symbols().at(1), S);
        FLTL_TEST_EQUAL(S_aS.variable(), S);
    }

    void test_pattern_match(void) throw() {
        CFG<char> cfg;
        CFG<char>::var_t S(cfg.add_variable());
        CFG<char>::term_t a(cfg.get_terminal('a'));
        CFG<char>::term_t b(cfg.get_terminal('b'));
        CFG<char>::term_t c(cfg.get_terminal('c'));

        CFG<char>::prod_t P0(cfg.add_production(S, cfg.epsilon()));
        CFG<char>::prod_t P1(cfg.add_production(S, a));
        CFG<char>::prod_t P2(cfg.add_production(S, b));
        CFG<char>::prod_t P3(cfg.add_production(S, c));
        CFG<char>::prod_t P4(cfg.add_production(S, a + b));
        CFG<char>::prod_t P5(cfg.add_production(S, a + c));
        CFG<char>::prod_t P6(cfg.add_production(S, b + c));
        CFG<char>::prod_t P7(cfg.add_production(S, a + b + c));
        CFG<char>::prod_t P8(cfg.add_production(S, S));
        CFG<char>::prod_t P9(cfg.add_production(S, S + S));
        CFG<char>::prod_t P10(cfg.add_production(S, a + S + b));

        // matching anything
        FLTL_TEST_ASSERT_TRUE((cfg._ --->* cfg.__).match(P0));
        FLTL_TEST_ASSERT_TRUE((cfg._ --->* cfg.__).match(P1));
        FLTL_TEST_ASSERT_TRUE((cfg._ --->* cfg.__).match(P2));
        FLTL_TEST_ASSERT_TRUE((cfg._ --->* cfg.__).match(P3));
        FLTL_TEST_ASSERT_TRUE((cfg._ --->* cfg.__).match(P4));
        FLTL_TEST_ASSERT_TRUE((cfg._ --->* cfg.__).match(P5));
        FLTL_TEST_ASSERT_TRUE((cfg._ --->* cfg.__).match(P6));
        FLTL_TEST_ASSERT_TRUE((cfg._ --->* cfg.__).match(P7));
        FLTL_TEST_ASSERT_TRUE((cfg._ --->* cfg.__).match(P8));
        FLTL_TEST_ASSERT_TRUE((cfg._ --->* cfg.__).match(P9));
        FLTL_TEST_ASSERT_TRUE((cfg._ --->* cfg.__).match(P10));

        // matching variable
        FLTL_TEST_ASSERT_TRUE((S --->* cfg.__).match(P0));
        FLTL_TEST_ASSERT_TRUE((S --->* cfg.__).match(P1));
        FLTL_TEST_ASSERT_TRUE((S --->* cfg.__).match(P2));
        FLTL_TEST_ASSERT_TRUE((S --->* cfg.__).match(P3));
        FLTL_TEST_ASSERT_TRUE((S --->* cfg.__).match(P4));
        FLTL_TEST_ASSERT_TRUE((S --->* cfg.__).match(P5));
        FLTL_TEST_ASSERT_TRUE((S --->* cfg.__).match(P6));
        FLTL_TEST_ASSERT_TRUE((S --->* cfg.__).match(P7));
        FLTL_TEST_ASSERT_TRUE((S --->* cfg.__).match(P8));
        FLTL_TEST_ASSERT_TRUE((S --->* cfg.__).match(P9));
        FLTL_TEST_ASSERT_TRUE((S --->* cfg.__).match(P10));

        // exact matches
        FLTL_TEST_ASSERT_TRUE((S --->* cfg.epsilon()).match(P0));
        FLTL_TEST_ASSERT_TRUE((S --->* a).match(P1));
        FLTL_TEST_ASSERT_TRUE((S --->* b).match(P2));
        FLTL_TEST_ASSERT_TRUE((S --->* c).match(P3));
        FLTL_TEST_ASSERT_TRUE((S --->* a + b).match(P4));
        FLTL_TEST_ASSERT_TRUE((S --->* a + c).match(P5));
        FLTL_TEST_ASSERT_TRUE((S --->* b + c).match(P6));
        FLTL_TEST_ASSERT_TRUE((S --->* a + b + c).match(P7));
        FLTL_TEST_ASSERT_TRUE((S --->* S).match(P8));
        FLTL_TEST_ASSERT_TRUE((S --->* S + S).match(P9));
        FLTL_TEST_ASSERT_TRUE((S --->* a + S + b).match(P10));

        // matching for a
        CFG<char>::pattern_t has_a(cfg._ --->* cfg.__ + a + cfg.__);
        FLTL_TEST_ASSERT_FALSE(has_a.match(P0));
        FLTL_TEST_ASSERT_TRUE(has_a.match(P1));
        FLTL_TEST_ASSERT_FALSE(has_a.match(P2));
        FLTL_TEST_ASSERT_FALSE(has_a.match(P3));
        FLTL_TEST_ASSERT_TRUE(has_a.match(P4));
        FLTL_TEST_ASSERT_TRUE(has_a.match(P5));
        FLTL_TEST_ASSERT_FALSE(has_a.match(P6));
        FLTL_TEST_ASSERT_TRUE(has_a.match(P7));
        FLTL_TEST_ASSERT_FALSE(has_a.match(P8));
        FLTL_TEST_ASSERT_FALSE(has_a.match(P9));
        FLTL_TEST_ASSERT_TRUE(has_a.match(P10));

        // inline matching for a
        FLTL_TEST_ASSERT_FALSE((cfg._ --->* cfg.__ + a + cfg.__).match(P0));
        FLTL_TEST_ASSERT_TRUE((cfg._ --->* cfg.__ + a + cfg.__).match(P1));
        FLTL_TEST_ASSERT_FALSE((cfg._ --->* cfg.__ + a + cfg.__).match(P2));
        FLTL_TEST_ASSERT_FALSE((cfg._ --->* cfg.__ + a + cfg.__).match(P3));
        FLTL_TEST_ASSERT_TRUE((cfg._ --->* cfg.__ + a + cfg.__).match(P4));
        FLTL_TEST_ASSERT_TRUE((cfg._ --->* cfg.__ + a + cfg.__).match(P5));
        FLTL_TEST_ASSERT_FALSE((cfg._ --->* cfg.__ + a + cfg.__).match(P6));
        FLTL_TEST_ASSERT_TRUE((cfg._ --->* cfg.__ + a + cfg.__).match(P7));
        FLTL_TEST_ASSERT_FALSE((cfg._ --->* cfg.__ + a + cfg.__).match(P8));
        FLTL_TEST_ASSERT_FALSE((cfg._ --->* cfg.__ + a + cfg.__).match(P9));
        FLTL_TEST_ASSERT_TRUE((cfg._ --->* cfg.__ + a + cfg.__).match(P10));

        // extract one terminal at a time
        CFG<char>::term_t t;
        FLTL_TEST_ASSERT_TRUE((cfg._ --->* cfg._ + cfg._ + ~t).match(P7));
        FLTL_TEST_EQUAL(t, c);
        FLTL_TEST_ASSERT_TRUE((cfg._ --->* cfg._ + ~t + cfg._).match(P7));
        FLTL_TEST_EQUAL(t, b);
        FLTL_TEST_ASSERT_TRUE((cfg._ --->* ~t + cfg._ + cfg._).match(P7));
        FLTL_TEST_EQUAL(t, a);

        // extract suffixes of a production
        CFG<char>::sym_str_t s;
        FLTL_TEST_ASSERT_TRUE((cfg._ --->* ~s).match(P7));
        FLTL_TEST_EQUAL(s, a + b + c);
        FLTL_TEST_ASSERT_TRUE((cfg._ --->* cfg._ + ~s).match(P7));
        FLTL_TEST_EQUAL(s, b + c);
        FLTL_TEST_ASSERT_TRUE((cfg._ --->* cfg._ + cfg._ + ~s).match(P7));
        FLTL_TEST_EQUAL(s, c);
        FLTL_TEST_ASSERT_TRUE((cfg._ --->* cfg._ + cfg._ + cfg._ + ~s).match(P7));
        FLTL_TEST_EQUAL(s, cfg.epsilon());
        FLTL_TEST_ASSERT_FALSE((cfg._ --->* cfg._ + cfg._ + cfg._ + cfg._ + ~s).match(P7));

        // extract prefixes of a production
        FLTL_TEST_ASSERT_TRUE((cfg._ --->* ~s + cfg._).match(P7));
        FLTL_TEST_EQUAL(s, a + b);
        FLTL_TEST_ASSERT_TRUE((cfg._ --->* ~s + cfg._ + cfg._).match(P7));
        FLTL_TEST_EQUAL(s, a);
        FLTL_TEST_ASSERT_TRUE((cfg._ --->* ~s + cfg._ + cfg._ + cfg._).match(P7));
        FLTL_TEST_EQUAL(s, cfg.epsilon());
        FLTL_TEST_ASSERT_FALSE((cfg._ --->* ~s + cfg._ + cfg._ + cfg._ + cfg._).match(P7));

        // extract middles
        CFG<char>::prod_t P11(cfg.add_production(S, a + b + c + S + c + b + a));
        FLTL_TEST_ASSERT_TRUE((cfg._ --->* ~s).match(P11));
        FLTL_TEST_EQUAL(s, a + b + c + S + c + b + a);
        FLTL_TEST_ASSERT_TRUE((cfg._ --->* cfg._ + ~s + cfg._).match(P11));
        FLTL_TEST_EQUAL(s, b + c + S + c + b);
        FLTL_TEST_ASSERT_TRUE((cfg._ --->* cfg._ + cfg._ + ~s + cfg._ + cfg._).match(P11));
        FLTL_TEST_EQUAL(s, c + S + c);
        FLTL_TEST_ASSERT_TRUE((cfg._ --->* cfg._ + cfg._ + cfg._ + ~s + cfg._ + cfg._ + cfg._).match(P11));
        FLTL_TEST_EQUAL(s, S);
        FLTL_TEST_ASSERT_TRUE((cfg._ --->* cfg._ + cfg._ + cfg._ + cfg._ + cfg._ + cfg._ + cfg._).match(P11));
        FLTL_TEST_ASSERT_FALSE((cfg._ --->* cfg._ + cfg._ + cfg._ + cfg._ + cfg._ + cfg._ + cfg._ + cfg._).match(P11));

        // test for checking individual symbols
        FLTL_TEST_ASSERT_TRUE((cfg._ --->* a + cfg._ + cfg._ + cfg._ + cfg._ + cfg._ + cfg._).match(P11));
        FLTL_TEST_ASSERT_TRUE((cfg._ --->* cfg._ + b + cfg._ + cfg._ + cfg._ + cfg._ + cfg._).match(P11));
        FLTL_TEST_ASSERT_TRUE((cfg._ --->* cfg._ + cfg._ + c + cfg._ + cfg._ + cfg._ + cfg._).match(P11));
        FLTL_TEST_ASSERT_TRUE((cfg._ --->* cfg._ + cfg._ + cfg._ + S + cfg._ + cfg._ + cfg._).match(P11));
        FLTL_TEST_ASSERT_TRUE((cfg._ --->* cfg._ + cfg._ + cfg._ + cfg._ + c + cfg._ + cfg._).match(P11));
        FLTL_TEST_ASSERT_TRUE((cfg._ --->* cfg._ + cfg._ + cfg._ + cfg._ + cfg._ + b + cfg._).match(P11));
        FLTL_TEST_ASSERT_TRUE((cfg._ --->* cfg._ + cfg._ + cfg._ + cfg._ + cfg._ + cfg._ + a).match(P11));
        FLTL_TEST_ASSERT_TRUE((cfg._ --->* a + b + c + S + c + b + a).match(P11));

        CFG<char>::prod_t P12(cfg.add_production(S, a + a + a));
        CFG<char>::prod_t P13(cfg.add_production(S, a + a + a + a + a + a));

        FLTL_TEST_ASSERT_TRUE((cfg._ --->* ~s + s + s).match(P12));
        FLTL_TEST_ASSERT_TRUE((cfg._ --->* ~s + s + s).match(P13));
        FLTL_TEST_ASSERT_TRUE((cfg._ --->* ~s + s).match(P13));
    }

    void test_generate_terminals(void) throw() {
        CFG<char> cfg;
        CFG<char>::term_t T;
        CFG<char>::generator_t term_gen(cfg.search(~T));

        FLTL_TEST_ASSERT_FALSE(term_gen.match_next());

        CFG<char>::term_t a(cfg.get_terminal('a'));
        CFG<char>::term_t b(cfg.get_terminal('b'));
        CFG<char>::term_t c(cfg.get_terminal('c'));
        CFG<char>::term_t d(cfg.get_terminal('d'));
        CFG<char>::term_t e(cfg.get_terminal('e'));

        FLTL_TEST_DOC(term_gen.rewind());

        FLTL_TEST_ASSERT_TRUE(term_gen.match_next());
        FLTL_TEST_EQUAL(T, a);

        FLTL_TEST_ASSERT_TRUE(term_gen.match_next());
        FLTL_TEST_EQUAL(T, b);

        FLTL_TEST_ASSERT_TRUE(term_gen.match_next());
        FLTL_TEST_EQUAL(T, c);

        FLTL_TEST_ASSERT_TRUE(term_gen.match_next());
        FLTL_TEST_EQUAL(T, d);

        FLTL_TEST_ASSERT_TRUE(term_gen.match_next());
        FLTL_TEST_EQUAL(T, e);

        FLTL_TEST_ASSERT_FALSE(term_gen.match_next());

        FLTL_TEST_DOC(term_gen.rewind());

        FLTL_TEST_ASSERT_TRUE(term_gen.match_next());
        FLTL_TEST_EQUAL(T, a);

        FLTL_TEST_ASSERT_TRUE(term_gen.match_next());
        FLTL_TEST_EQUAL(T, b);

        FLTL_TEST_ASSERT_TRUE(term_gen.match_next());
        FLTL_TEST_EQUAL(T, c);

        FLTL_TEST_ASSERT_TRUE(term_gen.match_next());
        FLTL_TEST_EQUAL(T, d);

        FLTL_TEST_ASSERT_TRUE(term_gen.match_next());
        FLTL_TEST_EQUAL(T, e);

        FLTL_TEST_ASSERT_FALSE(term_gen.match_next());

        CFG<char> cfg2;
        CFG<char>::generator_t term_gen2(cfg2.search(~T));
        FLTL_TEST_ASSERT_FALSE(term_gen2.match_next());
    }

    void test_generate_variables(void) throw() {
        CFG<char> cfg;
        CFG<char>::var_t V;
        CFG<char>::generator_t var_gen(cfg.search(~V));

        FLTL_TEST_ASSERT_FALSE(var_gen.match_next());

        CFG<char>::var_t A(cfg.add_variable());
        CFG<char>::var_t B(cfg.add_variable());
        CFG<char>::var_t C(cfg.add_variable());
        CFG<char>::var_t D(cfg.add_variable());
        CFG<char>::var_t E(cfg.add_variable());

        FLTL_TEST_DOC(var_gen.rewind());
        FLTL_TEST_ASSERT_TRUE(var_gen.match_next());
        FLTL_TEST_EQUAL(V, A);

        FLTL_TEST_ASSERT_TRUE(var_gen.match_next());
        FLTL_TEST_EQUAL(V, B);

        FLTL_TEST_ASSERT_TRUE(var_gen.match_next());
        FLTL_TEST_EQUAL(V, C);

        FLTL_TEST_ASSERT_TRUE(var_gen.match_next());
        FLTL_TEST_EQUAL(V, D);

        FLTL_TEST_ASSERT_TRUE(var_gen.match_next());
        FLTL_TEST_EQUAL(V, E);

        FLTL_TEST_ASSERT_FALSE(var_gen.match_next());

        FLTL_TEST_DOC(var_gen.rewind());

        FLTL_TEST_ASSERT_TRUE(var_gen.match_next());
        FLTL_TEST_EQUAL(V, A);

        FLTL_TEST_ASSERT_TRUE(var_gen.match_next());
        FLTL_TEST_EQUAL(V, B);

        FLTL_TEST_ASSERT_TRUE(var_gen.match_next());
        FLTL_TEST_EQUAL(V, C);

        FLTL_TEST_ASSERT_TRUE(var_gen.match_next());
        FLTL_TEST_EQUAL(V, D);

        FLTL_TEST_ASSERT_TRUE(var_gen.match_next());
        FLTL_TEST_EQUAL(V, E);

        FLTL_TEST_ASSERT_FALSE(var_gen.match_next());

        CFG<char> cfg2;
        CFG<char>::generator_t var_gen2(cfg2.search(~V));
        FLTL_TEST_ASSERT_FALSE(var_gen2.match_next());
    }

    void test_generate_productions(void) throw() {
        CFG<char> cfg;
        CFG<char>::prod_t P;
        CFG<char>::generator_t gen(cfg.search(~P));

        FLTL_TEST_ASSERT_FALSE(gen.match_next());

        CFG<char>::var_t A(cfg.add_variable());
        CFG<char>::var_t B(cfg.add_variable());
        CFG<char>::var_t C(cfg.add_variable());
        CFG<char>::var_t D(cfg.add_variable());
        CFG<char>::var_t E(cfg.add_variable());

        // test that there are no productions
        FLTL_TEST_DOC(gen.rewind());
        FLTL_TEST_ASSERT_FALSE(gen.match_next());

        CFG<char>::prod_t p[3];
        bool p_seen[3] = {false};

        p[0] = cfg.add_production(B, A);
        p[1] = cfg.add_production(D, A + A);
        p[2] = cfg.add_production(D, A + A + A);

        FLTL_TEST_DOC(gen.rewind());
        FLTL_TEST_ASSERT_TRUE(gen.match_next());
        FLTL_TEST_ASSERT_TRUE(gen.match_next());
        FLTL_TEST_ASSERT_TRUE(gen.match_next());
        FLTL_TEST_ASSERT_FALSE(gen.match_next());

        for(gen.rewind(); gen.match_next(); ) {
            for(unsigned i(0); i < 3; ++i) {
                if(P == p[i]) {
                    p_seen[i] = true;
                }
            }
        }

        FLTL_TEST_ASSERT_TRUE(p_seen[0]);
        FLTL_TEST_ASSERT_TRUE(p_seen[1]);
        FLTL_TEST_ASSERT_TRUE(p_seen[2]);
    }

    void test_generate_search(void) throw() {

    }
}}}
