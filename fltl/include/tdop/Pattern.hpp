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

            Slot(void) throw() {
                memset(this, 0, sizeof *this);
            }

            ~Slot(void) throw() {
                memset(this, 0, sizeof *this);
            }
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
    class Match<AlphaT,StringT,offset,tdop::category_lb_tag, T> {
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
    class Match<AlphaT,StringT,offset,tdop::category_tag, T> {
    public:
        inline static bool bind(tdop::detail::Slot<AlphaT> *slots, const tdop::Operator<AlphaT> *symbols, const unsigned len) throw() {

            if(1 <= len) {
                tdop::OpaqueCategory<AlphaT> cat;

                return symbols->match(cat)
                    && !symbols->is_bound_category()
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
                    && !symbols->is_symbol_predicate()
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
            if(len >= 1
            && !symbols->is_bound_category()
            && symbols->match(*(slots->as_category))) {

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
            if(len >= 1
            && symbols->match(*(slots->as_symbol))
            && !symbols->is_symbol_predicate()) {

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

    /// unbound symbol predicate + T
    template <typename AlphaT, typename StringT, const unsigned offset, typename T>
    class Match<AlphaT,StringT,offset,tdop::ubound_symbol_predicate_tag, T> {
    public:
        inline static bool bind(tdop::detail::Slot<AlphaT> *slots, const tdop::Operator<AlphaT> *symbols, const unsigned len) throw() {
            if(len >= 1
            && symbols->is_symbol_predicate()
            && symbols->match(*(slots->as_symbol))) {

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
    class Match<AlphaT,StringT,offset,tdop::unbound_operator_tag, T> {
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
            if(!symbols->match(cat)
            || symbols->is_bound_category()) {
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
            if(!symbols->match(sym)
            || symbols->is_symbol_predicate()) {
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

            return symbols->match(*(slots->as_category))
                && !symbols->is_bound_category();
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

            return symbols->match(*(slots->as_symbol))
                && !symbols->is_symbol_predicate();
        }
    };

    /// trailing unbound symbol predicate
    template <typename AlphaT, typename StringT, const unsigned offset>
    class Match<AlphaT,StringT,offset,tdop::ubound_symbol_predicate_tag, void> {
    public:
        inline static bool bind(tdop::detail::Slot<AlphaT> *slots, const tdop::Operator<AlphaT> *symbols, const unsigned len) throw() {
            if(len > 1) {
                return false;
            }

            return symbols->is_symbol_predicate()
                && symbols->match(*(slots->as_symbol));
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

#define FLTL_TDOP_DESTRUCTURE(tag, rule_check, after_reset_check) \
    template <typename AlphaT, typename StringT> \
    class DestructuringBind<AlphaT, tag, StringT> { \
    public: \
        typedef typename TDOP<AlphaT>::rule_type rule_type; \
        inline static bool bind( \
            tdop::detail::PatternData<AlphaT> *pattern, \
            const rule_type &rule \
        ) { \
            rule_check \
            tdop::detail::Slot<AlphaT> *slots(&(pattern->slots[0])); \
            pattern::ResetPattern< \
                AlphaT, \
                StringT, \
                typename pattern::GetFactor<StringT,0>::type, \
                0 \
            >::reset(slots); \
            after_reset_check \
            const tdop::OperatorString<AlphaT> &str(rule.operators()); \
            const unsigned str_len(str.length()); \
            if((1U + str_len) >= (1U + StringT::MIN_NUM_SYMBOLS)) { \
                return Match< \
                    AlphaT, \
                    StringT, \
                    0, \
                    typename pattern::GetFactor<StringT,0>::type, \
                    typename pattern::GetFactor<StringT,1>::type \
                >::bind(slots, 0 == str_len ? 0 : &(str.at(0)), str_len); \
            } \
            return false; \
        } \
    };

    /// destructuring initial rules related to a category, where the category
    /// is unbound
    FLTL_TDOP_DESTRUCTURE(tdop::unbound_category_tag,
    {
        if(!rule.is_initial_rule()) {
            return false;
        }
    },
    {
        *(pattern->category) = rule.category();
        pattern->upper_bound = 0;
    })

    /// destructuring initial rules related to a category, where the category
    /// is bound
    FLTL_TDOP_DESTRUCTURE(tdop::category_tag,
    {
        if(!rule.is_initial_rule()) {
            return false;
        }
    },
    {
        if(rule.category() != *(pattern->category)) {
            return false;
        }
        pattern->upper_bound = 0;
    })

    /// destructuring initial rules related to a category, where the category
    /// is anything and is never bound; operator is meant to signify some fixed,
    /// "small" left-hand side
    FLTL_TDOP_DESTRUCTURE(tdop::any_operator_tag,
    {
        if(!rule.is_initial_rule()) {
            return false;
        }
    },
    {
        pattern->category = 0;
        pattern->upper_bound = 0;
    })

    /// destructuring extension rules related to a category, where the category
    /// is unbound
    FLTL_TDOP_DESTRUCTURE(tdop::unbound_category_lb_tag,
    {
        if(!rule.is_extension_rule()) {
            return false;
        }
    },
    {
        tdop::OpaqueCategory<AlphaT> cat(rule.category());
        tdop::OperatorString<AlphaT> str;
        unsigned upper_bound(0);

        if(!rule.match(upper_bound, str)) {
            return false;
        }

        *(pattern->category) = cat;
        *(pattern->upper_bound) = upper_bound;
    })

    /// destructuring extension rules related to a category, where the category
    /// is bound, but the upper bound is unbound.
    FLTL_TDOP_DESTRUCTURE(tdop::category_lb_tag,
    {
        if(!rule.is_extension_rule()) {
            return false;
        }
    },
    {
        tdop::OpaqueCategory<AlphaT> cat(rule.category());
        tdop::OperatorString<AlphaT> str;
        unsigned upper_bound(0);

        if(!rule.match(upper_bound, str)) {
            return false;
        }

        if(cat != *(pattern->category)) {
            return false;
        }

        *(pattern->upper_bound) = upper_bound;
    })

    /// destructuring extension rules related to a category, where the category
    /// is anything; operator string here is meant to signify a "big" left-hand
    /// side.
    FLTL_TDOP_DESTRUCTURE(tdop::any_operator_string_tag,
    {
        if(!rule.is_extension_rule()) {
            return false;
        }
    },
    {
        tdop::OpaqueCategory<AlphaT> cat(rule.category());
        tdop::OperatorString<AlphaT> str;
        unsigned upper_bound(0);

        if(!rule.match(upper_bound, str)) {
            return false;
        }

        *(pattern->category) = cat;
        *(pattern->upper_bound) = upper_bound;
    })

#undef FLTL_TDOP_DESTRUCTURE
}}

namespace fltl { namespace tdop { namespace detail {

    class EmptyString {
    public:
        enum {
            NEXT_OFFSET = 0
        };
    };

    template <
        typename AlphaT,
        typename CatTagT,
        typename StringT,
        const bool can_append_string // true iff an operator string can be appended
    >
    class PatternBuilder {
    private:

        friend class AnyOperator<AlphaT>;
        friend class AnyOperatorString<AlphaT>;
        friend class OpaqueCategory<AlphaT>;
        friend class Unbound<AlphaT,category_tag>;
        friend class Unbound<AlphaT,category_lb_tag>;
        friend class Bound<AlphaT,category_lb_tag>;
        friend class OpaquePattern<AlphaT>;

        template <typename, typename, typename, const bool>
        friend class PatternBuilder;

        FLTL_TDOP_USE_TYPES(TDOP<AlphaT>);
        typedef PatternBuilder<AlphaT,CatTagT,StringT,can_append_string> self_type;

        PatternData<AlphaT> *pattern;

        PatternBuilder(PatternData<AlphaT> *_pattern) throw()
            : pattern(_pattern)
        {
            PatternData<AlphaT>::incref(pattern);
        }

    public:

        PatternBuilder(const self_type &that)
            : pattern(that.pattern)
        {
            PatternData<AlphaT>::incref(pattern);
        }

        ~PatternBuilder(void) throw() {
            PatternData<AlphaT>::decref(pattern);
            pattern = 0;
        }

        enum {
            NEXT_OFFSET = mpl::IfEqual<
                StringT,void,EmptyString,StringT
            >::type::NEXT_OFFSET
        };

        template <typename T>
        FLTL_FORCE_INLINE
        PatternBuilder<
            AlphaT,
            CatTagT,
            pattern::Catenation<
                StringT,
                pattern::Factor<typename T::tag_type, NEXT_OFFSET>
            >,
            true
        >
        operator+(const T &expr) const throw() {
            pattern->extend(const_cast<T *>(&expr), NEXT_OFFSET);
            return PatternBuilder<
                AlphaT,
                CatTagT,
                pattern::Catenation<
                    StringT,
                    pattern::Factor<typename T::tag_type, NEXT_OFFSET>
                >,
                true
            >(pattern);
        }

        FLTL_FORCE_INLINE
        PatternBuilder<
            AlphaT,
            CatTagT,
            pattern::Catenation<
                StringT,
                pattern::Factor<unbound_operator_string_tag, NEXT_OFFSET>
            >,
            false
        >
        operator+(const Unbound<AlphaT,operator_string_tag> &expr) const throw() {
            FLTL_STATIC_ASSERT(can_append_string);
            pattern->extend(const_cast<Unbound<AlphaT,operator_string_tag> *>(&expr), StringT::NEXT_OFFSET);
            return PatternBuilder<
                AlphaT,
                CatTagT,
                pattern::Catenation<
                    StringT,
                    pattern::Factor<unbound_operator_string_tag, NEXT_OFFSET>
                >,
                false
            >(pattern);
        }

        /// perform an in-line match according to a pattern
        inline bool match(const rule_type &rule) const throw() {
            if(!rule.is_valid()) {
                return false;
            }
            return pattern::DestructuringBind<
                AlphaT,
                CatTagT,
                StringT
            >::bind(pattern, rule);
        }
    };

    template <
        typename AlphaT,
        typename CatTagT
    >
    class PatternBuilder<AlphaT,CatTagT,void,true> {
    private:

        friend class AnyOperator<AlphaT>;
        friend class AnyOperatorString<AlphaT>;
        friend class OpaqueCategory<AlphaT>;
        friend class Unbound<AlphaT,category_tag>;
        friend class Unbound<AlphaT,category_lb_tag>;
        friend class Bound<AlphaT,category_lb_tag>;
        friend class OpaquePattern<AlphaT>;

        FLTL_TDOP_USE_TYPES(TDOP<AlphaT>);
        typedef PatternBuilder<AlphaT,CatTagT,void,true> self_type;

        PatternData<AlphaT> *pattern;

        PatternBuilder(PatternData<AlphaT> *_pattern) throw()
            : pattern(_pattern)
        {
            PatternData<AlphaT>::incref(pattern);
        }

    public:

        PatternBuilder(const self_type &that)
            : pattern(that.pattern)
        {
            PatternData<AlphaT>::incref(pattern);
        }

        ~PatternBuilder(void) throw() {
            PatternData<AlphaT>::decref(pattern);
            pattern = 0;
        }

        template <typename T>
        FLTL_FORCE_INLINE
        PatternBuilder<
            AlphaT,
            CatTagT,
            pattern::Factor<typename T::tag_type, 0>,
            true
        >
        operator->*(const T &expr) const throw() {
            pattern->extend(const_cast<T *>(&expr), 0);
            return PatternBuilder<
                AlphaT,
                CatTagT,
                pattern::Factor<typename T::tag_type, 0>,
                true
            >(pattern);
        }

        FLTL_FORCE_INLINE
        PatternBuilder<
            AlphaT,
            CatTagT,
            pattern::Factor<unbound_operator_string_tag, 0>,
            false
        >
        operator->*(const Unbound<AlphaT,operator_string_tag> &expr) const throw() {
            pattern->extend(const_cast<Unbound<AlphaT,operator_string_tag> *>(&expr), 0);
            return PatternBuilder<
                AlphaT,
                CatTagT,
                pattern::Factor<unbound_operator_string_tag, 0>,
                false
            >(pattern);
        }
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
        friend class Operator<AlphaT>;
        friend class OpaquePattern<AlphaT>;

        friend class AnyOperator<AlphaT>;
        friend class AnyOperatorString<AlphaT>;
        friend class OpaqueCategory<AlphaT>;
        friend class Unbound<AlphaT,category_tag>;
        friend class Unbound<AlphaT,category_lb_tag>;
        friend class Bound<AlphaT,category_lb_tag>;

        template <typename, typename, typename, const bool>
        friend class PatternBuilder;

        template <typename, typename, typename>
        friend class pattern::DestructuringBind;

        FLTL_TDOP_USE_TYPES(TDOP<AlphaT>);

        /// reference counting; needed so that multiple pattern_types can
        /// be passed around, use the same pattern, etc.
        unsigned ref_count;

        /// upper bound; if non-NULL, then the pattern function looks only
        /// at extension rules, and might use this for reading/writing. If
        /// NULL, then initial rules are looked at.
        unsigned *upper_bound;

        /// category pointed to by this data; if NULL then we don't care
        /// about the category, if non-NULL then the pattern-matching
        /// function determines if this is used for reading or for writing
        category_type *category;

        /// slots holding pointers back to pattern data
        Slot<AlphaT> slots[NUM_SLOTS];

        /// allocator for pattern data
        static allocator_type pattern_allocator;

    public:

        /// constructor / destructor

        PatternData(void)
            : ref_count(0U)
            , upper_bound(0)
            , category(0)
            , slots()
        { }

        ~PatternData(void) {
            category = 0;
            upper_bound = 0;
            memset(&(slots[0]), 0, NUM_SLOTS * sizeof(Slot<AlphaT>));
        }

    private:

        /// allocation for binding/reading from a category, for initial
        /// rules
        static self_type *allocate(category_type *category) throw() {
            self_type *pattern(pattern_allocator.allocate());
            pattern->category = category;
            pattern->upper_bound = 0;
            return pattern;
        }

        /// allocation for binding/reading from a category, for initial
        /// rules; the category is unbound
        static self_type *allocate(Unbound<AlphaT,category_tag> *expr) throw() {
            self_type *pattern(pattern_allocator.allocate());
            pattern->category = expr->expr;
            pattern->upper_bound = 0;
            return pattern;
        }

        /// allocation for binding/reading from a category, for extension
        /// rules; the category is unbound
        static self_type *allocate(Unbound<AlphaT,category_lb_tag> *expr) throw() {
            self_type *pattern(pattern_allocator.allocate());
            pattern->category = expr->expr;
            pattern->upper_bound = expr->lower_bound;
            return pattern;
        }

        /// allocation for binding/reading from a category, for extension
        /// rules; the category is bound
        static self_type *allocate(Bound<AlphaT,category_lb_tag> *expr) throw() {
            self_type *pattern(pattern_allocator.allocate());
            pattern->category = expr->expr;
            pattern->upper_bound = expr->lower_bound;
            return pattern;
        }

        /// allocation for binding/reading from a category, for initial
        /// rules; the category is ignored
        static self_type *allocate(AnyOperator<AlphaT> *) throw() {
            self_type *pattern(pattern_allocator.allocate());
            pattern->category = 0;
            pattern->upper_bound = 0;
            return pattern;
        }

        /// allocation for binding/reading from a category, for extension
        /// rules; the category is ignored
        static self_type *allocate(AnyOperatorString<AlphaT> *) throw() {
            self_type *pattern(pattern_allocator.allocate());
            pattern->category = 0;
            pattern->upper_bound = 0;
            return pattern;
        }

        /// reference counting

        static void incref(self_type *self) throw() {
            if(0 != self) {
                ++(self->ref_count);
            }
        }

        static void decref(PatternData<AlphaT> *self) throw() {
            if(0 != self) {
                self->ref_count -= 1U;
                if(0 == self->ref_count) {
                    pattern_allocator.deallocate(self);
                }
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

        inline void extend(Unbound<AlphaT, symbol_tag> *expr, const unsigned slot) throw() {
            slots[slot].as_symbol = expr->expr;
        }

        inline void extend(Unbound<AlphaT, ubound_symbol_predicate_tag> *expr, const unsigned slot) throw() {
            slots[slot].as_symbol = expr->expr;
        }

        inline void extend(Unbound<AlphaT, term_tag> *expr, const unsigned slot) throw() {
            slots[slot].as_term = expr->expr;
        }

        inline void extend(Unbound<AlphaT, operator_tag> *expr, const unsigned slot) throw() {
            slots[slot].as_operator = expr->expr;
        }

        inline void extend(Unbound<AlphaT, category_tag> *expr, const unsigned slot) throw() {
            slots[slot].as_category = expr->expr;
        }

        inline void extend(Unbound<AlphaT, category_lb_tag> *expr, const unsigned slot) throw() {
            slots[slot].as_category = expr->expr;
            slots[slot+1].as_lower_bound = expr->lower_bound;
        }

        inline void extend(Bound<AlphaT, category_lb_tag> *expr, const unsigned slot) throw() {
            slots[slot].as_category = expr->expr;
            slots[slot+1].as_lower_bound = expr->lower_bound;
        }

        inline void extend(Unbound<AlphaT, operator_string_tag> *expr, const unsigned slot) throw() {
            slots[slot].as_operator_string = expr->expr;
        }

        /// extension of the pattern with non-binding values
        inline void extend(AnyOperator<AlphaT> *, const unsigned) throw() { }

        inline void extend(AnyOperatorString<AlphaT> *, const unsigned) throw() { }

        inline void extend(AnyOperatorStringOfLength<AlphaT> *expr, const unsigned slot) throw() {
            slots[slot].as_length = expr->length;
        }
    };

    /// initialize static allocator for patterns
    template <typename AlphaT>
    typename PatternData<AlphaT>::allocator_type
    PatternData<AlphaT>::pattern_allocator;
}}}

#endif /* Grail_Plus_TDOP_PATTERN_HPP_ */
