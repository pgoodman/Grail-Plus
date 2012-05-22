/*
 * Rule.hpp
 *
 *  Created on: May 10, 2012
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef Grail_Plus_TDOP_RULE_HPP_
#define Grail_Plus_TDOP_RULE_HPP_

namespace fltl { namespace tdop {

    /// represents a parser rule. This encompasses both initial and extension
    /// rules. An initial rule is has a termina/symbol left corner. An extension
    /// rule has as its left corner another rule. The extenstion rule is
    /// anchored by a symbol, or a symbol predicate.
    template <typename AlphaT>
    class Rule {
    private:

        typedef Rule<AlphaT> self_type;
        typedef helper::BlockAllocator<self_type> allocator_type;

        friend class TDOP<AlphaT>;
        friend class OpaqueRule<AlphaT>;
        friend class Category<AlphaT>;
        friend class helper::BlockAllocator<self_type>;

        friend class detail::CategoryGenerator<AlphaT>;
        friend class detail::SymbolGenerator<AlphaT>;
        friend class detail::RuleGenerator<AlphaT>;
        friend class detail::PatternGenerator<AlphaT,true>;
        friend class detail::PatternGenerator<AlphaT,false>;

        enum {
            INITIAL_RULE_UPPER_BOUND = -1,
            UNINITIALIZED = -2
        };

        uint32_t ref_count;
        bool is_deleted;

        /// the category this rule belongs to
        Category<AlphaT> *category;

        /// rule chain of a category
        self_type *prev;
        self_type *next;

        /// upper bound for continuation rules, otherwise
        /// INITIAL_RULE_UPPER_BOUND for initial rules.
        int32_t upper_bound;

        /// sequence of operators
        OperatorString<AlphaT> str;

        /// reference counting, allocation
        static allocator_type rule_allocator;

        static void incref(self_type *ptr) throw() {
            if(0 != ptr) {
                ++(ptr->ref_count);
            }
        }

        static void decref(self_type *ptr) throw() {
            if(0 != ptr) {
                --(ptr->ref_count);
                if(0 == ptr->ref_count) {
                    rule_allocator.deallocate(ptr);
                }
            }
        }

        static self_type *allocate(void) throw() {
            return rule_allocator.allocate();
        }

        /// lexicograpgically compare two rules
        bool is_less_than(const self_type *that) const throw() {
            if(that == this) {
                return false;
            }

            // both extension rules
            if(0 <= upper_bound
            && 0 <= that->upper_bound) {

                if(str < that->str) {
                    return true;

                } else if(that->str < str) {
                    return false;

                } else {
                    return upper_bound < that->upper_bound;
                }

            // both initial rules
            } else if(INITIAL_RULE_UPPER_BOUND == upper_bound
                   && INITIAL_RULE_UPPER_BOUND == that->upper_bound) {

                return str < that->str;

            // 'that' is an initial rule
            } else if(0 <= upper_bound) {
                return false;

            // 'this' is an initial rule
            } else {
                return true;
            }
        }

    public:

        /// constructor
        Rule(void) throw()
            : ref_count(1U)
            , is_deleted(false)
            , category(0)
            , prev(0)
            , next(0)
            , upper_bound(UNINITIALIZED)
            , str()
        { }

        /// destructor
        ~Rule(void) throw() {

            if(0 != category) {

                if(category->first_initial_rule == this) {
                    category->first_initial_rule = next;

                } else if(category->first_extension_rule == this) {
                    category->first_extension_rule = next;
                }
            }

            if(0 != next) {
                next->prev = prev;
            }

            if(0 != prev) {
                prev->next = next;
            }

            category = 0;
            prev = 0;
            next = 0;
            upper_bound = UNINITIALIZED;
        }
    };

    /// static initialization of rule allocator
    template <typename AlphaT>
    typename Rule<AlphaT>::allocator_type Rule<AlphaT>::rule_allocator;

}}

#endif /* Grail_Plus_TDOP_RULE_HPP_ */
