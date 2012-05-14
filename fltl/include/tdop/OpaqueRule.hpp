/*
 * OpaqueRule.hpp
 *
 *  Created on: May 10, 2012
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef Grail_Plus_TDOP_OPAQUERULE_HPP_
#define Grail_Plus_TDOP_OPAQUERULE_HPP_

namespace fltl { namespace tdop {

    template <typename AlphaT>
    class OpaqueRule {
    private:

        FLTL_TDOP_USE_TYPES(TDOP<AlphaT>);

        Rule<AlphaT> *rule;

    public:

        /// construction, destruction

        OpaqueRule(void) throw()
            : rule(0)
        { }

        OpaqueRule(const rule_type &that) throw()
            : rule(that.rule)
        {
            Rule<AlphaT>::incref(rule);

        }

        ~OpaqueRule(void) throw() {
            Rule<AlphaT>::decref(rule);
            rule = 0;
        }

        /// assignment
        rule_type &operator=(const rule_type &that) throw() {
            if(&that == this) {
                return *this;
            }

            Rule<AlphaT>::decref(rule);
            rule = that.rule;
            Rule<AlphaT>::incref(rule);
            return *this;
        }

        /// extractions
        category_type category(void) const throw() {
            assert(is_valid());
            return category_type(rule->category->number);
        }

        /// match a continuation rule
        bool match(int32_t &upper_bound_, operator_string_type &str_) const throw() {
            if(0 != rule && 0 <= rule->upper_bound) {
                upper_bound_ = rule->upper_bound;
                str_ = rule->str;
                return true;
            }
            return false;
        }

        bool match(int64_t &upper_bound_, operator_string_type &str_) const throw() {
            if(0 != rule && 0 <= rule->upper_bound) {
                upper_bound_ = rule->upper_bound;
                str_ = rule->str;
                return true;
            }
            return false;
        }

        /// match an initial rule
        bool match(operator_string_type &str_) const throw() {
            if(0 != rule && Rule<AlphaT>::INITIAL_RULE_UPPER_BOUND == rule->upper_bound) {
                str_ = rule->str;
                return true;
            }
            return false;
        }

        /// check for equality
        bool operator==(const rule_type &that) const throw() {
            return rule == that.rule;
        }

        bool operator!=(const rule_type &that) const throw() {
            return rule != that.rule;
        }

        /// status checking
        bool is_valid(void) const throw() {
            return 0 != rule;
        }
    };

}}


#endif /* Grail_Plus_TDOP_OPAQUERULE_HPP_ */
