/*
 * CFG_REMOVE_UNITS.hpp
 *
 *  Created on: Feb 15, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_CFG_REMOVE_UNITS_HPP_
#define FLTL_CFG_REMOVE_UNITS_HPP_

#include <cassert>
#include <set>
#include <utility>

#include "fltl/include/CFG.hpp"

namespace grail { namespace algorithm {

    /// remove unit productions from a context-free grammar.
    template <typename AlphaT>
    class CFG_REMOVE_UNITS {
    public:

        // take off the templates!
        typedef fltl::CFG<AlphaT> CFG;
        typedef typename CFG::variable_type variable_type;
        typedef typename CFG::terminal_type terminal_type;
        typedef typename CFG::production_type production_type;
        typedef typename CFG::generator_type generator_type;
        typedef typename CFG::symbol_string_type symbol_string_type;
        typedef typename CFG::production_builder_type production_builder_type;

        static void run(CFG &cfg) throw() {

            std::pair<variable_type,variable_type> elim_key;
            std::set<std::pair<variable_type,variable_type> > eliminated;

            production_type unit_production;
            variable_type A;
            variable_type B;
            generator_type unit_productions(cfg.search(
                ~unit_production,
                (~A) --->* ~B
            ));

            symbol_string_type gen_string;
            generator_type related_productions(cfg.search(
                B --->* ~gen_string
            ));

            bool updated(true);

            for(; updated; ) {
                updated = false;

                for(unit_productions.rewind();
                    unit_productions.match_next(); ) {

                    cfg.remove_production(unit_production);

                    if(A == B) {
                        continue;
                    }

                    elim_key = std::make_pair(A,B);
                    if(0 != eliminated.count(elim_key)) {
                        continue;
                    }

                    eliminated.insert(elim_key);
                    updated = true;

                    for(related_productions.rewind();
                        related_productions.match_next(); ) {

                        // don't introduce self-loops
                        if(1 == gen_string.length()
                        && A == gen_string.at(0)) {
                            continue;
                        }

                        cfg.add_production(A, gen_string);
                    }
                }
            }

#if 0
            const variable_type start_var(cfg.get_start_variable());
            symbol_string_type str;
            variable_type A;
            variable_type B;
            production_type unit_production;
            production_type prod_related_to_B;
            production_type P;

            std::pair<variable_type,variable_type> elim_key;
            std::set<std::pair<variable_type,variable_type> > eliminated;

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

                    eliminated.insert(std::make_pair(A, B));

                    // don't follow into self-loops!
                    if(A == B) {
                        found_self_loops = true;
                        cfg.remove_production(unit_production);
                        continue;
                    }

                    for(rules_with_nonempty_rhs.rewind();
                        rules_with_nonempty_rhs.match_next(); ) {

                        str = P.symbols();

                        // make sure not to add in any self loops
                        if(1 == str.length()) {
                            if(str.at(0).is_variable()) {
                                if(A != str.at(0)) {

                                    elim_key = std::make_pair(
                                        A,
                                        variable_type(str.at(0))
                                    );

                                    if(0 == eliminated.count(elim_key)) {
                                        cfg.add_production(A, str);
                                        added_unit_rhs = true;
                                    }
                                } else {
                                    found_self_loops = true;
                                }
                            }
                        } else {
                            cfg.add_production(A, str);
                        }
                    }

                    cfg.remove_production(unit_production);
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

                    // we found a null production and it was automatically
                    // added in by the system (i.e. we deleted the last
                    // production related to a variable, and then it added
                    // in an epsilon production to keep the relation.)
                    if(null_prod.was_automatically_made()) {
                        cfg.remove_variable(A);
                    }

                    /*
                    if(A == start_var) {

                    } else {
                        const unsigned num_prods(cfg.num_productions());
                        cfg.remove_production(null_prod);
                        if(num_prods == cfg.num_productions()) {
                            cfg.remove_variable(A);
                        }
                    }*/
                }
            }
#endif
        }
    };
}}

#endif /* FLTL_CFG_REMOVE_UNITS_HPP_ */
