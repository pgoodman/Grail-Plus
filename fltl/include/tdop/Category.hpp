/*
 * Category.hpp
 *
 *  Created on: May 10, 2012
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef Grail_Plus_TDOP_CATEGORY_HPP_
#define Grail_Plus_TDOP_CATEGORY_HPP_

namespace fltl { namespace tdop {

    /// a TDOP parser category of rules.
    template <typename AlphaT>
    class Category {
    private:

        friend class TDOP<AlphaT>;
        friend class Term<AlphaT>;
        friend class OpaqueCategory<AlphaT>;
        friend class OpaqueRule<AlphaT>;
        friend class Rule<AlphaT>;

        friend class detail::CategoryGenerator<AlphaT>;
        friend class detail::SymbolGenerator<AlphaT>;
        friend class detail::RuleGenerator<AlphaT>;
        friend class detail::PatternGenerator<AlphaT,true>;
        friend class detail::PatternGenerator<AlphaT,false>;

        typedef Category<AlphaT> self_type;

        unsigned ref_count;

        /// the "number"/id of this parsercategory
        uint32_t number;

        /// previous and next categories within this TDOP machine
        self_type *prev;
        self_type *next;

        /// first initial and extension rules
        Rule<AlphaT> *first_initial_rule;
        Rule<AlphaT> *first_extension_rule;

        unsigned num_initial_rules;
        unsigned num_extension_rules;

        /// name of the category, might be undefined until it is asked
        /// for
        ///
        /// !!! when deleted, this is used for chaining together categories
        ///     in the unused category list so that category numbers can be
        ///     reused.
        const char *name;

        /// is this category deleted?
        bool is_deleted;

        static helper::BlockAllocator<self_type> allocator;

        static self_type *allocate(void) throw() {
            return allocator.allocate();
        }

        static void incref(self_type *ptr) throw() {
            if(0 != ptr) {
                ++(ptr->ref_count);
            }
        }

        static void decref(self_type *ptr) throw() {
            if(0 != ptr) {
                ptr->ref_count -= 1;
                if(0 == ptr->ref_count) {
                    allocator.deallocate(ptr);
                }
            }
        }

        /// delete the rules of a TDOP machine
        /// !!! this deletes the rules, but does not unlink them.
        void delete_rules(void) throw() {

            // clear out the initial rules
            for(Rule<AlphaT> *rule(first_initial_rule), *next_rule(0);
                0 != rule;
                rule = next_rule) {

                next_rule = rule->next;
                rule->is_deleted = true;
                Rule<AlphaT>::decref(rule);
            }

            // clear out the extension rules
            for(Rule<AlphaT> *rule(first_extension_rule), *next_rule(0);
                0 != rule;
                rule = next_rule) {

                next_rule = rule->next;
                rule->is_deleted = true;
                Rule<AlphaT>::decref(rule);
            }
        }

        /// relink the TDOP machine's category list
        void relink(void) throw() {

            // re-link the category chains
            if(0 != prev) {
                prev->next = next;
            }

            if(0 != next) {
                next->prev = prev;
            }
        }

    public:

        Category(void) throw()
            : ref_count(1U)
            , is_deleted(false)
        { }

        ~Category(void) throw() {
            if(0 != name) {
                trait::Alphabet<const char *>::destroy(name);
                name = 0;
            }

            relink();
            delete_rules();

            first_initial_rule = 0;
            first_extension_rule = 0;
            num_initial_rules = 0U;
            num_extension_rules = 0U;
        }
    };

    // static initialization
    template <typename AlphaT>
    helper::BlockAllocator<Category<AlphaT> > Category<AlphaT>::allocator;
}}

#endif /* Grail_Plus_TDOP_CATEGORY_HPP_ */
