/*
 * CFG_REMOVE_EPSILON.hpp
 *
 *  Created on: Feb 15, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_CFG_REMOVE_EPSILON_HPP_
#define FLTL_CFG_REMOVE_EPSILON_HPP_

#include <cassert>
#include <set>

#include "fltl/lib/CFG.hpp"

#include "grail/include/Algorithm.hpp"

namespace grail { namespace algorithm {

    /// remove all epsilon productions from a context-free grammar, *except*
    /// for epsilon productions in the start variable.
    template <typename AlphaT>
    class CFG_REMOVE_EPSILON {
    public:

        // take off the templates!
        typedef fltl::lib::CFG<AlphaT> CFG;
        typedef typename CFG::variable_type variable_type;
        typedef typename CFG::terminal_type terminal_type;
        typedef typename CFG::production_type production_type;
        typedef typename CFG::generator_type generator_type;
        typedef typename CFG::symbol_string_type symbol_string_type;
        typedef typename CFG::production_builder_type production_builder_type;

        /// remove all rules with epsilons on their right-hand-sides (RHS), except
        /// in the case that the new start variable has the epsilon
        static void run(CFG &cfg) throw() {

            const variable_type start_var(cfg.get_start_variable());

            // buffer for builder productions
            production_builder_type buffer;

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
                    if(A == start_var) {
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

                            // add in the buffered production
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
        }
    };
}}

#endif /* FLTL_CFG_REMOVE_EPSILON_HPP_ */
