/*
 * Pattern.hpp
 *
 *  Created on: Jan 27, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_PATTERN_HPP_
#define FLTL_PATTERN_HPP_

#ifndef _FLTL_CFG_UNBOUND
#define _FLTL_CFG_UNBOUND(tag) cfg::Unbound<AlphaT,tag>
#endif

#define FLTL_CFG_PRODUCTION_PATTERN_INIT(type, tag, state) \
    FLTL_FORCE_INLINE detail::PatternBuilder<AlphaT, VarTagT, detail::Factor<tag,0>, state> \
    operator->*(type &expr) throw() { \
        pattern->extend(&expr, 0); \
        return detail::PatternBuilder<AlphaT, VarTagT, detail::Factor<tag,0>, state>( \
            pattern \
        ); \
    } \
    FLTL_FORCE_INLINE detail::PatternBuilder<AlphaT, VarTagT, detail::Factor<tag,0>, state> \
    operator->*(const type &expr) const throw() { \
        pattern->extend(const_cast<type *>(&expr), 0); \
        return detail::PatternBuilder<AlphaT, VarTagT, detail::Factor<tag,0>, state>( \
            pattern \
        ); \
    }

#define FLTL_CFG_UNBOUND_PRODUCTION_PATTERN_INIT(type, tag, state) \
    FLTL_CFG_PRODUCTION_PATTERN_INIT(_FLTL_CFG_UNBOUND(tag), unbound_ ## tag, state)

/*
 *     FLTL_FORCE_INLINE detail::PatternBuilder<AlphaT, VarTagT, detail::Catenation<StringT,Factor<tag, StringT::NEXT_OFFSET> >, state> \
    operator+(type &expr) throw() { \
        pattern->extend(&expr, StringT::NEXT_OFFSET); \
        return detail::PatternBuilder<AlphaT, VarTagT, detail::Catenation<StringT,Factor<tag, StringT::NEXT_OFFSET> >, state>( \
            pattern \
        ); \
    } \
 */

#define FLTL_CFG_PRODUCTION_PATTERN_EXTEND(type, tag, state) \
    FLTL_FORCE_INLINE detail::PatternBuilder<AlphaT, VarTagT, detail::Catenation<StringT,Factor<tag, StringT::NEXT_OFFSET> >, state> \
    operator+(const type &expr) const throw() { \
        pattern->extend(const_cast<type *>(&expr), StringT::NEXT_OFFSET); \
        return detail::PatternBuilder<AlphaT, VarTagT, detail::Catenation<StringT,Factor<tag, StringT::NEXT_OFFSET> >, state>( \
            pattern \
        ); \
    }

#define FLTL_CFG_UNBOUND_PRODUCTION_PATTERN_EXTEND(type,tag,state) \
    FLTL_CFG_PRODUCTION_PATTERN_EXTEND(_FLTL_CFG_UNBOUND(tag),unbound_ ## tag,state)


namespace fltl { namespace lib { namespace cfg {

    namespace detail {

        template <typename AlphaT>
        union Slot {
        public:

            typedef typename CFG<AlphaT>::variable_type variable_type;
            typedef typename CFG<AlphaT>::terminal_type terminal_type;
            typedef typename CFG<AlphaT>::symbol_type symbol_type;
            typedef typename CFG<AlphaT>::symbol_string_type symbol_string_type;

            symbol_type *as_symbol;
            variable_type *as_variable;
            terminal_type *as_terminal;
            symbol_string_type *as_symbol_string;
        };

        ///

        template <typename T, const unsigned offset>
        class Factor {
        public:
            enum {
                WIDTH = 1,
                MIN_NUM_SYMBOLS = 1,
                OFFSET = offset,
                NEXT_OFFSET = OFFSET + WIDTH
            };
        };

        template <const unsigned offset>
        class Factor<symbol_string_tag,offset> {
        public:
            enum {
                WIDTH = 1,
                MIN_NUM_SYMBOLS = 0,
                OFFSET = offset,
                NEXT_OFFSET = OFFSET + WIDTH
            };
        };

        template <const unsigned offset>
        class Factor<any_symbol_string_tag,offset> {
        public:
            enum {
                WIDTH = 1,
                MIN_NUM_SYMBOLS = 0,
                OFFSET = offset,
                NEXT_OFFSET = OFFSET + WIDTH
            };
        };

        template <const unsigned offset>
        class Factor<unbound_symbol_string_tag,offset> {
        public:
            enum {
                WIDTH = 1,
                MIN_NUM_SYMBOLS = 0,
                OFFSET = offset,
                NEXT_OFFSET = OFFSET + WIDTH
            };
        };

        /// concatenate facotrs/concatenations together

        template <typename T0, typename T1>
        class Catenation {
        public:
            enum {
                WIDTH = T0::WIDTH + T1::WIDTH,
                OFFSET = T0::OFFSET,
                MIN_NUM_SYMBOLS = T0::MIN_NUM_SYMBOLS + T1::MIN_NUM_SYMBOLS,
                NEXT_OFFSET = OFFSET + WIDTH
            };
        };

        /// extract out a specific type stored in a factor from the pattern
        /// type tree that represents the types of the factors in the pattern
        /// string

        template <typename T, const unsigned offset>
        class GetFactor {
        public:
            typedef void type;
        };

        template <typename T0, typename T1, const unsigned offset>
        class GetFactor<Catenation<T0,T1>, offset> {
        public:
            typedef typename GetFactor<
                typename mpl::IfTrue<(offset >= T1::OFFSET),T1,T0>::type,
                offset
            >::type type;
        };

        template <typename T, const unsigned offset>
        class GetFactor<Factor<T, offset>, offset> {
        public:
            typedef T type;
        };

        /// extract the minimum number of symbols that the pattern must
        /// match that come after a specific offset
        template <typename T, const unsigned offset>
        class GetMinNumSymbolsAfter {
        public:
            enum {
                RESULT = 0
            };
        };

        template <typename T0, typename T1, const unsigned offset>
        class GetMinNumSymbolsAfter<Catenation<T0, T1>, offset> {
        public:
            enum {
                RESULT = (
                    GetMinNumSymbolsAfter<T0,offset>::RESULT +
                    GetMinNumSymbolsAfter<T1,offset>::RESULT
                )
            };
            typedef typename GetFactor<
                typename mpl::IfTrue<(offset > T1::OFFSET),T1,T0>::type,
                offset
            >::type type;
        };

        template <typename T, const unsigned o, const unsigned offset>
        class GetMinNumSymbolsAfter<Factor<T,o>, offset> {
        public:
            enum {
                RESULT = o > offset ? Factor<T,o>::MIN_NUM_SYMBOLS : 0
            };
        };

        /// match symbols from the pattern

        template <typename AlphaT, typename StringT, const unsigned offset, typename T0, typename T1>
        class Match2;

        /// symbol + T
        template <typename AlphaT, typename StringT, const unsigned offset, typename T>
        class Match2<AlphaT,StringT,offset,symbol_tag, T> {
        public:
            inline static bool bind(Slot<AlphaT> *slots, const Symbol<AlphaT> *symbols, const unsigned len) throw() {
                if((1 <= len) && (*symbols == *(slots->as_symbol))) {
                    return Match2<
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

        /// terminal + T
        template <typename AlphaT, typename StringT, const unsigned offset, typename T>
        class Match2<AlphaT,StringT,offset,terminal_tag, T> {
        public:
            inline static bool bind(Slot<AlphaT> *slots, const Symbol<AlphaT> *symbols, const unsigned len) throw() {
                if((1 <= len) && (*symbols == *(slots->as_symbol))) {
                    return Match2<
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

        /// variable + T
        template <typename AlphaT, typename StringT, const unsigned offset, typename T>
        class Match2<AlphaT,StringT,offset,variable_tag, T> {
        public:
            inline static bool bind(Slot<AlphaT> *slots, const Symbol<AlphaT> *symbols, const unsigned len) throw() {
                if((1 <= len) && (*symbols == *(slots->as_symbol))) {
                    return Match2<
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

        /// symbol string + T
        template <typename AlphaT, typename StringT, const unsigned offset, typename T>
        class Match2<AlphaT,StringT,offset,symbol_string_tag, T> {
        public:
            inline static bool bind(Slot<AlphaT> *slots, const Symbol<AlphaT> *symbols, const unsigned len) throw() {
                const unsigned str_len(slots->as_symbol_string->length());
                enum {
                    MIN_REQUIRED_LEN = 1 + GetMinNumSymbolsAfter<StringT,offset>::RESULT
                };

                if(str_len > len || MIN_REQUIRED_LEN > (1 + (len - str_len))) {
                    return false;
                }

                if(0 != str_len) {
                    const bool prefix_is_str(SymbolString<AlphaT>::symbol_memcmp(
                        symbols,
                        &(slots->as_symbol_string->at(0)),
                        symbols + str_len - 1
                    ));

                    if(!prefix_is_str) {
                        return false;
                    }
                }

                return Match2<
                    AlphaT,
                    StringT,
                    offset + 1,
                    T,
                    typename GetFactor<StringT,offset + 2>::type
                >::bind(slots + 1, symbols + str_len, len - str_len);
            }
        };

        /// ignore + T
        template <typename AlphaT, typename StringT, const unsigned offset, typename T>
        class Match2<AlphaT,StringT,offset,any_symbol_tag, T> {
        public:
            inline static bool bind(Slot<AlphaT> *slots, const Symbol<AlphaT> *symbols, const unsigned len) throw() {
                return (len >= 1) && Match2<
                    AlphaT,
                    StringT,
                    offset + 1,
                    T,
                    typename GetFactor<StringT,offset + 2>::type
                >::bind(slots + 1, symbols + 1, len - 1);
            }
        };

        /// unbound symbol + T
        template <typename AlphaT, typename StringT, const unsigned offset, typename T>
        class Match2<AlphaT,StringT,offset,unbound_symbol_tag, T> {
        public:
            inline static bool bind(Slot<AlphaT> *slots, const Symbol<AlphaT> *symbols, const unsigned len) throw() {
                if(len >= 1) {
                    *(slots->as_symbol) = *symbols;

                    return Match2<
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

        /// unbound variable + T
        template <typename AlphaT, typename StringT, const unsigned offset, typename T>
        class Match2<AlphaT,StringT,offset,unbound_variable_tag, T> {
        public:
            inline static bool bind(Slot<AlphaT> *slots, const Symbol<AlphaT> *symbols, const unsigned len) throw() {
                if(len >= 1 && symbols->is_variable()) {
                    *(slots->as_symbol) = *helper::unsafe_cast<typename CFG<AlphaT>::variable_type *>(symbols);

                    return Match2<
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

        /// unbound terminal + T
        template <typename AlphaT, typename StringT, const unsigned offset, typename T>
        class Match2<AlphaT,StringT,offset,unbound_terminal_tag, T> {
        public:
            inline static bool bind(Slot<AlphaT> *slots, const Symbol<AlphaT> *symbols, const unsigned len) throw() {
                if(len >= 1 && symbols->is_terminal()) {
                    *(slots->as_terminal) = *helper::unsafe_cast<typename CFG<AlphaT>::terminal_type *>(symbols);

                    return Match2<
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

        /// unbound symbol string + T
        template <typename AlphaT, typename StringT, const unsigned offset, typename T>
        class Match2<AlphaT,StringT,offset,unbound_symbol_string_tag, T> {
        public:
            inline static bool bind(Slot<AlphaT> *slots, const Symbol<AlphaT> *symbols, const unsigned len) throw() {

                const unsigned max(len - GetMinNumSymbolsAfter<StringT,offset>::RESULT);
                for(unsigned i(0); i <= max; ++i) {

                    SymbolString<AlphaT> str(symbols, i);
                    *(slots->as_symbol_string) = str;

                    const bool matched_next_at_i(Match2<
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
                SymbolString<AlphaT> epsilon;
                *(slots->as_symbol_string) = epsilon;

                return false;
            }
        };

        /// any symbol string + T
        template <typename AlphaT, typename StringT, const unsigned offset, typename T>
        class Match2<AlphaT,StringT,offset,any_symbol_string_tag, T> {
        public:
            inline static bool bind(Slot<AlphaT> *slots, const Symbol<AlphaT> *symbols, const unsigned len) throw() {

                const unsigned max(len - GetMinNumSymbolsAfter<StringT,offset>::RESULT);
                for(unsigned i(0); i <= max; ++i) {
                    const bool matched_next_at_i(Match2<
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

        /// nothing to match, success.
        template <typename AlphaT, typename StringT, const unsigned offset>
        class Match2<AlphaT,StringT,offset, void, void> {
        public:
            inline static bool bind(Slot<AlphaT> *, const Symbol<AlphaT> *, const unsigned len) throw() {
                return 0 == len;
            }
        };

        /// trailing symbol
        template <typename AlphaT, typename StringT, const unsigned offset>
        class Match2<AlphaT,StringT,offset,symbol_tag, void> {
        public:
            inline static bool bind(Slot<AlphaT> *slots, const Symbol<AlphaT> *symbols, const unsigned len) throw() {
                return (1 == len) && (*symbols == *(slots->as_symbol));
            }
        };

        /// trailing terminal
        template <typename AlphaT, typename StringT, const unsigned offset>
        class Match2<AlphaT,StringT,offset,terminal_tag, void> {
        public:
            inline static bool bind(Slot<AlphaT> *slots, const Symbol<AlphaT> *symbols, const unsigned len) throw() {
                return (1 == len) && (*symbols == *(slots->as_symbol));
            }
        };

        /// trailing variable
        template <typename AlphaT, typename StringT, const unsigned offset>
        class Match2<AlphaT,StringT,offset,variable_tag, void> {
        public:
            inline static bool bind(Slot<AlphaT> *slots, const Symbol<AlphaT> *symbols, const unsigned len) throw() {
                return (1 == len) && (*symbols == *(slots->as_symbol));
            }
        };

        /// trailing symbol string
        template <typename AlphaT, typename StringT, const unsigned offset>
        class Match2<AlphaT,StringT,offset,symbol_string_tag, void> {
        public:
            inline static bool bind(Slot<AlphaT> *slots, const Symbol<AlphaT> *symbols, const unsigned len) throw() {

                const unsigned str_len(slots->as_symbol_string->length());

                if(str_len != len) {
                    return false;
                } else if(0 == str_len) {
                    return true;
                }

                return SymbolString<AlphaT>::symbol_memcmp(
                    symbols,
                    &(slots->as_symbol_string->at(0)),
                    symbols + str_len - 1
                );
            }
        };

        /// trailing ignore
        template <typename AlphaT, typename StringT, const unsigned offset>
        class Match2<AlphaT,StringT,offset,any_symbol_tag, void> {
        public:
            inline static bool bind(Slot<AlphaT> *, const Symbol<AlphaT> *, const unsigned len) throw() {
                return 1 == len;
            }
        };

        /// trailing unbound symbol
        template <typename AlphaT, typename StringT, const unsigned offset>
        class Match2<AlphaT,StringT,offset,unbound_symbol_tag, void> {
        public:
            inline static bool bind(Slot<AlphaT> *slots, const Symbol<AlphaT> *symbols, const unsigned len) throw() {
                if(len > 1) {
                    return false;
                }

                *(slots->as_symbol) = *symbols;

                return true;
            }
        };

        /// trailing unbound variable
        template <typename AlphaT, typename StringT, const unsigned offset>
        class Match2<AlphaT,StringT,offset,unbound_variable_tag, void> {
        public:
            inline static bool bind(Slot<AlphaT> *slots, const Symbol<AlphaT> *symbols, const unsigned len) throw() {
                if(len > 1 || !symbols->is_variable()) {
                    return false;
                }

                *(slots->as_variable) = *helper::unsafe_cast<typename CFG<AlphaT>::variable_type *>(symbols);

                return true;
            }
        };

        /// trailing unbound terminal
        template <typename AlphaT, typename StringT, const unsigned offset>
        class Match2<AlphaT,StringT,offset,unbound_terminal_tag, void> {
        public:
            inline static bool bind(Slot<AlphaT> *slots, const Symbol<AlphaT> *symbols, const unsigned len) throw() {
                if(len > 1 || !symbols->is_terminal()) {
                    return false;
                }

                *(slots->as_terminal) = *helper::unsafe_cast<typename CFG<AlphaT>::terminal_type *>(symbols);

                return true;
            }
        };

        /// trailing unbound symbol string
        template <typename AlphaT, typename StringT, const unsigned offset>
        class Match2<AlphaT,StringT,offset,unbound_symbol_string_tag, void> {
        public:
            inline static bool bind(Slot<AlphaT> *slots, const Symbol<AlphaT> *symbols, const unsigned len) throw() {
                SymbolString<AlphaT> str(symbols, len);
                *(slots->as_symbol_string) = str;
                return true;
            }
        };

        /// trailing any symbol string
        template <typename AlphaT, typename StringT, const unsigned offset>
        class Match2<AlphaT,StringT,offset,any_symbol_string_tag, void> {
        public:
            inline static bool bind(Slot<AlphaT> *, const Symbol<AlphaT> *, const unsigned) throw() {
                return true;
            }
        };

        /// reset the unbound strings in the pattern

        template <typename AlphaT, typename StringT, typename CurrT,const unsigned offset>
        class ResetPattern {
        public:
            inline static void reset(Slot<AlphaT> *slots) throw() {
                ResetPattern<
                    AlphaT,
                    StringT,
                    typename GetFactor<StringT,offset + 1>::type,
                    offset + 1
                >::reset(slots);
            }
        };

        template <typename AlphaT, typename StringT,const unsigned offset>
        class ResetPattern<AlphaT,StringT,void,offset> {
        public:
            inline static void reset(Slot<AlphaT> *) throw() { }
        };

        template <typename AlphaT, typename StringT, const unsigned offset>
        class ResetPattern<AlphaT,StringT,unbound_symbol_string_tag,offset> {
        public:
            inline static void reset(Slot<AlphaT> *slots) throw() {

                // reset the unbound symbol string
                SymbolString<AlphaT> epsilon;
                *(slots[offset].as_symbol_string) = epsilon;

                ResetPattern<
                    AlphaT,
                    StringT,
                    typename GetFactor<StringT,offset + 1>::type,
                    offset + 1
                >::reset(slots);
            }
        };

        /// decrease reference counters in the pattern
        /*
        template <typename AlphaT, typename StringT, typename CurrT,const unsigned offset>
        class DecRefCounts {
        public:
            inline static void decref(Slot<AlphaT> *slots) throw() {
                ResetPattern<
                    AlphaT,
                    StringT,
                    typename GetFactor<StringT,offset + 1>::type,
                    offset + 1
                >::reset(slots);
            }
        };

        template <typename AlphaT, typename StringT,const unsigned offset>
        class DecRefCounts<AlphaT,StringT,void,offset> {
        public:
            inline static void decref(Slot<AlphaT> *) throw() { }
        };

        template <typename AlphaT, typename StringT, const unsigned offset>
        class DecRefCounts<AlphaT,StringT,unbound_symbol_string_tag,offset> {
        public:
            inline static void decref(Slot<AlphaT> *slots) throw() {

                SymbolString<AlphaT>::decref(slots[offset].as_symbol_string);

                ResetPattern<
                    AlphaT,
                    StringT,
                    typename GetFactor<StringT,offset + 1>::type,
                    offset + 1
                >::reset(slots);
            }
        };

        template <typename AlphaT, typename StringT, const unsigned offset>
        class DecRefCounts<AlphaT,StringT,symbol_string_tag,offset> {
        public:
            inline static void decref(Slot<AlphaT> *slots) throw() {

                SymbolString<AlphaT>::decref(slots[offset].as_symbol_string);

                ResetPattern<
                    AlphaT,
                    StringT,
                    typename GetFactor<StringT,offset + 1>::type,
                    offset + 1
                >::reset(slots);
            }
        };
        */

        template <typename AlphaT, typename VarTagT, typename StringT>
        class DestructuringBind;

        /// check the variable of the pattern
        template <typename AlphaT, typename StringT>
        class DestructuringBind<AlphaT, variable_tag, StringT> {
        public:

            typedef typename CFG<AlphaT>::production_type production_type;

            inline static bool bind(
                PatternData<AlphaT> *pattern,
                const production_type &prod
            ) {
                if(!prod.is_valid()) {
                    return false;
                }

                Slot<AlphaT> *slots(&(pattern->slots[0]));

                ResetPattern<
                    AlphaT,
                    StringT,
                    typename GetFactor<StringT,0>::type,
                    0
                >::reset(slots);

                if(*(pattern->var) != prod.variable()) {
                    return false;
                }

                const SymbolString<AlphaT> &str(prod.symbols());
                const unsigned str_len(str.length());

                if((1U + str_len) >= (1U + StringT::MIN_NUM_SYMBOLS)) {
                    return Match2<
                        AlphaT,
                        StringT,
                        0,
                        typename GetFactor<StringT,0>::type,
                        typename GetFactor<StringT,1>::type
                    >::bind(slots, 0 == str_len ? 0 : &(str.at(0)), str_len);
                }

                return false;
            }
        };

        /// bind the variable
        template <typename AlphaT, typename StringT>
        class DestructuringBind<AlphaT, unbound_variable_tag, StringT> {
        public:

            typedef typename CFG<AlphaT>::production_type production_type;

            inline static bool bind(
                PatternData<AlphaT> *pattern,
                const production_type &prod
            ) {
                if(!prod.is_valid()) {
                    return false;
                }

                Slot<AlphaT> *slots(&(pattern->slots[0]));

                ResetPattern<
                    AlphaT,
                    StringT,
                    typename GetFactor<StringT,0>::type,
                    0
                >::reset(slots);

                *(pattern->var) = prod.variable();

                const SymbolString<AlphaT> &str(prod.symbols());
                const unsigned str_len(str.length());

                if((1U + str_len) >= (1U + StringT::MIN_NUM_SYMBOLS)) {
                    return Match2<
                        AlphaT,
                        StringT,
                        0,
                        typename GetFactor<StringT,0>::type,
                        typename GetFactor<StringT,1>::type
                    >::bind(slots, 0 == str_len ? 0 : &(str.at(0)), str_len);
                }

                return false;
            }
        };

        /// ignore the variable
        template <typename AlphaT, typename StringT>
        class DestructuringBind<AlphaT, any_symbol_tag, StringT> {
        public:

            typedef typename CFG<AlphaT>::production_type production_type;

            inline static bool bind(
                PatternData<AlphaT> *pattern,
                const production_type &prod
            ) {
                Slot<AlphaT> *slots(&(pattern->slots[0]));

                ResetPattern<
                    AlphaT,
                    StringT,
                    typename GetFactor<StringT,0>::type,
                    0
                >::reset(slots);

                const SymbolString<AlphaT> &str(prod.symbols());
                const unsigned str_len(str.length());

                if((1U + str_len) >= (1U + StringT::MIN_NUM_SYMBOLS)) {
                    return Match2<
                        AlphaT,
                        StringT,
                        0,
                        typename GetFactor<StringT,0>::type,
                        typename GetFactor<StringT,1>::type
                    >::bind(slots, 0 == str_len ? 0 : &(str.at(0)), str_len);
                }

                return false;
            }
        };

        template <typename AlphaT, typename VarTagT, typename StringT>
        class PatternBuilder<AlphaT,VarTagT,StringT,0U> {
        public:

            enum {
                IS_BOUND_TO_VAR = mpl::IfTypesEqual<VarTagT,variable_tag>::RESULT
            };

            friend class CFG<AlphaT>;
            friend class OpaquePattern<AlphaT>;

            template <typename, typename>
            friend class PatternGenerator;

            typedef typename CFG<AlphaT>::variable_type variable_type;
            typedef typename CFG<AlphaT>::terminal_type terminal_type;
            typedef typename CFG<AlphaT>::symbol_type symbol_type;
            typedef typename CFG<AlphaT>::symbol_string_type symbol_string_type;
            typedef typename CFG<AlphaT>::production_type production_type;

            PatternData<AlphaT> *pattern;

            FLTL_FORCE_INLINE PatternBuilder(PatternData<AlphaT> *_pattern)
                : pattern(_pattern)
            {
                PatternData<AlphaT>::incref(pattern);

                /*pattern->dec_string_ref_counts = &(DecRefCounts<
                    AlphaT,
                    StringT,
                    typename GetFactor<StringT,0>::type,
                    0
                >::decref);*/
            }

            ~PatternBuilder(void) throw() {
                PatternData<AlphaT>::decref(pattern);
                pattern = 0;
            }

            FLTL_CFG_PRODUCTION_PATTERN_EXTEND(variable_type, variable_tag, 0)
            FLTL_CFG_UNBOUND_PRODUCTION_PATTERN_EXTEND(variable_type, variable_tag, 0)

            FLTL_CFG_PRODUCTION_PATTERN_EXTEND(AnySymbol<AlphaT>, any_symbol_tag, 0)
            FLTL_CFG_PRODUCTION_PATTERN_EXTEND(AnySymbolString<AlphaT>, any_symbol_string_tag, 1)

            FLTL_CFG_PRODUCTION_PATTERN_EXTEND(terminal_type, terminal_tag, 0)
            FLTL_CFG_UNBOUND_PRODUCTION_PATTERN_EXTEND(terminal_type, terminal_tag, 0)

            FLTL_CFG_PRODUCTION_PATTERN_EXTEND(symbol_type, symbol_tag, 0)
            FLTL_CFG_UNBOUND_PRODUCTION_PATTERN_EXTEND(symbol_type, symbol_tag, 0)

            FLTL_CFG_PRODUCTION_PATTERN_EXTEND(symbol_string_type, symbol_string_tag, 0)
            FLTL_CFG_UNBOUND_PRODUCTION_PATTERN_EXTEND(symbol_string_type, symbol_string_tag, 1)

            inline bool match(const production_type &prod) throw() {
                return DestructuringBind<
                    AlphaT,
                    VarTagT,
                    StringT
                >::bind(pattern, prod);
            }

        private:

            inline static bool static_match(
                cfg::PatternData<AlphaT> *_pattern,
                const production_type &prod
            ) throw() {

                return DestructuringBind<
                    AlphaT,
                    VarTagT,
                    StringT
                >::bind(_pattern, prod);
            }
        };

        template <typename AlphaT, typename VarTagT, typename StringT>
        class PatternBuilder<AlphaT,VarTagT,StringT,1U> {
        public:

            enum {
                IS_BOUND_TO_VAR = mpl::IfTypesEqual<VarTagT,variable_tag>::RESULT
            };

            friend class CFG<AlphaT>;
            friend class OpaquePattern<AlphaT>;

            template <typename, typename>
            friend class PatternGenerator;

            typedef typename CFG<AlphaT>::variable_type variable_type;
            typedef typename CFG<AlphaT>::terminal_type terminal_type;
            typedef typename CFG<AlphaT>::symbol_type symbol_type;
            typedef typename CFG<AlphaT>::symbol_string_type symbol_string_type;
            typedef typename CFG<AlphaT>::production_type production_type;

            PatternData<AlphaT> *pattern;

            FLTL_FORCE_INLINE PatternBuilder(PatternData<AlphaT> *_pattern) throw()
                : pattern(_pattern)
            {
                PatternData<AlphaT>::incref(pattern);

                /*pattern->dec_string_ref_counts = &(DecRefCounts<
                    AlphaT,
                    StringT,
                    typename GetFactor<StringT,0>::type,
                    0
                >::decref);*/
            }

            ~PatternBuilder(void) throw() {
                PatternData<AlphaT>::decref(pattern);
                pattern = 0;
            }

            FLTL_CFG_PRODUCTION_PATTERN_EXTEND(variable_type, variable_tag, 0)
            FLTL_CFG_UNBOUND_PRODUCTION_PATTERN_EXTEND(variable_type, variable_tag, 0)

            FLTL_CFG_PRODUCTION_PATTERN_EXTEND(terminal_type, terminal_tag, 0)
            FLTL_CFG_UNBOUND_PRODUCTION_PATTERN_EXTEND(terminal_type, terminal_tag, 0)

            FLTL_CFG_PRODUCTION_PATTERN_EXTEND(symbol_type, symbol_tag, 0)
            FLTL_CFG_UNBOUND_PRODUCTION_PATTERN_EXTEND(symbol_type, symbol_tag, 0)

            FLTL_CFG_PRODUCTION_PATTERN_EXTEND(AnySymbol<AlphaT>, any_symbol_tag, 0)

            FLTL_CFG_PRODUCTION_PATTERN_EXTEND(symbol_string_type, any_symbol_string_tag, 0)

            inline bool match(const production_type &prod) throw() {
                return DestructuringBind<
                    AlphaT,
                    VarTagT,
                    StringT
                >::bind(pattern, prod);
            }

        private:

            inline static bool static_match(
                cfg::PatternData<AlphaT> *_pattern,
                const production_type &prod
            ) throw() {
                return DestructuringBind<
                    AlphaT,
                    VarTagT,
                    StringT
                >::bind(_pattern, prod);
            }
        };
    }

    template <typename AlphaT>
    class PatternData {
    private:

        typedef typename CFG<AlphaT>::variable_type variable_type;
        typedef typename CFG<AlphaT>::terminal_type terminal_type;
        typedef typename CFG<AlphaT>::symbol_type symbol_type;
        typedef typename CFG<AlphaT>::symbol_string_type symbol_string_type;
        typedef PatternData<AlphaT> self_type;

        friend class CFG<AlphaT>;
        friend class OpaquePattern<AlphaT>;
        friend class helper::BlockAllocator<self_type>;
        friend class detail::SimpleGenerator<AlphaT>;

        template <typename, typename>
        friend class detail::PatternGenerator;

        template <typename,typename,typename,const unsigned>
        friend class detail::PatternBuilder;

        template <typename, typename, typename>
        friend class detail::DestructuringBind;

        enum {
            NUM_SLOTS = 8U
        };

        /// reference counter so we can pass the pattern around
        unsigned ref_count;

        /// variable pointed to by this data; if we don't care about the
        /// variable then it's left as zero
        variable_type *var;

        /// slots holding pointers back to pattern data
        detail::Slot<AlphaT> slots[NUM_SLOTS];

        /// function to call when pattern is deallocated to properly free
        /// up reference counts to related symbol strings
        //void (*dec_string_ref_counts)(detail::Slot<AlphaT> *);

        /// allocator for patterns
        static helper::BlockAllocator<self_type, 8U> pattern_allocator;

        //static void no_string_ref_counts(detail::Slot<AlphaT> *) throw() { }

    public:

        PatternData(void)
            : ref_count(0)
            , var(0)
            //, dec_string_ref_counts(&no_string_ref_counts)
        {
            memset(slots, 0, sizeof(detail::Slot<AlphaT>) * NUM_SLOTS);
        }

        ~PatternData(void) throw() {
            //dec_string_ref_counts(&(slots[0]));
            var = 0;
            //dec_string_ref_counts = &no_string_ref_counts;
        }

        inline void extend(symbol_type *expr, const unsigned slot) throw() {
            slots[slot].as_symbol = expr;
        }

        inline void extend(symbol_string_type *expr, const unsigned slot) throw() {
            slots[slot].as_symbol_string = expr;
            //SymbolString<AlphaT>::incref(expr);
        }

        inline void extend(Unbound<AlphaT, symbol_tag> *expr, const unsigned slot) throw() {
            slots[slot].as_symbol = expr->symbol;
        }

        inline void extend(Unbound<AlphaT, terminal_tag> *expr, const unsigned slot) throw() {
            slots[slot].as_terminal = expr->symbol;
        }

        inline void extend(Unbound<AlphaT, variable_tag> *expr, const unsigned slot) throw() {
            slots[slot].as_variable = expr->symbol;
        }

        inline void extend(Unbound<AlphaT, symbol_string_tag> *expr, const unsigned slot) throw() {
            slots[slot].as_symbol_string = expr->string;
            //SymbolString<AlphaT>::incref(expr->string);
        }

        inline void extend(AnySymbol<AlphaT> *, const unsigned) throw() { }

        inline void extend(AnySymbolString<AlphaT> *, const unsigned) throw() { }

    public:

        static self_type *allocate(variable_type *_var) throw() {
            self_type *self(pattern_allocator.allocate());
            self->var = _var;
            return self;
        }

        static self_type *allocate(Unbound<AlphaT, variable_tag> *_var) throw() {
            self_type *self(pattern_allocator.allocate());
            self->var = _var->symbol;
            return self;
        }

        static self_type *allocate(AnySymbol<AlphaT> *) throw() {
            return pattern_allocator.allocate();
        }

        static void incref(self_type *self) throw() {
            ++(self->ref_count);
        }

        static void decref(PatternData<AlphaT> *self) throw() {
            if(0 == --(self->ref_count)) {
                pattern_allocator.deallocate(self);
            }
        }
    };

    template <typename AlphaT>
    helper::BlockAllocator<
        PatternData<AlphaT>,
        8U
    > PatternData<AlphaT>::pattern_allocator;

    template <typename AlphaT, typename VarTagT>
    class Pattern {
    private:

        friend class CFG<AlphaT>;

        typedef typename CFG<AlphaT>::variable_type variable_type;
        typedef typename CFG<AlphaT>::terminal_type terminal_type;
        typedef typename CFG<AlphaT>::symbol_type symbol_type;
        typedef typename CFG<AlphaT>::symbol_string_type symbol_string_type;

        typedef Pattern<AlphaT,VarTagT> self_type;

        /// the actual data of the pattern
        PatternData<AlphaT> *pattern;

    public:

        Pattern(PatternData<AlphaT> *_pattern) throw()
            : pattern(_pattern)
        {
            PatternData<AlphaT>::incref(pattern);
        }

        ~Pattern(void) throw() {
            PatternData<AlphaT>::decref(pattern);
            pattern = 0;
        }

        FLTL_CFG_PRODUCTION_PATTERN_INIT(variable_type, variable_tag, 0)
        FLTL_CFG_UNBOUND_PRODUCTION_PATTERN_INIT(variable_type, variable_tag, 0)

        FLTL_CFG_PRODUCTION_PATTERN_INIT(terminal_type, terminal_tag, 0)
        FLTL_CFG_UNBOUND_PRODUCTION_PATTERN_INIT(terminal_type, terminal_tag, 0)

        FLTL_CFG_PRODUCTION_PATTERN_INIT(symbol_type, symbol_tag, 0)
        FLTL_CFG_UNBOUND_PRODUCTION_PATTERN_INIT(symbol_type, symbol_tag, 0)

        FLTL_CFG_PRODUCTION_PATTERN_INIT(symbol_string_type, symbol_string_tag, 0)
        FLTL_CFG_UNBOUND_PRODUCTION_PATTERN_INIT(symbol_string_type, symbol_string_tag, 1)

        FLTL_CFG_PRODUCTION_PATTERN_INIT(AnySymbol<AlphaT>, any_symbol_tag, 0)
        FLTL_CFG_PRODUCTION_PATTERN_INIT(AnySymbolString<AlphaT>, any_symbol_string_tag, 1)
    };
}}}

#endif /* FLTL_PATTERN_HPP_ */
