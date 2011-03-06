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
#include <utility>

#include "fltl/include/CFG.hpp"

#include "grail/algorithm/CFG_REMOVE_DIRECT_LOOPS.hpp"

#include "grail/include/io/fprint_cfg.hpp"

namespace grail { namespace algorithm {

    /// remove all epsilon productions from a context-free grammar, *except*
    /// for epsilon productions in the start variable.
    template <typename AlphaT>
    class CFG_REMOVE_EPSILON {
    private:

        // take off the templates!
        typedef fltl::CFG<AlphaT> CFG;
        typedef typename CFG::variable_type variable_type;
        typedef typename CFG::terminal_type terminal_type;
        typedef typename CFG::production_type production_type;
        typedef typename CFG::generator_type generator_type;
        typedef typename CFG::symbol_string_type symbol_string_type;
        typedef typename CFG::production_builder_type production_builder_type;

        static void add_new_start_var(CFG &cfg) throw() {
            variable_type S(cfg.add_variable());
            cfg.add_production(S, cfg.get_start_variable());
            cfg.set_start_variable(S);
        }

        static bool sub_epsilon_combinations(
            CFG &cfg,
            production_builder_type &buffer,
            symbol_string_type &str,
            std::set<variable_type> &ignore_set,
            variable_type A,
            variable_type B
        ) throw() {

            bool updated(false);

            // count the number of A's in RHS_B, this way we can determine
            // how many different productions we will need to generate
            unsigned long uses_of_A_mask(~0UL);
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

                if(1 == buffer.size()) {

                    // don't introduce self-loops
                    if(B == buffer.symbol_at(0)) {
                        continue;
                    }
                } else if(0 == buffer.size()) {
                    if(0 == ignore_set.count(B)) {
                        updated = true;
                    } else {
                        continue;
                    }
                }

                cfg.add_production(B, buffer);

            } while(0 != uses_of_A_mask);

            return updated;
        }

    public:

        /// remove all rules with epsilons on their right-hand-sides (RHS), except
        /// in the case that the new start variable has the epsilon
        static void run(CFG &cfg) throw() {

            CFG_REMOVE_DIRECT_LOOPS<AlphaT>::run(cfg);
            add_new_start_var(cfg);

            production_type null_production;
            variable_type A;

            variable_type S(cfg.get_start_variable());

            generator_type null_productions(cfg.search(
                ~null_production,
                (~A) --->* cfg.epsilon()
            ));

            variable_type B;
            production_type prod_with_nullable_var;
            symbol_string_type str;
            generator_type prods_with_nullable_var(cfg.search(
                ~prod_with_nullable_var,
                (~B) --->* cfg.__ + A + cfg.__
            ));

            // buffer for building productions
            production_builder_type buffer;

            std::set<variable_type> ignore_set;

            bool updated(true);

            for(; updated; null_productions.rewind()) {

                updated = false;

                for(; null_productions.match_next(); ) {

                    // accept null in the start variable
                    if(S == A) {
                        continue;
                    }

                    cfg.remove_production(null_production);
                    ignore_set.insert(A);

                    for(prods_with_nullable_var.rewind();
                        prods_with_nullable_var.match_next(); ) {

                        str = prod_with_nullable_var.symbols();
                        updated = sub_epsilon_combinations(
                            cfg,
                            buffer,
                            str,
                            ignore_set,
                            A,
                            B
                        ) || updated;
                    }
                }

                if(cfg.num_productions() > 100) {
                    return;
                }
            }

            generator_type non_null_productions(cfg.search(
                A --->* cfg._ + cfg.__
            ));

            // clean up any left over productions that only have
            for(; null_productions.match_next(); ) {
                if(A == S) {
                    continue;
                }

                non_null_productions.rewind();
                assert(!non_null_productions.match_next());
                cfg.remove_variable(A);
            }
        }
    };
}}

#endif /* FLTL_CFG_REMOVE_EPSILON_HPP_ */
