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
        friend class helper::BlockAllocator<self_type>;

        enum {
            INITIAL_RULE_UPPER_BOUND = -1,
            UNINITIALIZED = -2
        };

        uint32_t ref_count;

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

        /// constructor
        Rule(void) throw()
            : ref_count(1U)
            , category(0)
            , prev(0)
            , next(0)
            , upper_bound(UNINITIALIZED)
            , str()
        { }

    public:

        /// destructor
        ~Rule(void) throw() {
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
