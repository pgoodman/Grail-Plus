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
#define _FLTL_CFG_UNBOUND(type) cfg::Unbound<AlphaT,type>
#endif

#define FLTL_CFG_PRODUCTION_PATTERN_INIT(type, state) \
    FLTL_FORCE_INLINE detail::PatternBuilder<AlphaT, self_type, detail::Factor<AlphaT,type,0>, state> \
    operator->*(const type &expr) throw() { \
        pattern->extend(&expr); \
        return detail::PatternBuilder<AlphaT, self_type, detail::Factor<AlphaT,type,0>, state>( \
            pattern \
        ); \
    }

#define FLTL_CFG_UNBOUND_PRODUCTION_PATTERN_INIT(type,state) \
    FLTL_CFG_PRODUCTION_PATTERN_INIT(_FLTL_CFG_UNBOUND(type),state)

#define FLTL_CFG_PRODUCTION_PATTERN_EXTEND(type, state) \
    FLTL_FORCE_INLINE detail::PatternBuilder<AlphaT, PatternT, detail::Catenation<AlphaT,StringT,detail::Factor<AlphaT,type, StringT::NEXT_OFFSET> >, state> \
    operator+(const type &expr) throw() { \
        pattern->extend(&expr); \
        return detail::PatternBuilder<AlphaT, PatternT, detail::Catenation<AlphaT,StringT,detail::Factor<AlphaT,type, StringT::NEXT_OFFSET> >, state>( \
            pattern \
        ); \
    }

#define FLTL_CFG_UNBOUND_PRODUCTION_PATTERN_EXTEND(type,state) \
    FLTL_CFG_PRODUCTION_PATTERN_EXTEND(_FLTL_CFG_UNBOUND(type),state)


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

        template <typename AlphaT,typename T, const unsigned offset>
        class Factor {
        public:
            enum {
                WIDTH = 1,
                MIN_NUM_SYMBOLS = 1,
                OFFSET = offset,
                NEXT_OFFSET = OFFSET + WIDTH
            };
        };

        template <typename AlphaT, const unsigned offset>
        class Factor<AlphaT,typename CFG<AlphaT>::symbol_string_type,offset> {
        public:
            enum {
                WIDTH = 1,
                MIN_NUM_SYMBOLS = 0,
                OFFSET = offset,
                NEXT_OFFSET = OFFSET + WIDTH
            };
        };

        template <typename AlphaT, const unsigned offset>
        class Factor<AlphaT,AnySymbolString<AlphaT>,offset> {
        public:
            enum {
                WIDTH = 1,
                MIN_NUM_SYMBOLS = 0,
                OFFSET = offset,
                NEXT_OFFSET = OFFSET + WIDTH
            };
        };

        template <typename AlphaT, const unsigned offset>
        class Factor<AlphaT,Unbound<AlphaT, typename CFG<AlphaT>::symbol_string_type>,offset> {
        public:
            enum {
                WIDTH = 1,
                MIN_NUM_SYMBOLS = 0,
                OFFSET = offset,
                NEXT_OFFSET = OFFSET + WIDTH
            };
        };

        /// concatenate facotrs/concatenations together

        template <typename AlphaT,typename T0, typename T1>
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

        template <typename AlphaT,typename T, const unsigned offset>
        class GetFactor {
        public:
            typedef void type;
        };

        template <typename AlphaT,typename T0, typename T1, const unsigned offset>
        class GetFactor<AlphaT,Catenation<AlphaT,T0, T1>, offset> {
        public:
            typedef typename GetFactor<AlphaT,
                typename mpl::IfTrue<(offset >= T1::OFFSET),T1,T0>::type,
                offset
            >::type type;
        };

        template <typename AlphaT,typename T, const unsigned offset>
        class GetFactor<AlphaT,Factor<AlphaT,T, offset>, offset> {
        public:
            typedef T type;
        };

        /// extract the minimum number of symbols that the pattern must
        /// match that come after a specific offset
        template <typename AlphaT, typename T, const unsigned offset>
        class GetMinNumSymbolsAfter {
        public:
            enum {
                RESULT = 0
            };
        };

        template <typename AlphaT,typename T0, typename T1, const unsigned offset>
        class GetMinNumSymbolsAfter<AlphaT,Catenation<AlphaT,T0, T1>, offset> {
        public:
            enum {
                RESULT = (
                    GetMinNumSymbolsAfter<AlphaT,T0,offset>::RESULT +
                    GetMinNumSymbolsAfter<AlphaT,T1,offset>::RESULT
                )
            };
            typedef typename GetFactor<AlphaT,
                typename mpl::IfTrue<(offset > T1::OFFSET),T1,T0>::type,
                offset
            >::type type;
        };

        template <typename AlphaT,typename T, const unsigned o, const unsigned offset>
        class GetMinNumSymbolsAfter<AlphaT,Factor<AlphaT,T,o>, offset> {
        public:
            enum {
                RESULT = o > offset ? Factor<AlphaT,T,o>::MIN_NUM_SYMBOLS : 0
            };
        };

        /// match symbols from the pattern

        template <typename AlphaT, typename StringT, const unsigned offset, typename T0, typename T1>
        class Match2;

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
        class Match2<AlphaT,StringT,offset,typename CFG<AlphaT>::symbol_type, void> {
        public:
            inline static bool bind(Slot<AlphaT> *slots, const Symbol<AlphaT> *symbols, const unsigned len) throw() {
                return (1 == len) && (*symbols == *(slots->as_symbol));
            }
        };

        /// trailing terminal
        template <typename AlphaT, typename StringT, const unsigned offset>
        class Match2<AlphaT,StringT,offset,typename CFG<AlphaT>::terminal_type, void> {
        public:
            inline static bool bind(Slot<AlphaT> *slots, const Symbol<AlphaT> *symbols, const unsigned len) throw() {
                return (1 == len) && (*symbols == *(slots->as_symbol));
            }
        };

        /// trailing variable
        template <typename AlphaT, typename StringT, const unsigned offset>
        class Match2<AlphaT,StringT,offset,typename CFG<AlphaT>::variable_type, void> {
        public:
            inline static bool bind(Slot<AlphaT> *slots, const Symbol<AlphaT> *symbols, const unsigned len) throw() {
                return (1 == len) && (*symbols == *(slots->as_symbol));
            }
        };

        /// trailing symbol string
        template <typename AlphaT, typename StringT, const unsigned offset>
        class Match2<AlphaT,StringT,offset,typename CFG<AlphaT>::symbol_string_type, void> {
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
        class Match2<AlphaT,StringT,offset,AnySymbol<AlphaT>, void> {
        public:
            inline static bool bind(Slot<AlphaT> *, const Symbol<AlphaT> *, const unsigned len) throw() {
                return 1 == len;
            }
        };

        /// trailing unbound symbol
        template <typename AlphaT, typename StringT, const unsigned offset>
        class Match2<AlphaT,StringT,offset,Unbound<AlphaT,typename CFG<AlphaT>::symbol_type>, void> {
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
        class Match2<AlphaT,StringT,offset,Unbound<AlphaT,typename CFG<AlphaT>::variable_type>, void> {
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
        class Match2<AlphaT,StringT,offset,Unbound<AlphaT,typename CFG<AlphaT>::terminal_type>, void> {
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
        class Match2<AlphaT,StringT,offset,Unbound<AlphaT,typename CFG<AlphaT>::symbol_string_type>, void> {
        public:
            inline static bool bind(Slot<AlphaT> *slots, const Symbol<AlphaT> *symbols, const unsigned len) throw() {
                SymbolString<AlphaT> str(symbols, len);
                *(slots->as_symbol_string) = str;
                return true;
            }
        };

        /// trailing any symbol string
        template <typename AlphaT, typename StringT, const unsigned offset>
        class Match2<AlphaT,StringT,offset,AnySymbolString<AlphaT>, void> {
        public:
            inline static bool bind(Slot<AlphaT> *, const Symbol<AlphaT> *, const unsigned) throw() {
                return true;
            }
        };

        /// symbol + T
        template <typename AlphaT, typename StringT, const unsigned offset, typename T>
        class Match2<AlphaT,StringT,offset,typename CFG<AlphaT>::symbol_type, T> {
        public:
            inline static bool bind(Slot<AlphaT> *slots, const Symbol<AlphaT> *symbols, const unsigned len) throw() {
                if((1 <= len) && (*symbols == *(slots->as_symbol))) {
                    return Match2<
                        AlphaT,
                        StringT,
                        offset + 1,
                        T,
                        typename GetFactor<AlphaT,StringT,offset + 2>::type
                    >::bind(slots + 1, symbols + 1, len - 1);
                } else {
                    return false;
                }
            }
        };

        /// terminal + T
        template <typename AlphaT, typename StringT, const unsigned offset, typename T>
        class Match2<AlphaT,StringT,offset,typename CFG<AlphaT>::terminal_type, T> {
        public:
            inline static bool bind(Slot<AlphaT> *slots, const Symbol<AlphaT> *symbols, const unsigned len) throw() {
                if((1 <= len) && (*symbols == *(slots->as_symbol))) {
                    return Match2<
                        AlphaT,
                        StringT,
                        offset + 1,
                        T,
                        typename GetFactor<AlphaT,StringT,offset + 2>::type
                    >::bind(slots + 1, symbols + 1, len - 1);
                } else {
                    return false;
                }
            }
        };

        /// variable + T
        template <typename AlphaT, typename StringT, const unsigned offset, typename T>
        class Match2<AlphaT,StringT,offset,typename CFG<AlphaT>::variable_type, T> {
        public:
            inline static bool bind(Slot<AlphaT> *slots, const Symbol<AlphaT> *symbols, const unsigned len) throw() {
                if((1 <= len) && (*symbols == *(slots->as_symbol))) {
                    return Match2<
                        AlphaT,
                        StringT,
                        offset + 1,
                        T,
                        typename GetFactor<AlphaT,StringT,offset + 2>::type
                    >::bind(slots + 1, symbols + 1, len - 1);
                } else {
                    return false;
                }
            }
        };

        /// symbol string + T
        template <typename AlphaT, typename StringT, const unsigned offset, typename T>
        class Match2<AlphaT,StringT,offset,typename CFG<AlphaT>::symbol_string_type, T> {
        public:
            inline static bool bind(Slot<AlphaT> *slots, const Symbol<AlphaT> *symbols, const unsigned len) throw() {
                const unsigned str_len(slots->as_symbol_string->length());
                enum {
                    MIN_REQUIRED_LEN = 1 + GetMinNumSymbolsAfter<AlphaT,StringT,offset>::RESULT
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
                    typename GetFactor<AlphaT,StringT,offset + 2>::type
                >::bind(slots + 1, symbols + str_len, len - str_len);
            }
        };

        /// ignore + T
        template <typename AlphaT, typename StringT, const unsigned offset, typename T>
        class Match2<AlphaT,StringT,offset,AnySymbol<AlphaT>, T> {
        public:
            inline static bool bind(Slot<AlphaT> *slots, const Symbol<AlphaT> *symbols, const unsigned len) throw() {
                return (len >= 1) && Match2<
                    AlphaT,
                    StringT,
                    offset + 1,
                    T,
                    typename GetFactor<AlphaT,StringT,offset + 2>::type
                >::bind(slots + 1, symbols + 1, len - 1);
            }
        };

        /// unbound symbol + T
        template <typename AlphaT, typename StringT, const unsigned offset, typename T>
        class Match2<AlphaT,StringT,offset,Unbound<AlphaT,typename CFG<AlphaT>::symbol_type>, T> {
        public:
            inline static bool bind(Slot<AlphaT> *slots, const Symbol<AlphaT> *symbols, const unsigned len) throw() {
                if(len >= 1) {
                    *(slots->as_symbol) = *symbols;

                    return Match2<
                        AlphaT,
                        StringT,
                        offset + 1,
                        T,
                        typename GetFactor<AlphaT,StringT,offset + 2>::type
                    >::bind(slots + 1, symbols + 1, len - 1);
                }
                return false;
            }
        };

        /// unbound variable + T
        template <typename AlphaT, typename StringT, const unsigned offset, typename T>
        class Match2<AlphaT,StringT,offset,Unbound<AlphaT,typename CFG<AlphaT>::variable_type>, T> {
        public:
            inline static bool bind(Slot<AlphaT> *slots, const Symbol<AlphaT> *symbols, const unsigned len) throw() {
                if(len >= 1 && symbols->is_variable()) {
                    *(slots->as_symbol) = *helper::unsafe_cast<typename CFG<AlphaT>::variable_type *>(symbols);

                    return Match2<
                        AlphaT,
                        StringT,
                        offset + 1,
                        T,
                        typename GetFactor<AlphaT,StringT,offset + 2>::type
                    >::bind(slots + 1, symbols + 1, len - 1);
                }
                return false;
            }
        };

        /// unbound terminal + T
        template <typename AlphaT, typename StringT, const unsigned offset, typename T>
        class Match2<AlphaT,StringT,offset,Unbound<AlphaT,typename CFG<AlphaT>::terminal_type>, T> {
        public:
            inline static bool bind(Slot<AlphaT> *slots, const Symbol<AlphaT> *symbols, const unsigned len) throw() {
                if(len >= 1 && symbols->is_terminal()) {
                    *(slots->as_terminal) = *helper::unsafe_cast<typename CFG<AlphaT>::terminal_type *>(symbols);

                    return Match2<
                        AlphaT,
                        StringT,
                        offset + 1,
                        T,
                        typename GetFactor<AlphaT,StringT,offset + 2>::type
                    >::bind(slots + 1, symbols + 1, len - 1);
                }
                return false;
            }
        };

        /// unbound symbol string + T
        template <typename AlphaT, typename StringT, const unsigned offset, typename T>
        class Match2<AlphaT,StringT,offset,Unbound<AlphaT,typename CFG<AlphaT>::symbol_string_type>, T> {
        public:
            inline static bool bind(Slot<AlphaT> *slots, const Symbol<AlphaT> *symbols, const unsigned len) throw() {

                const unsigned max(len - GetMinNumSymbolsAfter<AlphaT,StringT,offset>::RESULT);
                for(unsigned i(0); i <= max; ++i) {

                    SymbolString<AlphaT> str(symbols, i);
                    *(slots->as_symbol_string) = str;

                    const bool matched_next_at_i(Match2<
                        AlphaT,
                        StringT,
                        offset + 1,
                        T,
                        typename GetFactor<AlphaT,StringT,offset + 2>::type
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
        class Match2<AlphaT,StringT,offset,AnySymbolString<AlphaT>, T> {
        public:
            inline static bool bind(Slot<AlphaT> *slots, const Symbol<AlphaT> *symbols, const unsigned len) throw() {

                const unsigned max(len - GetMinNumSymbolsAfter<AlphaT,StringT,offset>::RESULT);
                for(unsigned i(0); i <= max; ++i) {
                    const bool matched_next_at_i(Match2<
                        AlphaT,
                        StringT,
                        offset + 1,
                        T,
                        typename GetFactor<AlphaT,StringT,offset + 2>::type
                    >::bind(slots + 1, symbols + i, len - i));
                    if(matched_next_at_i) {
                        return true;
                    }
                }

                return false;
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
                    typename GetFactor<AlphaT,StringT,offset + 1>::type,
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
        class ResetPattern<AlphaT,StringT,Unbound<AlphaT,SymbolString<AlphaT> >,offset> {
        public:
            inline static void reset(Slot<AlphaT> *slots) throw() {

                // reset the unbound symbol string
                SymbolString<AlphaT> epsilon;
                *(slots[offset].as_symbol_string) = epsilon;

                ResetPattern<
                    AlphaT,
                    StringT,
                    typename GetFactor<AlphaT,StringT,offset + 1>::type,
                    offset + 1
                >::reset(slots);
            }
        };

        template <typename AlphaT, typename VarT, typename PatternT, typename StringT>
        class DestructuringBind;

        /// check the variable of the pattern
        template <typename AlphaT, typename StringT>
        class DestructuringBind<
            AlphaT,
            typename CFG<AlphaT>::variable_type,
            Pattern<AlphaT, typename CFG<AlphaT>::variable_type>,
            StringT
        > {
        public:

            typedef typename CFG<AlphaT>::production_type production_type;

            inline static bool bind(
                Pattern<AlphaT, typename CFG<AlphaT>::variable_type> &pattern,
                const production_type &prod
            ) {
                if(!prod.is_valid()) {
                    return false;
                }

                Slot<AlphaT> *slots(&(pattern.slots[0]));

                ResetPattern<
                    AlphaT,
                    StringT,
                    typename GetFactor<AlphaT,StringT,0>::type,
                    0
                >::reset(slots);

                if(*(pattern.var) != prod.variable()) {
                    return false;
                }

                const SymbolString<AlphaT> &str(prod.symbols());
                const unsigned str_len(str.length());

                if((1U + str_len) >= (1U + StringT::MIN_NUM_SYMBOLS)) {
                    return Match2<
                        AlphaT,
                        StringT,
                        0,
                        typename GetFactor<AlphaT,StringT,0>::type,
                        typename GetFactor<AlphaT,StringT,1>::type
                    >::bind(slots, 0 == str_len ? 0 : &(str.at(0)), str_len);
                }

                return false;
            }
        };

        /// bind the variable
        template <typename AlphaT, typename StringT>
        class DestructuringBind<
            AlphaT,
            Unbound<AlphaT, typename CFG<AlphaT>::variable_type>,
            Pattern<AlphaT, Unbound<AlphaT, typename CFG<AlphaT>::variable_type> >,
            StringT
        > {
        public:

            typedef typename CFG<AlphaT>::production_type production_type;

            inline static bool bind(
                Pattern<AlphaT, Unbound<AlphaT, typename CFG<AlphaT>::variable_type> > &pattern,
                const production_type &prod
            ) {
                if(!prod.is_valid()) {
                    return false;
                }

                Slot<AlphaT> *slots(&(pattern.slots[0]));

                ResetPattern<
                    AlphaT,
                    StringT,
                    typename GetFactor<AlphaT,StringT,0>::type,
                    0
                >::reset(slots);

                *(pattern.var) = prod.variable();

                const SymbolString<AlphaT> &str(prod.symbols());
                const unsigned str_len(str.length());

                if((1U + str_len) >= (1U + StringT::MIN_NUM_SYMBOLS)) {
                    return Match2<
                        AlphaT,
                        StringT,
                        0,
                        typename GetFactor<AlphaT,StringT,0>::type,
                        typename GetFactor<AlphaT,StringT,1>::type
                    >::bind(slots, 0 == str_len ? 0 : &(str.at(0)), str_len);
                }

                return false;
            }
        };

        /// ignore the variable
        template <typename AlphaT, typename StringT>
        class DestructuringBind<
            AlphaT,
            AnySymbol<AlphaT>,
            Pattern<AlphaT, AnySymbol<AlphaT> >,
            StringT
        > {
        public:

            typedef typename CFG<AlphaT>::production_type production_type;

            inline static bool bind(
                Pattern<AlphaT, AnySymbol<AlphaT> > &pattern,
                const production_type &prod
            ) {
                Slot<AlphaT> *slots(&(pattern.slots[0]));

                ResetPattern<
                    AlphaT,
                    StringT,
                    typename GetFactor<AlphaT,StringT,0>::type,
                    0
                >::reset(slots);

                const SymbolString<AlphaT> &str(prod.symbols());
                const unsigned str_len(str.length());

                if((1U + str_len) >= (1U + StringT::MIN_NUM_SYMBOLS)) {
                    return Match2<
                        AlphaT,
                        StringT,
                        0,
                        typename GetFactor<AlphaT,StringT,0>::type,
                        typename GetFactor<AlphaT,StringT,1>::type
                    >::bind(slots, 0 == str_len ? 0 : &(str.at(0)), str_len);
                }

                return false;
            }
        };

        template <typename AlphaT, typename PatternT, typename StringT>
        class PatternBuilder<AlphaT,PatternT,StringT,0U> {
        public:

            enum {
                IS_BOUND_TO_VAR = PatternT::IS_BOUND
            };

            friend class CFG<AlphaT>;

            template <typename, typename>
            friend class PatternGenerator;

            typedef typename CFG<AlphaT>::variable_type variable_type;
            typedef typename CFG<AlphaT>::terminal_type terminal_type;
            typedef typename CFG<AlphaT>::symbol_type symbol_type;
            typedef typename CFG<AlphaT>::symbol_string_type symbol_string_type;
            typedef typename CFG<AlphaT>::production_type production_type;
            typedef PatternT pattern_type;

            PatternT *pattern;

            FLTL_FORCE_INLINE PatternBuilder(PatternT *_pattern)
                : pattern(_pattern)
            { }

            FLTL_CFG_PRODUCTION_PATTERN_EXTEND(variable_type, 0)
            FLTL_CFG_UNBOUND_PRODUCTION_PATTERN_EXTEND(variable_type, 0)

            FLTL_CFG_PRODUCTION_PATTERN_EXTEND(AnySymbol<AlphaT>, 0)
            FLTL_CFG_PRODUCTION_PATTERN_EXTEND(AnySymbolString<AlphaT>, 1)

            FLTL_CFG_PRODUCTION_PATTERN_EXTEND(terminal_type, 0)
            FLTL_CFG_UNBOUND_PRODUCTION_PATTERN_EXTEND(terminal_type, 0)

            FLTL_CFG_PRODUCTION_PATTERN_EXTEND(symbol_type, 0)
            FLTL_CFG_UNBOUND_PRODUCTION_PATTERN_EXTEND(symbol_type, 0)

            FLTL_CFG_PRODUCTION_PATTERN_EXTEND(symbol_string_type, 0)
            FLTL_CFG_UNBOUND_PRODUCTION_PATTERN_EXTEND(symbol_string_type, 1)

            inline bool match(const production_type &prod) throw() {
                return DestructuringBind<
                    AlphaT,
                    typename PatternT::lhs_type,
                    PatternT,
                    StringT
                >::bind(*pattern, prod);
            }

        private:

            inline static bool static_match(
                void *_pattern,
                const production_type &prod
            ) throw() {

                return DestructuringBind<
                    AlphaT,
                    typename PatternT::lhs_type,
                    PatternT,
                    StringT
                >::bind(*helper::unsafe_cast<PatternT *>(_pattern), prod);
            }
        };

        template <typename AlphaT, typename PatternT, typename StringT>
        class PatternBuilder<AlphaT,PatternT,StringT,1U> {
        public:

            enum {
                IS_BOUND_TO_VAR = PatternT::IS_BOUND
            };

            friend class CFG<AlphaT>;

            template <typename, typename>
            friend class PatternGenerator;

            typedef typename CFG<AlphaT>::variable_type variable_type;
            typedef typename CFG<AlphaT>::terminal_type terminal_type;
            typedef typename CFG<AlphaT>::symbol_type symbol_type;
            typedef typename CFG<AlphaT>::symbol_string_type symbol_string_type;
            typedef typename CFG<AlphaT>::production_type production_type;
            typedef PatternT pattern_type;

            PatternT *pattern;

            FLTL_FORCE_INLINE PatternBuilder(PatternT *_pattern) throw()
                : pattern(_pattern)
            { }

            FLTL_CFG_PRODUCTION_PATTERN_EXTEND(variable_type, 0)
            FLTL_CFG_UNBOUND_PRODUCTION_PATTERN_EXTEND(variable_type, 0)

            FLTL_CFG_PRODUCTION_PATTERN_EXTEND(terminal_type, 0)
            FLTL_CFG_UNBOUND_PRODUCTION_PATTERN_EXTEND(terminal_type, 0)

            FLTL_CFG_PRODUCTION_PATTERN_EXTEND(symbol_type, 0)
            FLTL_CFG_UNBOUND_PRODUCTION_PATTERN_EXTEND(symbol_type, 0)

            FLTL_CFG_PRODUCTION_PATTERN_EXTEND(AnySymbol<AlphaT>, 0)

            FLTL_CFG_PRODUCTION_PATTERN_EXTEND(symbol_string_type, 0)

            inline bool match(const production_type &prod) throw() {
                return DestructuringBind<
                    AlphaT,
                    typename PatternT::lhs_type,
                    PatternT,
                    StringT
                >::bind(*pattern, prod);
            }

        private:

            inline static bool static_match(
                void *_pattern,
                const production_type &prod
            ) throw() {

                return DestructuringBind<
                    AlphaT,
                    typename PatternT::lhs_type,
                    PatternT,
                    StringT
                >::bind(*helper::unsafe_cast<PatternT *>(_pattern), prod);
            }
        };
    }

    template <typename AlphaT, typename V>
    class Pattern {
    private:

        friend class CFG<AlphaT>;

        friend class detail::SimpleGenerator<AlphaT>;

        template <typename, typename>
        friend class detail::PatternGenerator;

        template <typename,typename,typename,const unsigned>
        friend class detail::PatternBuilder;

        template <typename, typename, typename, typename>
        friend class detail::DestructuringBind;

        typedef typename CFG<AlphaT>::variable_type variable_type;
        typedef typename CFG<AlphaT>::terminal_type terminal_type;
        typedef typename CFG<AlphaT>::symbol_type symbol_type;
        typedef typename CFG<AlphaT>::symbol_string_type symbol_string_type;

        typedef Pattern<AlphaT,V> self_type;
        typedef V lhs_type;

        enum {
            NUM_SLOTS = 8U,
            IS_BOUND = mpl::IfTypesEqual<V, variable_type>::RESULT
        };

        variable_type *var;

        detail::Slot<AlphaT> slots[NUM_SLOTS];

        detail::Slot<AlphaT> *next_slot;

        /// reference back to this pattern
        Pattern<AlphaT,V> *pattern;

        void extend(const symbol_type *expr) throw() {
            next_slot->as_symbol = const_cast<symbol_type *>(expr);
            ++(next_slot);
        }

        void extend(const symbol_string_type *expr) throw() {
            next_slot->as_symbol_string = const_cast<symbol_string_type *>(expr);
            ++(next_slot);
        }

        void extend(const Unbound<AlphaT, symbol_type> *expr) throw() {
            next_slot->as_symbol = const_cast<Unbound<AlphaT, symbol_type> *>(expr)->symbol;
            ++(next_slot);
        }

        void extend(const Unbound<AlphaT, terminal_type> *expr) throw() {
            next_slot->as_terminal = const_cast<Unbound<AlphaT, terminal_type> *>(expr)->symbol;
            ++(next_slot);
        }

        void extend(const Unbound<AlphaT, variable_type> *expr) throw() {
            next_slot->as_variable = const_cast<Unbound<AlphaT, variable_type> *>(expr)->symbol;
            ++(next_slot);
        }

        void extend(const Unbound<AlphaT, symbol_string_type> *expr) throw() {
            next_slot->as_symbol_string = const_cast<Unbound<AlphaT, symbol_string_type> *>(expr)->string;
            ++(next_slot);
        }

        void extend(const AnySymbol<AlphaT> *) throw() {
            ++(next_slot);
        }

        void extend(const AnySymbolString<AlphaT> *) throw() {
            ++(next_slot);
        }

        const void *get_var(void) const throw() {
            return reinterpret_cast<const void *>(&var);
        }

    public:

        Pattern(variable_type &_var) throw()
            : var(&_var)
            , next_slot(&(slots[0]))
            , pattern(this)
        {
            memset(slots, 0, sizeof(detail::Slot<AlphaT>) * NUM_SLOTS);
        }

        Pattern(Unbound<AlphaT, variable_type> &_var) throw()
            : var(_var.symbol)
            , next_slot(&(slots[0]))
            , pattern(this)
        {
            memset(slots, 0, sizeof(detail::Slot<AlphaT>) * NUM_SLOTS);
        }

        Pattern(AnySymbol<AlphaT>) throw()
            : var(0)
            , next_slot(&(slots[0]))
            , pattern(this)
        {
            memset(slots, 0, sizeof(detail::Slot<AlphaT>) * NUM_SLOTS);
        }

        FLTL_CFG_PRODUCTION_PATTERN_INIT(variable_type, 0)
        FLTL_CFG_UNBOUND_PRODUCTION_PATTERN_INIT(variable_type, 0)

        FLTL_CFG_PRODUCTION_PATTERN_INIT(terminal_type, 0)
        FLTL_CFG_UNBOUND_PRODUCTION_PATTERN_INIT(terminal_type, 0)

        FLTL_CFG_PRODUCTION_PATTERN_INIT(symbol_type, 0)
        FLTL_CFG_UNBOUND_PRODUCTION_PATTERN_INIT(symbol_type, 0)

        FLTL_CFG_PRODUCTION_PATTERN_INIT(symbol_string_type, 0)
        FLTL_CFG_UNBOUND_PRODUCTION_PATTERN_INIT(symbol_string_type, 1)

        FLTL_CFG_PRODUCTION_PATTERN_INIT(AnySymbol<AlphaT>, 0)
        FLTL_CFG_PRODUCTION_PATTERN_INIT(AnySymbolString<AlphaT>, 1)
    };
}}}

#endif /* FLTL_PATTERN_HPP_ */
