/*
 * TDOP_RENUMBER.hpp
 *
 *  Created on: May 22, 2012
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef Grail_Plus_TDOP_RENUMBER_HPP_
#define Grail_Plus_TDOP_RENUMBER_HPP_

#include <cstring>
#include <cstdlib>

namespace grail { namespace algorithm {

    /// goes and forcibly numbers all tdop categories, such that there are no
    /// unbound parser categories.
    ///
    /// Note: this algorithm will remove all existing categories, as the set
    ///       of instantiated categories is necessarily the set of reachable
    ///       categories.
    template <typename AlphaT>
    class TDOP_RENUMBER {

        FLTL_TDOP_USE_TYPES(fltl::TDOP<AlphaT>);

    private:

        /// get the instance name for an instantiated category
        static void set_instance_name(
            char *buffer,
            const tdop_type &tdop,
            category_type base,
            unsigned lower_bound
        ) throw() {
            sprintf(buffer, "%s$%u", tdop.get_name(base), lower_bound);
        }

        /// instantiate all reachable categories within an operator string, and
        /// return the fully instantiated operator string
        static operator_string_type instantiate_ops(
            tdop_type &tdop,
            unsigned inherited_lower_bound,
            const operator_string_type &ops_
        ) throw() {
            operator_string_type ops;
            category_type C;
            category_type C_prime;

            for(unsigned i(0); i < ops_.length(); ++i) {
                unsigned lower_bound(0);
                if(ops_[i].is_symbol()) {
                    ops = ops + ops_[i];
                } else if(ops_[i].match(C, lower_bound)) {
                    C_prime = instantiate(tdop, C, lower_bound);
                    ops = ops + C_prime(lower_bound);
                } else if(ops_[i].match(C)) {
                    C_prime = instantiate(tdop, C, inherited_lower_bound);
                    ops = ops + C_prime(inherited_lower_bound);
                } else {
                    assert(false);
                }
            }

            return ops;
        }

        /// instantiate a particular category
        static category_type instantiate(
            tdop_type &tdop,
            category_type base,
            unsigned lower_bound
        ) throw() {
            char name[1024] = {'\0'};
            set_instance_name(name, tdop, base, lower_bound);

            const unsigned old_num_categories(tdop.num_categories());
            category_type C(tdop.get_category(name));

            // getting this category by its name didn't add a new category;
            // thus this category instance was just created
            if(old_num_categories == tdop.num_categories()) {
                return C;
            }

            unsigned upper_bound(0);
            operator_string_type ops;
            generator_type initial_rules(tdop.search(base --->* (~ops)));
            generator_type extension_rules(tdop.search(base[upper_bound] --->* (~ops)));

            for(; initial_rules.match_next(); ) {
                tdop.add_initial_rule(C, instantiate_ops(tdop, lower_bound, ops));
            }

            for(; extension_rules.match_next(); ) {
                if(lower_bound < upper_bound) {
                    tdop.add_extension_rule(C, upper_bound, instantiate_ops(tdop, lower_bound, ops));
                }
            }

            return C;
        }

    public:

        static void run(tdop_type &tdop) throw() {
            category_type C;
            generator_type categories(tdop.search(~C));

            // collect all original categories for later removal
            std::set<category_type> original_categories;
            for(; categories.match_next(); ) {
                original_categories.insert(C);
            }

            // instantiate all reachable categories
            if(0 != tdop.num_categories()) {
                category_type start(instantiate(tdop, tdop.get_initial_category(), 0));
                tdop.set_initial_category(start);
            }

            // remove all original categories
            for(categories.rewind(); categories.match_next(); ) {
                if(original_categories.count(C)) {
                    tdop.unsafe_remove_category(C);
                }
            }
        }
    };
}}


#endif /* Grail_Plus_TDOP_RENUMBER_HPP_ */
