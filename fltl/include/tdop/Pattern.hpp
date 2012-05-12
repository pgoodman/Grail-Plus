/*
 * Pattern.hpp
 *
 *  Created on: May 12, 2012
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef Grail_Plus_TDOP_PATTERN_HPP_
#define Grail_Plus_TDOP_PATTERN_HPP_

namespace fltl { namespace tdop {

    namespace detail {

        template <typename AlphaT>
        union Slot {
        public:

            FLTL_TDOP_USE_TYPES(TDOP<AlphaT>);

            symbol_type *as_symbol;
            category_type *as_category;
            term_type *as_term;

            // more general than above
            operator_type *as_operator;
            operator_string_type *as_operator_string;

            // for operator strings
            unsigned *as_length;

            // for category instantiations
            unsigned *as_lower_bound;

        };

        /// stores the data needed to match patterns in a TDOP parser.
        template <typename AlphaT>
        class PatternData {
        private:

            enum {
                NUM_SLOTS = 8U,
                ALLOC_GROUP_SIZE = 8U
            };

            typedef PatternData<AlphaT> self_type;
            typedef helper::BlockAllocator<self_type, ALLOC_GROUP_SIZE> allocator_type;

            friend class helper::BlockAllocator<self_type, ALLOC_GROUP_SIZE>;
            friend class Pattern<AlphaT>;

            FLTL_TDOP_USE_TYPES(TDOP<AlphaT>);

            /// reference counting; needed so that multiple pattern_types can
            /// be passed around, use the same pattern, etc.
            unsigned ref_count;

            /// lower bound; if non-NULL, then the pattern function looks only
            /// at extension rules, and might use this for reading/writing. If
            /// NULL, then initial rules are looked at.
            unsigned *lower_bound;

            /// category pointed to by this data; if NULL then we don't care
            /// about the category, if non-NULL then the pattern-matching
            /// function determines if this is used for reading or for writing
            category_type *category;

            /// slots holding pointers back to pattern data
            Slot<AlphaT> slots[NUM_SLOTS];

            /// allocator for pattern data
            static allocator_type pattern_allocator;

            /// constructor / destructor

            PatternData(void)
                : ref_count(0U)
                , lower_bound(0)
                , category(0)
            {
                memset(&(slots[0]), 0, NUM_SLOTS * sizeof(Slot<AlphaT>));
            }

            ~PatternData(void) {
                category = 0;
                lower_bound = 0;
                memset(&(slots[0]), 0, NUM_SLOTS * sizeof(Slot<AlphaT>));
            }

            /// allocation for binding/reading from a category, for initial
            /// rules
            static self_type *allocate(category_type *category) throw() {
                self_type *pattern(pattern_allocator.allocate());
                pattern->category = category;
                return pattern;
            }

            /// allocation for initial rules, ignoring the category
            static self_type *allocate(void) throw() {
                return pattern_allocator.allocate();
            }

            /// allocation for binding/reading from a category, for extension
            /// rules
            static self_type *allocate(category_type *category, unsigned *lower_bound) throw() {
                self_type *pattern(pattern_allocator.allocate());
                pattern->category = category;
                pattern->lower_bound = lower_bound;
                return pattern;
            }

            /// allocation for extension rules, ignoring the category
            static self_type *allocate(unsigned *lower_bound) throw() {
                self_type *pattern(pattern_allocator.allocate());
                pattern->lower_bound = lower_bound;
                return pattern;
            }

            /// reference counting

            static void incref(self_type *self) throw() {
                ++(self->ref_count);
            }

            static void decref(PatternData<AlphaT> *self) throw() {
                self->ref_count -= 1U;
                if(0 == self->ref_count) {
                    pattern_allocator.deallocate(self);
                }
            }

            /// extension of the pattern

        };

        /// static allocator for patterns
        template <typename AlphaT>
        helper::BlockAllocator<
            PatternData<AlphaT>,
            PatternData<AlphaT>::ALLOC_GROUP_SIZE
        > PatternData<AlphaT>::pattern_allocator;
    }

}}

#endif /* Grail_Plus_TDOP_PATTERN_HPP_ */
