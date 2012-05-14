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
    }
}}

/// set up bounds for individual pattern parts (factors)
namespace fltl { namespace pattern {

    template <const unsigned offset>
    class Factor<tdop::category_lb_tag,offset> {
    public:
        enum {
            WIDTH = 2,
            MIN_NUM_SYMBOLS = 1,
            OFFSET = offset,
            NEXT_OFFSET = OFFSET + WIDTH
        };
    };

    template <const unsigned offset>
    class Factor<tdop::unbound_category_lb_tag,offset> {
    public:
        enum {
            WIDTH = 2,
            MIN_NUM_SYMBOLS = 1,
            OFFSET = offset,
            NEXT_OFFSET = OFFSET + WIDTH
        };
    };

    template <const unsigned offset>
    class Factor<tdop::operator_tag,offset> {
    public:
        enum {
            WIDTH = 2,
            MIN_NUM_SYMBOLS = 1,
            OFFSET = offset,
            NEXT_OFFSET = OFFSET + WIDTH
        };
    };

    template <const unsigned offset>
    class Factor<tdop::unbound_operator_tag,offset> {
    public:
        enum {
            WIDTH = 2,
            MIN_NUM_SYMBOLS = 1,
            OFFSET = offset,
            NEXT_OFFSET = OFFSET + WIDTH
        };
    };

    template <const unsigned offset>
    class Factor<tdop::operator_string_tag,offset> {
    public:
        enum {
            WIDTH = 1,
            MIN_NUM_SYMBOLS = 0,
            OFFSET = offset,
            NEXT_OFFSET = OFFSET + WIDTH
        };
    };

    template <const unsigned offset>
    class Factor<tdop::any_operator_tag,offset> {
    public:
        enum {
            WIDTH = 0,
            MIN_NUM_SYMBOLS = 1,
            OFFSET = offset,
            NEXT_OFFSET = OFFSET + WIDTH
        };
    };

    template <const unsigned offset>
    class Factor<tdop::any_operator_string_tag,offset> {
    public:
        enum {
            WIDTH = 1,
            MIN_NUM_SYMBOLS = 0,
            OFFSET = offset,
            NEXT_OFFSET = OFFSET + WIDTH
        };
    };

    template <const unsigned offset>
    class Factor<tdop::any_operator_string_of_length_tag,offset> {
    public:
        enum {
            WIDTH = 1,
            MIN_NUM_SYMBOLS = 0,
            OFFSET = offset,
            NEXT_OFFSET = OFFSET + WIDTH
        };
    };

    template <const unsigned offset>
    class Factor<tdop::unbound_operator_string_tag,offset> {
    public:
        enum {
            WIDTH = 1,
            MIN_NUM_SYMBOLS = 0,
            OFFSET = offset,
            NEXT_OFFSET = OFFSET + WIDTH
        };
    };
}}

/// match individual things within a pattern
namespace fltl { namespace pattern {

    /// category + unbound lower bound + T
    template <typename AlphaT, typename StringT, const unsigned offset, typename T>
    class Match<AlphaT,StringT,offset,tdop::symbol_tag, T> {
    public:
        inline static bool bind(tdop::detail::Slot<AlphaT> *slots, const tdop::Operator<AlphaT> *symbols, const unsigned len) throw() {

            if(1 <= len) {
                tdop::OpaqueCategory<AlphaT> cat;
                unsigned lb(0);

                if(!symbols->match(cat, lb)
                || cat != *(slots->as_category)) {
                    return false;
                }

                *((slots + 1)->as_lower_bound) = lb;

                return Match<
                    AlphaT,
                    StringT,
                    offset + 1,
                    T,
                    typename GetFactor<StringT,offset + 2>::type
                >::bind(slots + 2, symbols + 1, len - 1);

            } else {
                return false;
            }
        }
    };

    /// category + T
    template <typename AlphaT, typename StringT, const unsigned offset, typename T>
    class Match<AlphaT,StringT,offset,tdop::symbol_tag, T> {
    public:
        inline static bool bind(tdop::detail::Slot<AlphaT> *slots, const tdop::Operator<AlphaT> *symbols, const unsigned len) throw() {

            if(1 <= len) {
                tdop::OpaqueCategory<AlphaT> cat;

                return symbols->match(cat)
                    && (cat == *(slots->as_category))
                    && Match<
                        AlphaT,
                        StringT,
                        offset + 1,
                        T,
                        typename GetFactor<StringT,offset + 2>::type
                    >::bind(slots + 1, symbols + 1, len - 1);
            } else {
                return false;
            }
        }
    };

    /// symbol + T
    template <typename AlphaT, typename StringT, const unsigned offset, typename T>
    class Match<AlphaT,StringT,offset,tdop::symbol_tag, T> {
    public:
        inline static bool bind(tdop::detail::Slot<AlphaT> *slots, const tdop::Operator<AlphaT> *symbols, const unsigned len) throw() {

            if(1 <= len) {
                tdop::Symbol<AlphaT> sym;

                return symbols->match(sym)
                    && (sym == *(slots->as_symbol))
                    && Match<
                        AlphaT,
                        StringT,
                        offset + 1,
                        T,
                        typename GetFactor<StringT,offset + 2>::type
                    >::bind(slots + 1, symbols + 1, len - 1);
            } else {
                return false;
            }
        }
    };

    /// term + T
    template <typename AlphaT, typename StringT, const unsigned offset, typename T>
    class Match<AlphaT,StringT,offset,tdop::term_tag, T> {
    public:
        inline static bool bind(tdop::detail::Slot<AlphaT> *slots, const tdop::Operator<AlphaT> *symbols, const unsigned len) throw() {

            if((1 <= len) && (symbols->term == *(slots->as_term))) {
                return Match<
                    AlphaT,
                    StringT,
                    offset + 1,
                    T,
                    typename GetFactor<StringT,offset + 2>::type
                >::bind(slots + 1, symbols + 1, len - 1);
            } else {
                return false;
            }
        }
    };

    /// operator + T
    template <typename AlphaT, typename StringT, const unsigned offset, typename T>
    class Match<AlphaT,StringT,offset,tdop::operator_tag, T> {
    public:
        inline static bool bind(tdop::detail::Slot<AlphaT> *slots, const tdop::Operator<AlphaT> *symbols, const unsigned len) throw() {

            if((1 <= len) && (*symbols == *(slots->as_operator))) {
                return Match<
                    AlphaT,
                    StringT,
                    offset + 1,
                    T,
                    typename GetFactor<StringT,offset + 2>::type
                >::bind(slots + 1, symbols + 1, len - 1);
            } else {
                return false;
            }
        }
    };

    /// operator string + T
    template <typename AlphaT, typename StringT, const unsigned offset, typename T>
    class Match<AlphaT,StringT,offset,tdop::operator_string_tag, T> {
    public:
        inline static bool bind(tdop::detail::Slot<AlphaT> *slots, const tdop::Operator<AlphaT> *symbols, const unsigned len) throw() {
            const unsigned str_len(slots->as_operator_string->length());
            enum {
                MIN_REQUIRED_LEN = 1 + GetMinNumSymbolsAfter<StringT,offset>::RESULT
            };

            if(str_len > len || MIN_REQUIRED_LEN > (1 + (len - str_len))) {
                return false;
            }

            if(0 != str_len) {
                const bool prefix_is_str(tdop::OperatorString<AlphaT>::symbol_memcmp(
                    symbols,
                    &(slots->as_operator_string->at(0)),
                    symbols + str_len - 1
                ));

                if(!prefix_is_str) {
                    return false;
                }
            }

            return Match<
                AlphaT,
                StringT,
                offset + 1,
                T,
                typename GetFactor<StringT,offset + 2>::type
            >::bind(slots + 1, symbols + str_len, len - str_len);
        }
    };

}}

/// unbound, non-trailing pattern parts
namespace fltl { namespace pattern {

    /// ignore + T
    template <typename AlphaT, typename StringT, const unsigned offset, typename T>
    class Match<AlphaT,StringT,offset,tdop::any_operator_tag, T> {
    public:
        inline static bool bind(tdop::detail::Slot<AlphaT> *slots, const tdop::Operator<AlphaT> *symbols, const unsigned len) throw() {
            return (len >= 1) && Match<
                AlphaT,
                StringT,
                offset + 1,
                T,
                typename GetFactor<StringT,offset + 2>::type
            >::bind(slots + 1, symbols + 1, len - 1);
        }
    };

    /// unbound category + T
    template <typename AlphaT, typename StringT, const unsigned offset, typename T>
    class Match<AlphaT,StringT,offset,tdop::unbound_category_tag, T> {
    public:
        inline static bool bind(tdop::detail::Slot<AlphaT> *slots, const tdop::Operator<AlphaT> *symbols, const unsigned len) throw() {
            if(len >= 1 && symbols->match(*(slots->as_category))) {

                return Match<
                    AlphaT,
                    StringT,
                    offset + 1,
                    T,
                    typename GetFactor<StringT,offset + 2>::type
                >::bind(slots + 1, symbols + 1, len - 1);
            }
            return false;
        }
    };

    /// unbound category + unbound lower bound + T
    template <typename AlphaT, typename StringT, const unsigned offset, typename T>
    class Match<AlphaT,StringT,offset,tdop::unbound_category_lb_tag, T> {
    public:
        inline static bool bind(tdop::detail::Slot<AlphaT> *slots, const tdop::Operator<AlphaT> *symbols, const unsigned len) throw() {
            if(len >= 1 && symbols->match(*(slots->as_category), *((slots + 1)->as_lower_bound))) {

                return Match<
                    AlphaT,
                    StringT,
                    offset + 1,
                    T,
                    typename GetFactor<StringT,offset + 2>::type
                >::bind(slots + 2, symbols + 1, len - 1);
            }
            return false;
        }
    };

    /// unbound symbol + T
    template <typename AlphaT, typename StringT, const unsigned offset, typename T>
    class Match<AlphaT,StringT,offset,tdop::unbound_symbol_tag, T> {
    public:
        inline static bool bind(tdop::detail::Slot<AlphaT> *slots, const tdop::Operator<AlphaT> *symbols, const unsigned len) throw() {
            if(len >= 1 && symbols->match(*(slots->as_symbol))) {

                return Match<
                    AlphaT,
                    StringT,
                    offset + 1,
                    T,
                    typename GetFactor<StringT,offset + 2>::type
                >::bind(slots + 1, symbols + 1, len - 1);
            }
            return false;
        }
    };

    /// unbound term + T
    template <typename AlphaT, typename StringT, const unsigned offset, typename T>
    class Match<AlphaT,StringT,offset,tdop::unbound_term_tag, T> {
    public:
        inline static bool bind(tdop::detail::Slot<AlphaT> *slots, const tdop::Operator<AlphaT> *symbols, const unsigned len) throw() {
            if(len >= 1) {
                *(slots->as_term) = symbols->term;

                return Match<
                    AlphaT,
                    StringT,
                    offset + 1,
                    T,
                    typename GetFactor<StringT,offset + 2>::type
                >::bind(slots + 1, symbols + 1, len - 1);
            }
            return false;
        }
    };

    /// unbound operator + T
    template <typename AlphaT, typename StringT, const unsigned offset, typename T>
    class Match<AlphaT,StringT,offset,tdop::unbound_symbol_tag, T> {
    public:
        inline static bool bind(tdop::detail::Slot<AlphaT> *slots, const tdop::Operator<AlphaT> *symbols, const unsigned len) throw() {
            if(len >= 1) {
                *(slots->as_operator) = *symbols;

                return Match<
                    AlphaT,
                    StringT,
                    offset + 1,
                    T,
                    typename GetFactor<StringT,offset + 2>::type
                >::bind(slots + 1, symbols + 1, len - 1);
            }
            return false;
        }
    };

    /// unbound operator string + T
    template <typename AlphaT, typename StringT, const unsigned offset, typename T>
    class Match<AlphaT,StringT,offset,tdop::unbound_operator_string_tag, T> {
    public:
        inline static bool bind(tdop::detail::Slot<AlphaT> *slots, const tdop::Operator<AlphaT> *symbols, const unsigned len) throw() {

            const unsigned max(len - GetMinNumSymbolsAfter<StringT,offset>::RESULT);
            for(unsigned i(0); i <= max; ++i) {

                tdop::OperatorString<AlphaT> str(symbols, i);
                *(slots->as_operator_string) = str;

                const bool matched_next_at_i(Match<
                    AlphaT,
                    StringT,
                    offset + 1,
                    T,
                    typename GetFactor<StringT,offset + 2>::type
                >::bind(slots + 1, symbols + i, len - i));

                if(matched_next_at_i) {
                    return true;
                }
            }

            // unbind it
            tdop::OperatorString<AlphaT> epsilon;
            *(slots->as_symbol_string) = epsilon;

            return false;
        }
    };

    /// any operator string + T
    template <typename AlphaT, typename StringT, const unsigned offset, typename T>
    class Match<AlphaT,StringT,offset,tdop::any_operator_string_tag, T> {
    public:
        inline static bool bind(tdop::detail::Slot<AlphaT> *slots, const tdop::Operator<AlphaT> *symbols, const unsigned len) throw() {

            const unsigned max(len - GetMinNumSymbolsAfter<StringT,offset>::RESULT);
            for(unsigned i(0); i <= max; ++i) {

                const bool matched_next_at_i(Match<
                    AlphaT,
                    StringT,
                    offset + 1,
                    T,
                    typename GetFactor<StringT,offset + 2>::type
                >::bind(slots + 1, symbols + i, len - i));

                if(matched_next_at_i) {
                    return true;
                }
            }

            return false;
        }
    };

    /// any operator string of a specific length + T
    template <typename AlphaT, typename StringT, const unsigned offset, typename T>
    class Match<AlphaT,StringT,offset,tdop::any_operator_string_of_length_tag, T> {
    public:
        inline static bool bind(tdop::detail::Slot<AlphaT> *slots, const tdop::Operator<AlphaT> *symbols, const unsigned len) throw() {

            const unsigned min_len(*(slots->as_length));

            if(len < (min_len + GetMinNumSymbolsAfter<StringT,offset>::RESULT)) {
                return false;
            }

            return Match<
                AlphaT,
                StringT,
                offset + 1,
                T,
                typename GetFactor<StringT,offset + 2>::type
            >::bind(slots + 1, symbols + min_len, len - min_len);
        }
    };

}}

/// bound, trailing pattern parts
namespace fltl { namespace pattern {

    /// base case, nothing to match.
    template <typename AlphaT, typename StringT, const unsigned offset>
    class Match<AlphaT,StringT,offset, void, void> {
    public:
        inline static bool bind(tdop::detail::Slot<AlphaT> *, const tdop::Operator<AlphaT> *, const unsigned len) throw() {
            return 0 == len;
        }
    };

    /// trailing category, unbound lower bound
    template <typename AlphaT, typename StringT, const unsigned offset>
    class Match<AlphaT,StringT,offset,tdop::category_lb_tag, void> {
    public:
        inline static bool bind(tdop::detail::Slot<AlphaT> *slots, const tdop::Operator<AlphaT> *symbols, const unsigned len) throw() {
            if(1 != len) {
                return false;
            }

            tdop::OpaqueCategory<AlphaT> cat;
            unsigned lb(0);
            if(!symbols->match(cat, lb)) {
                return false;
            }

            if(cat != *(slots->as_category)) {
                return false;
            }

            *((slots+1)->as_lower_bound) = lb;

            return true;
        }
    };

    /// trailing category
    template <typename AlphaT, typename StringT, const unsigned offset>
    class Match<AlphaT,StringT,offset,tdop::category_tag, void> {
    public:
        inline static bool bind(tdop::detail::Slot<AlphaT> *slots, const tdop::Operator<AlphaT> *symbols, const unsigned len) throw() {
            if(1 != len) {
                return false;
            }

            tdop::OpaqueCategory<AlphaT> cat;
            if(!symbols->match(cat)) {
                return false;
            }

            return cat == *(slots->as_category);
        }
    };

    /// trailing symbol
    template <typename AlphaT, typename StringT, const unsigned offset>
    class Match<AlphaT,StringT,offset,tdop::symbol_tag, void> {
    public:
        inline static bool bind(tdop::detail::Slot<AlphaT> *slots, const tdop::Operator<AlphaT> *symbols, const unsigned len) throw() {
            if(1 != len) {
                return false;
            }

            tdop::Term<AlphaT> sym;
            if(!symbols->match(sym)) {
                return false;
            }

            return sym == *(slots->as_symbol);
        }
    };

    /// trailing term
    template <typename AlphaT, typename StringT, const unsigned offset>
    class Match<AlphaT,StringT,offset,tdop::term_tag, void> {
    public:
        inline static bool bind(tdop::detail::Slot<AlphaT> *slots, const tdop::Operator<AlphaT> *symbols, const unsigned len) throw() {
            return (1 == len) && (symbols->term == slots->term);
        }
    };

    /// trailing operator
    template <typename AlphaT, typename StringT, const unsigned offset>
    class Match<AlphaT,StringT,offset,tdop::operator_tag, void> {
    public:
        inline static bool bind(tdop::detail::Slot<AlphaT> *slots, const tdop::Operator<AlphaT> *symbols, const unsigned len) throw() {
            return (1 == len) && (*symbols == *(slots->as_operator));
        }
    };

    /// trailing operator string
    template <typename AlphaT, typename StringT, const unsigned offset>
    class Match<AlphaT,StringT,offset,tdop::operator_string_tag, void> {
    public:
        inline static bool bind(tdop::detail::Slot<AlphaT> *slots, const tdop::Operator<AlphaT> *symbols, const unsigned len) throw() {

            const unsigned str_len(slots->as_operator_string->length());

            if(str_len != len) {
                return false;
            } else if(0 == str_len) {
                return true;
            }

            return tdop::OperatorString<AlphaT>::operator_memcmp(
                symbols,
                &(slots->as_operator_string->at(0)),
                symbols + str_len - 1
            );
        }
    };

}}

/// unbound trailing pattern parts
namespace fltl { namespace pattern {

    /// trailing ignore
    template <typename AlphaT, typename StringT, const unsigned offset>
    class Match<AlphaT,StringT,offset,tdop::any_operator_tag, void> {
    public:
        inline static bool bind(tdop::detail::Slot<AlphaT> *, const tdop::Operator<AlphaT> *, const unsigned len) throw() {
            return 1 == len;
        }
    };

    /// trailing unbound category
    template <typename AlphaT, typename StringT, const unsigned offset>
    class Match<AlphaT,StringT,offset,tdop::unbound_category_tag, void> {
    public:
        inline static bool bind(tdop::detail::Slot<AlphaT> *slots, const tdop::Operator<AlphaT> *symbols, const unsigned len) throw() {
            if(len > 1) {
                return false;
            }

            return symbols->match(*(slots->as_category));
        }
    };

    /// trailing unbound category with unbound lower bound
    template <typename AlphaT, typename StringT, const unsigned offset>
    class Match<AlphaT,StringT,offset,tdop::unbound_category_lb_tag, void> {
    public:
        inline static bool bind(tdop::detail::Slot<AlphaT> *slots, const tdop::Operator<AlphaT> *symbols, const unsigned len) throw() {
            if(len > 1) {
                return false;
            }

            return symbols->match(*(slots->as_category), (slots+1)->as_lower_bound);
        }
    };

    /// trailing unbound symbol
    template <typename AlphaT, typename StringT, const unsigned offset>
    class Match<AlphaT,StringT,offset,tdop::unbound_symbol_tag, void> {
    public:
        inline static bool bind(tdop::detail::Slot<AlphaT> *slots, const tdop::Operator<AlphaT> *symbols, const unsigned len) throw() {
            if(len > 1) {
                return false;
            }

            return symbols->match(*(slots->as_symbol));
        }
    };

    /// trailing unbound term
    template <typename AlphaT, typename StringT, const unsigned offset>
    class Match<AlphaT,StringT,offset,tdop::unbound_term_tag, void> {
    public:
        inline static bool bind(tdop::detail::Slot<AlphaT> *slots, const tdop::Operator<AlphaT> *symbols, const unsigned len) throw() {
            if(len > 1) {
                return false;
            }

            *(slots->as_term) = symbols->term;

            return true;
        }
    };

    /// trailing unbound operator
    template <typename AlphaT, typename StringT, const unsigned offset>
    class Match<AlphaT,StringT,offset,tdop::unbound_operator_tag, void> {
    public:
        inline static bool bind(tdop::detail::Slot<AlphaT> *slots, const tdop::Operator<AlphaT> *symbols, const unsigned len) throw() {
            if(len > 1) {
                return false;
            }

            *(slots->as_operator) = *symbols;

            return true;
        }
    };

    /// trailing unbound operator string
    template <typename AlphaT, typename StringT, const unsigned offset>
    class Match<AlphaT,StringT,offset,tdop::unbound_operator_string_tag, void> {
    public:
        inline static bool bind(tdop::detail::Slot<AlphaT> *slots, const tdop::Operator<AlphaT> *symbols, const unsigned len) throw() {
            tdop::OperatorString<AlphaT> str(symbols, len);
            *(slots->as_operator_string) = str;
            return true;
        }
    };

    /// trailing any symbol string
    template <typename AlphaT, typename StringT, const unsigned offset>
    class Match<AlphaT,StringT,offset,tdop::any_operator_string_tag, void> {
    public:
        inline static bool bind(tdop::detail::Slot<AlphaT> *, const tdop::Operator<AlphaT> *, const unsigned) throw() {
            return true;
        }
    };

    /// trailing any operator string of a specific length
    template <typename AlphaT, typename StringT, const unsigned offset>
    class Match<AlphaT,StringT,offset,tdop::any_operator_string_of_length_tag, void> {
    public:
        inline static bool bind(tdop::detail::Slot<AlphaT> *slots, const tdop::Operator<AlphaT> *, const unsigned len) throw() {
            return len == *(slots->as_length);
        }
    };

}}

/// reset pattern parts
namespace fltl { namespace pattern {

    /// reset the unbound operator strings of a pattern
    template <typename AlphaT, typename StringT, const unsigned offset>
    class ResetPattern<AlphaT,StringT,tdop::unbound_operator_string_tag,offset> {
    public:
        inline static void reset(tdop::detail::Slot<AlphaT> *slots) throw() {

            // reset the unbound symbol string
            tdop::OperatorString<AlphaT> epsilon;
            *(slots[offset].as_operator_string) = epsilon;

            ResetPattern<
                AlphaT,
                StringT,
                typename pattern::GetFactor<StringT,offset + 1>::type,
                offset + 1
            >::reset(slots);
        }
    };

}}

namespace fltl { namespace pattern {

    /// destructuring initial rules
    template <typename AlphaT, typename StringT>
    class DestructuringBind<AlphaT, tdop::symbol_tag, StringT> {
    public:

        typedef typename TDOP<AlphaT>::rule_type rule_type;

        inline static bool bind(
            tdop::detail::PatternData<AlphaT> *pattern,
            const rule_type &rule
        ) {
            if(!rule.is_valid()) {
                return false;
            }

            tdop::detail::Slot<AlphaT> *slots(&(pattern->slots[0]));

            pattern::ResetPattern<
                AlphaT,
                StringT,
                typename pattern::GetFactor<StringT,0>::type,
                0
            >::reset(slots);

            if(*(pattern->category) != prod.variable()) {
                return false;
            }

            const SymbolString<AlphaT> &str(prod.symbols());
            const unsigned str_len(str.length());

            if((1U + str_len) >= (1U + StringT::MIN_NUM_SYMBOLS)) {
                return Match<
                    AlphaT,
                    StringT,
                    0,
                    typename pattern::GetFactor<StringT,0>::type,
                    typename pattern::GetFactor<StringT,1>::type
                >::bind(slots, 0 == str_len ? 0 : &(str.at(0)), str_len);
            }

            return false;
        }
    };
}}

namespace fltl { namespace tdop { namespace detail {

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

        /// extension of the pattern with bound values

        inline void extend(symbol_type *expr, const unsigned slot) throw() {
            slots[slot].as_symbol = expr;
        }

        inline void extend(term_type *expr, const unsigned slot) throw() {
            slots[slot].as_term = expr;
        }

        inline void extend(operator_type *expr, const unsigned slot) throw() {
            slots[slot].as_operator = expr;
        }

        inline void extend(operator_string_type *expr, const unsigned slot) throw() {
            slots[slot].as_operator_string = expr;
        }

        /// extension of the pattern with unbound values

        inline void extend(Unbound<AlphaT, symbol_tag> expr, const unsigned slot) throw() {
            slots[slot].as_symbol = expr.expr;
        }

        inline void extend(Unbound<AlphaT, term_tag> expr, const unsigned slot) throw() {
            slots[slot].as_term = expr.expr;
        }

        inline void extend(Unbound<AlphaT, operator_tag> expr, const unsigned slot) throw() {
            slots[slot].as_operator = expr.expr;
        }

        inline void extend(Unbound<AlphaT, operator_string_tag> expr, const unsigned slot) throw() {
            slots[slot].as_operator_string = expr.expr;
        }

        /// extension of the pattern with non-binding values
        inline void extend(AnyOperator<AlphaT> *, const unsigned) throw() { }

        inline void extend(AnyOperatorString<AlphaT> *, const unsigned) throw() { }

        inline void extend(AnyOperatorStringOfLength<AlphaT> *expr, const unsigned slot) throw() {
            slots[slot].as_length = expr->length;
        }
    };

    /// static allocator for patterns
    template <typename AlphaT>
    helper::BlockAllocator<
        PatternData<AlphaT>,
        PatternData<AlphaT>::ALLOC_GROUP_SIZE
    > PatternData<AlphaT>::pattern_allocator;
}}}

#endif /* Grail_Plus_TDOP_PATTERN_HPP_ */
