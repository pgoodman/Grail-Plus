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

        friend class TDOP<AlphaT>;
        friend class detail::RuleGenerator<AlphaT>;
        friend class detail::PatternGenerator<AlphaT,true>;
        friend class detail::PatternGenerator<AlphaT,false>;

        FLTL_TDOP_USE_TYPES(TDOP<AlphaT>);

        Rule<AlphaT> *rule;

        OpaqueRule(Rule<AlphaT> *rule_) throw()
            : rule(rule_)
        {
            Rule<AlphaT>::incref(rule_);
        }

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
            if(rule == that.rule) {
                return *this;
            }

            Rule<AlphaT>::incref(that.rule);
            Rule<AlphaT>::decref(rule);
            rule = that.rule;
            return *this;
        }

        /// extractions
        category_type category(void) const throw() {
            assert(is_valid());
            return category_type(rule->category->number);
        }

        const operator_string_type operators(void) const throw() {
            if(!is_valid()) {
                operator_string_type epsilon;
                return epsilon;
            } else {
                return rule->str;
            }
        }

        /// match a continuation rule
        bool match(uint32_t &upper_bound_, operator_string_type &str_) const throw() {
            if(is_extension_rule()) {
                upper_bound_ = static_cast<uint32_t>(rule->upper_bound);
                str_ = rule->str;
                return true;
            }
            return false;
        }

        bool match(uint64_t &upper_bound_, operator_string_type &str_) const throw() {
            if(is_extension_rule()) {
                upper_bound_ = static_cast<uint64_t>(rule->upper_bound);
                str_ = rule->str;
                return true;
            }
            return false;
        }

        /// match an initial rule
        bool match(operator_string_type &str_) const throw() {
            if(is_initial_rule()) {
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
        bool is_initial_rule(void) const throw() {
            return 0 != rule && Rule<AlphaT>::INITIAL_RULE_UPPER_BOUND == rule->upper_bound;
        }
        bool is_extension_rule(void) const throw() {
            return 0 != rule && 0 <= rule->upper_bound;
        }
        bool is_valid(void) const throw() {
            return 0 != rule;
        }

        /// generators, pattern matching
        const Unbound<AlphaT,rule_tag>
        operator~(void) const throw() {
            return Unbound<AlphaT,rule_tag>(this);
        }
    };

}}


#endif /* Grail_Plus_TDOP_OPAQUERULE_HPP_ */
