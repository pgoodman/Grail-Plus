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

template <typename A>
void print_grammar(fltl::lib::CFG<A> &);

template <typename AlphaT>
class CFG_TO_CNF {

    // take off the templates!
    typedef fltl::lib::CFG<AlphaT> CFG;
    typedef typename CFG::variable_type variable_type;
    typedef typename CFG::terminal_type terminal_type;
    typedef typename CFG::production_type production_type;
    typedef typename CFG::generator_type generator_type;
    typedef typename CFG::symbol_string_type symbol_string_type;
    typedef typename CFG::production_builder_type production_builder_type;

private:

    inline static void add_production_from_buffer(
        CFG &cfg,
        production_builder_type &buffer,
        std::set<variable_type> &removed_epsilon,
        variable_type &B,
        bool &added_epsilon_rule
    ) throw() {
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
    }

    /// go look for all productions with three or more symbols on their RHS
    /// and break them into pairs of productions
    inline static void remove_long_productions(CFG &cfg) throw() {

        variable_type A;
        symbol_string_type str;
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
    }

    /// remove all rules with epsilons on their right-hand-sides (RHS), except
    /// in the case that the new start variable has the epsilon
    inline static void remove_epsilon_rules(
        CFG &cfg,
        variable_type new_start_var,
        production_builder_type &buffer
    ) throw() {

        symbol_string_type str;
        variable_type A;
        variable_type B;
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
        unsigned old_num_productions(0);
        for(bool added_epsilon_rule(true); added_epsilon_rule; ) {

            added_epsilon_rule = false;
            epsilon_rules.rewind();

            // go find each production A -> RHS_A where RHS_A is epsilon
            for(; epsilon_rules.match_next(); ) {

                // ignore epsilon productions on the new start variable
                if(A == new_start_var) {
                    continue;
                }

                old_num_productions = cfg.num_productions();
                cfg.remove_production(null_prod);
                removed_epsilon.insert(A);

                // remove A, we just deleted its only production which was the
                // epsilon production, remove the variable entirely
                if(old_num_productions == cfg.num_productions()) {
                    cfg.remove_variable(A);
                    continue;
                }

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

                        add_production_from_buffer(
                            cfg,
                            buffer,
                            removed_epsilon,
                            B,
                            added_epsilon_rule
                        );

                    } while(0 != uses_of_A_mask);
                }
            }
        }
    }

    inline static void remove_unit_productions(
        CFG &cfg,
        variable_type start_var
    ) throw() {

        symbol_string_type str;
        variable_type A;
        variable_type B;
        production_type unit_production;
        production_type prod_related_to_B;
        production_type P;

        // generator that will find all unit productions
        generator_type unit_rules(cfg.search(
            ~unit_production,
            (~A) --->* ~B
        ));

        // generator that will find all productions related to B where there
        // is at least one symbol in the string
        generator_type rules_with_nonempty_rhs(cfg.search(
            ~P,
            B --->* cfg._ + cfg.__
        ));

        bool found_self_loops(false);

        /// go look for all unit productions of the form A --> B, remove the
        /// production, and for every RHS such that B --> str, add A --> str
        for(bool added_unit_rhs(true); added_unit_rhs; ) {

            added_unit_rhs = false;

            for(unit_rules.rewind(); unit_rules.match_next(); ) {

                cfg.remove_production(unit_production);

                // don't follow into self-loops!
                if(A == B) {
                    found_self_loops = true;
                    continue;
                }

                for(rules_with_nonempty_rhs.rewind();
                    rules_with_nonempty_rhs.match_next(); ) {

                    str = P.symbols();

                    // make sure not to add in any self loops
                    if(1 == str.length()) {
                        if(str.at(0).is_variable()) {
                            if(A != str.at(0)) {
                                cfg.add_production(A, str);
                                added_unit_rhs = true;
                            } else {
                                found_self_loops = true;
                            }
                        }
                    } else {
                        cfg.add_production(A, str);
                    }
                }
            }
        }

        // some self-loops were detected; they might have inadvertently
        // introduced some epsilons, so go and remove them
        if(found_self_loops) {
            production_type null_prod;
            generator_type null_prods(cfg.search(
                ~null_prod,
                (~A) --->* cfg.epsilon()
            ));

            for(; null_prods.match_next(); ) {

                if(A == start_var) {
                    if(null_prod.was_automatically_made()) {
                        cfg.remove_variable(A);
                    }
                } else {
                    const unsigned num_prods(cfg.num_productions());
                    cfg.remove_production(null_prod);
                    if(num_prods == cfg.num_productions()) {
                        cfg.remove_variable(A);
                    }
                }
            }
        }
    }

    // go look for productions with two symbols, if either of the symbols
        // are terminals, then replace them with variables
    inline static void clean_up_terminals(
        CFG &cfg,
        std::map<terminal_type, variable_type> &terminal_rules,
        std::map<variable_type, variable_type> &vars_to_replace
    ) throw() {

        production_type P;
        symbol_string_type str;
        variable_type A;
        variable_type B;
        terminal_type T;

        generator_type pairs(cfg.search(~P, cfg._ --->* cfg._ + cfg._));
        for(; pairs.match_next(); ) {
            str = P.symbols();

            const bool first_is_term(str.at(0).is_terminal());
            const bool second_is_term(str.at(1).is_terminal());

            if(first_is_term) {
                T = str.at(0);
                if(0 == terminal_rules.count(T)) {
                    A = cfg.add_variable();
                    cfg.add_production(A, T);
                    terminal_rules[T] = A;
                } else {
                    A = terminal_rules[T];
                }
            }

            if(second_is_term) {
                T = str.at(1);
                if(0 == terminal_rules.count(T)) {
                    B = cfg.add_variable();
                    cfg.add_production(B, T);
                    terminal_rules[T] = B;
                } else {
                    B = terminal_rules[T];
                }
            }

            // two terminals
            if(first_is_term && second_is_term) {
                cfg.remove_production(P);
                cfg.add_production(P.variable(), A + B);

            // first symbol is a terminal
            } else if(first_is_term) {
                cfg.remove_production(P);
                B = str.at(1);
                if(vars_to_replace.count(B)) {
                    B = vars_to_replace[B];
                }
                cfg.add_production(P.variable(), A + B);

            // second symbol is a terminal
            } else if(second_is_term) {
                cfg.remove_production(P);
                A = str.at(0);
                if(vars_to_replace.count(A)) {
                    A = vars_to_replace[A];
                }
                cfg.add_production(P.variable(), A + B);
            }
        }
    }

public:

    /// convert a context-free grammar to chomsky normal form.
    static void run(CFG &cfg) throw() {

        if(0 == cfg.num_productions()) {
            return;
        }

        printf("\ninput:\n");
        print_grammar(cfg);

        // add a new start variable
        variable_type old_start_var(cfg.get_start_variable());
        variable_type new_start_var(cfg.add_variable());
        cfg.set_start_variable(new_start_var);
        cfg.add_production(new_start_var, old_start_var);

        remove_long_productions(cfg);

        printf("\nstep 1:\n");
        print_grammar(cfg);

        // buffer for builder productions
        production_builder_type buffer;
        remove_epsilon_rules(cfg, new_start_var, buffer);

        printf("\nstep 2:\n");
        print_grammar(cfg);

        remove_unit_productions(cfg, new_start_var);

        printf("\nstep 3:\n");
        print_grammar(cfg);

        // keep track of those productions that only generate a terminal
        std::map<terminal_type, variable_type> terminal_rules;
        std::map<variable_type, variable_type> vars_to_replace;

        terminal_type T;
        production_type P;
        variable_type A;
        variable_type B;

        generator_type terminal_units(cfg.search(~P, (~A) --->* T));

        for(; terminal_units.match_next(); ) {
            if(1 == cfg.num_productions(A)) {
                if(1 == terminal_rules.count(T)) {
                    vars_to_replace[A] = terminal_rules[T];
                    cfg.remove_relation(A);
                } else {
                    terminal_rules[T] = A;
                }
            }
        }

        clean_up_terminals(cfg, terminal_rules, vars_to_replace);

        printf("\nstep 4:\n");
        print_grammar(cfg);
    }
};

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

    fltl::test::run_tests();

    using fltl::lib::CFG;

    unsigned j(0);

    for(unsigned i(0); i < 1; ++i) {
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

        /*
        //cfg.add_production(S, S);
        cfg.add_production(S, A);
        cfg.add_production(A, B);
        //cfg.add_production(A, S);
        //cfg.add_production(B, cfg.epsilon());
        //cfg.add_production(B, A);
        cfg.add_production(B, S);

        //print_grammar(cfg);
        //printf("\n");
        */
        CFG_TO_CNF<char>::run(cfg);

        j += cfg.num_productions();
        //print_grammar(cfg);
    }

    printf("\n\nj = %u\n", j);

    return 0;
}
