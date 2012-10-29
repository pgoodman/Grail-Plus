/*
 * TDOP_MINIMIZE_WEAK.hpp
 *
 *  Created on: May 22, 2012
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef Grail_Plus_TDOP_MINIMIZE_WEAK_HPP_
#define Grail_Plus_TDOP_MINIMIZE_WEAK_HPP_

#include <cassert>
#include <map>

#include "fltl/include/mpl/Lambda.hpp"

namespace grail { namespace algorithm {

    /// attempt to merge categories together using a weak form of minimization.
    ///
    /// two categories are weakly equivalent iff
    ///     - they share the same initial rules
    ///     - they share the same operator strings for their extensions rules,
    ///       and:
    ///       - similar rules share the same upper bound; or:
    ///       - the different upper bounds on similar rules provably do not
    ///         change the language accepted by the machine.
    ///
    /// note: this tool will remove all symbol predicates.
    /// note: this tool assumes that there all categories are bound.
    /// note: this tool assumes all categories are reachable.
    template <typename AlphaT>
    class TDOP_MINIMIZE_WEAK {

        FLTL_TDOP_USE_TYPES(fltl::TDOP<AlphaT>);

    private:

        /// returns true iff the tdop machine has any rule that contains an
        /// unbound category.
        static bool has_unbound_category(tdop_type &tdop) throw() {
            category_type C;
            return tdop.contains_rule(tdop._ --->* tdop.__ + C + tdop.__)
                || tdop.contains_rule(tdop.__ --->* tdop.__ + C + tdop.__);
        }

        /// given an operator string, return an operator string with all symbol
        /// predicates removed
        static operator_string_type remove_predicates(
            operator_string_type in
        ) throw() {
            operator_string_type out;
            for(unsigned i(0); i < in.length(); ++i) {
                if(in[i].is_category()) {
                    out = out + in[i];
                } else if(in[i].is_symbol() && !in[i].is_symbol_predicate()) {
                    out = out + in[i];
                }
            }
            return out;
        }

        /// remove all predicate symbols
        static void remove_predicate_symbols(tdop_type &tdop) throw() {
            rule_type R;
            symbol_type S;
            category_type C;
            unsigned upper_bound(0);

            generator_type initial_rules(tdop.search(
                ~R, (~C) --->* tdop.__ + *S + tdop.__)
            );

            for(; initial_rules.match_next(); ) {
                tdop.remove_rule(R);
                tdop.add_initial_rule(C, remove_predicates(R.operators()));
            }

            generator_type extension_rules(tdop.search(
                ~R, (~C)[upper_bound] --->* tdop.__ + *S + tdop.__)
            );

            for(; extension_rules.match_next(); ) {
                tdop.remove_rule(R);
                tdop.add_extension_rule(C, upper_bound, remove_predicates(R.operators()));
            }
        }

        enum equivalence_state {
            UNCHECKED,
            YES,
            NO,
            MAYBE
        };

        static const char *name(equivalence_state status) throw() {
            switch(status) {
            case UNCHECKED: return "UNCHECKED";
            case YES: return "YES";
            case NO: return "NO";
            case MAYBE: return "MAYBE";
            }
            return "UNDEFINED??";
        }

        /// represents a category instance in the tdop machine
        struct category_instance {
        public:
            category_type category;
            unsigned lower_bound;

            category_instance(category_type c, unsigned lb) throw()
                : category(c)
                , lower_bound(lb)
            { }

            bool operator<(const category_instance &that) const throw() {
                if(category < that.category) return true;
                if(that.category < category) return false;
                return lower_bound < that.lower_bound;
            }

            bool operator==(const category_instance &that) const throw() {
                return category == that.category && lower_bound == that.lower_bound;
            }
        };

#if 0

        /// represents a matrix of equivalences.
        struct equivalence_map {

            typedef std::pair<category_type, category_instance> eq_coord;
            typedef std::map<eq_coord, equivalence_state> status_map_type;

            status_map_type status_map;
            bool has_maybe;

            equivalence_map(void)
                : status_map()
                , has_maybe(false)
            { }

            equivalence_state &
            operator()(category_type C, category_instance D_inst) throw() {
                return status_map[std::make_pair(C, D_inst)];
            }

            /// replace all NO entries with UNCHECKED entries.
            bool clear_nos(void) throw() {
                /*
                typename status_map_type::iterator it(status_map.begin());
                bool found_no(false);
                has_maybe = false;

                for(; it != status_map.end(); ++it) {
                    if(NO == it->second) {
                        it->second = UNCHECKED;
                        found_no = true;
                    } else if(MAYBE == it->second) {
                        has_maybe = true;
                    }
                }

                return found_no;*/
                return false;
            }
        };
#endif

        /// check if two category instances are strictly similar
        static equivalence_state
        check_strict_similarity(
            tdop_type &tdop,
            category_type C,
            category_type D,
            unsigned lower_bound
        ) {
            std::set<operator_string_type> C_ops, D_ops;
            operator_string_type ops;

            tdop.collect(C --->* (~ops), ops, C_ops);
            tdop.collect(D --->* (~ops), ops, D_ops);

            if(C_ops != D_ops) {
                return NO;
            }

            unsigned upper_bound(0);
            std::list<rule_type> C_rules;
            std::list<rule_type> D_rules;

            tdop.collect_if(
                C[upper_bound] --->* (~ops), ops, C_ops, C_rules,
                fltl::mpl::predicate::less_than(lower_bound, upper_bound)
            );

            tdop.collect_if(
                D[upper_bound] --->* (~ops), ops, D_ops, D_rules,
                fltl::mpl::predicate::less_than(lower_bound, upper_bound)
            );

            if(C_ops != D_ops) {
                return NO;
            }

            typedef typename std::list<rule_type>::iterator rule_list_iterator;

            rule_list_iterator C_rules_it(C_rules.begin());
            rule_list_iterator D_rules_it(D_rules.begin());

            unsigned C_upper_bound(0);
            unsigned D_upper_bound(0);

            for(; C_rules_it != C_rules.end(); ++C_rules_it, ++D_rules_it) {
                C_rules_it->match(C_upper_bound, ops);
                D_rules_it->match(D_upper_bound, ops);

                if(C_upper_bound != D_upper_bound) {
                    return MAYBE;
                }
            }

            return YES;
        }

        /// get the set of all operator instances
        static void get_instances(
            tdop_type &tdop,
            std::set<category_instance> &instances
        ) throw() {
            instances.insert(category_instance(tdop.get_initial_category(), 0U));

            rule_type R;
            category_type C;
            unsigned lower_bound(0U);
            generator_type rules(tdop.search(~R));

            for(; rules.match_next(); ) {
                const operator_string_type ops(R.operators());
                for(unsigned i(0); i < ops.length(); ++i) {
                    if(ops[i].match(C, lower_bound)) {
                        instances.insert(category_instance(C, lower_bound));
                    }
                }
            }
        }

        /// replace all instances of one category with another
        static void replace_instances(
            tdop_type &tdop,
            operator_type find,
            operator_type replace
        ) throw() {
            operator_string_type prefix, suffix;
            category_type C;
            rule_type rule, R;
            unsigned upper_bound(0U);

            generator_type initial_rules(tdop.search(~rule, (~C) --->* (~prefix) + find + (~suffix)));
            generator_type extension_rules(tdop.search(~rule, (~C)[upper_bound] --->* (~prefix) + find + (~suffix)));

            for(bool updated(true); updated; ) {
                updated = false;

                printf("loop:\n");

                for(initial_rules.rewind(); initial_rules.match_next(); ) {
                    updated = true;
                    tdop.remove_rule(rule);
                    R = tdop.add_initial_rule(C, prefix + replace + suffix);

                    printf("  replace:\n    ");
                    io::fprint(stdout, tdop, rule);
                    printf("\n    ");
                    io::fprint(stdout, tdop, R);
                    printf("\n\n");
                }

                for(extension_rules.rewind(); extension_rules.match_next(); ) {
                    updated = true;
                    tdop.remove_rule(rule);
                    R = tdop.add_extension_rule(C, upper_bound, prefix + replace + suffix);

                    printf("  replace:\n    ");
                    io::fprint(stdout, tdop, rule);
                    printf("\n    ");
                    io::fprint(stdout, tdop, R);
                    printf("\n\n");
                }
            }

            //printf("replaced %s with %s\n\n", tdop.get_name(find), tdop.get_name(replace));

            //tdop.unsafe_remove_category(find);
        }

    public:

        static void run(tdop_type &tdop) throw() {

            // nothing to do; no categories
            if(0U == tdop.num_categories()) {
                return;
            }

            assert(!has_unbound_category(tdop));

            // remove and replace all rules containing predicate symbols
            remove_predicate_symbols(tdop);

            //equivalence_map eq;

            std::set<category_instance> instances;
            get_instances(tdop, instances);

            typedef typename std::set<category_instance>::iterator instance_iterator;

            instance_iterator inst_it;
            //instance_iterator sub_inst_it;

            category_type C;
            //category_type find;
            //category_type replace;
            generator_type Cs(tdop.search(~C));

            //std::set<category_type> deleted;
            std::set<std::pair<category_type, category_instance> > merged;

            for(bool changed(true); changed; ) {
                changed = false;

                std::map<category_instance, std::set<category_type> > similar_categories;


                inst_it = instances.begin();
            //retry:
                for(; inst_it != instances.end(); ++inst_it) {

                    /*if(deleted.count(inst_it->category)) {
                        category_instance inst(*inst_it);
                        ++inst_it;
                        instances.erase(inst);
                        goto retry;
                    }*/

                    for(Cs.rewind(); Cs.match_next(); ) {

                        equivalence_state status(check_strict_similarity(
                            tdop, C, inst_it->category, inst_it->lower_bound
                        ));

                        printf("%s ~ %s(%u) ? %s\n",
                            tdop.get_name(C),
                            tdop.get_name(inst_it->category),
                            inst_it->lower_bound,
                            name(status)
                        );

                        if(MAYBE == status) {
                            similar_categories[*inst_it].insert(C);

                        // merge
                        } else if(YES == status && C != inst_it->category
                               && tdop.num_rules(C) >= tdop.num_rules(inst_it->category)) {

                            if(merged.count(std::make_pair(C, *inst_it))) {
                                continue;
                            }

                            changed = true;

                            /*
                            // todo find a better heuristic
                            find = C;
                            replace = inst_it->category;

                            if(tdop.num_rules(find) > tdop.num_rules(replace)) {
                                replace = C;
                                find = inst_it->category;
                                instances.erase(*inst_it);
                            }

                            deleted.insert(find);
                            */

                            merged.insert(std::make_pair(C, *inst_it));

                            replace_instances(tdop,
                                inst_it->category(inst_it->lower_bound),
                                C(inst_it->lower_bound)
                            );

                            goto merged_categories;
                        }


                    }

                    /*for(sub_inst_it = instances.begin(); sub_inst_it != instances.end(); ++sub_inst_it) {
                        category_instance C(*inst_it);
                        category_instance D(*sub_inst_it);

                        equivalence_state &status(eq(C, D));


                    }*/
                }


                printf("\n\n");
                break;

            merged_categories:

                printf("merged!\n");
                continue;

                /*
                for(Cs.rewind(); Cs.match_next(); ) {
                    for(Ds.rewind(); Ds.match_next(); ) {
                        equivalence_state &status(eq(C, D));

                        if(UNCHECKED == status) {
                            //status = check_equal(tdop, C, D);
                        }
                    }
                }*/
                /*
                // mark some non-equivalences as unchecked and continue
                if(eq.clear_nos()) {
                    continue;
                }

                // no chance of trying to prove weak (non)equivalence of similar
                // rules.
                if(!eq.has_maybe) {
                    break;
                }
                */
                // no chance of finding more weak equivalences; try for strong
                // equivalence, if any
            }
        }
    };
}}


#endif /* Grail_Plus_TDOP_MINIMIZE_WEAK_HPP_ */
