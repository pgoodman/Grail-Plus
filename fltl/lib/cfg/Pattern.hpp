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
            symbol_string_type *as_symbol_string;
            Unbound<AlphaT, symbol_type> *as_unbound_symbol;
            Unbound<AlphaT, terminal_type> *as_unbound_terminal;
            Unbound<AlphaT, variable_type> *as_unbound_variable;
            Unbound<AlphaT, symbol_string_type> *as_unbound_symbol_string;
        };

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
        class Factor<AlphaT,Unbound<AlphaT, typename CFG<AlphaT>::symbol_string_type>,offset> {
        public:
            enum {
                WIDTH = 1,
                MIN_NUM_SYMBOLS = 0,
                OFFSET = offset,
                NEXT_OFFSET = OFFSET + WIDTH
            };
        };

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
                    symbols + len
                );
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

                *(slots->as_unbound_symbol->symbol) = *symbols;

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

                *(slots->as_unbound_variable->symbol) = *helper::unsafe_cast<typename CFG<AlphaT>::variable_type *>(symbols);

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

                *(slots->as_unbound_terminal->symbol) = *helper::unsafe_cast<typename CFG<AlphaT>::terminal_type *>(symbols);

                return true;
            }
        };

        /// trailing unbound symbol string
        template <typename AlphaT, typename StringT, const unsigned offset>
        class Match2<AlphaT,StringT,offset,Unbound<AlphaT,typename CFG<AlphaT>::symbol_string_type>, void> {
        public:
            inline static bool bind(Slot<AlphaT> *slots, const Symbol<AlphaT> *symbols, const unsigned len) throw() {
                SymbolString<AlphaT> str(symbols, len);
                *(slots->as_unbound_symbol_string->string) = str;
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

                if(str_len > len) {
                    return false;
                }

                if(0 != str_len) {
                    const bool prefix_is_str(SymbolString<AlphaT>::symbol_memcmp(
                        symbols,
                        &(slots->as_symbol_string->at(0)),
                        symbols + str_len
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

        /// unbound symbol + T
        template <typename AlphaT, typename StringT, const unsigned offset, typename T>
        class Match2<AlphaT,StringT,offset,Unbound<AlphaT,typename CFG<AlphaT>::symbol_type>, T> {
        public:
            inline static bool bind(Slot<AlphaT> *slots, const Symbol<AlphaT> *symbols, const unsigned len) throw() {
                if(len >= 1) {
                    *(slots->as_unbound_symbol->symbol) = *symbols;

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
                    *(slots->as_unbound_variable->symbol) = *helper::unsafe_cast<typename CFG<AlphaT>::variable_type *>(symbols);

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
                    *(slots->as_unbound_terminal->symbol) = *helper::unsafe_cast<typename CFG<AlphaT>::terminal_type *>(symbols);

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
                //SymbolString<AlphaT> str(symbols, len);
                //*(slots->as_unbound_symbol_string->string) = str;
                //return true;

                for(unsigned i(0); i < len; ++i) {
                    SymbolString<AlphaT> str(symbols, i);
                    *(slots->as_unbound_symbol_string->string) = str;

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

        template <typename AlphaT, typename VarT, typename PatternT, typename StringT>
        class DestructuringBind;

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
                if(pattern.var != prod.variable()) {
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
                    >::bind(&(pattern.slots[0]), 0 == str_len ? 0 : &(str.at(0)), str_len);
                }

                return false;
            }
        };

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
                *(pattern.var.symbol) = prod.variable();

                const SymbolString<AlphaT> &str(prod.symbols());
                const unsigned str_len(str.length());

                if((1U + str_len) >= (1U + StringT::MIN_NUM_SYMBOLS)) {
                    return Match2<
                        AlphaT,
                        StringT,
                        0,
                        typename GetFactor<AlphaT,StringT,0>::type,
                        typename GetFactor<AlphaT,StringT,1>::type
                    >::bind(&(pattern.slots[0]), &(str.at(0)), str_len);
                }

                return false;
            }
        };

        template <typename AlphaT, typename PatternT, typename StringT>
        class PatternBuilder<AlphaT,PatternT,StringT,0U> {
        public:

            typedef typename CFG<AlphaT>::variable_type variable_type;
            typedef typename CFG<AlphaT>::terminal_type terminal_type;
            typedef typename CFG<AlphaT>::symbol_type symbol_type;
            typedef typename CFG<AlphaT>::symbol_string_type symbol_string_type;
            typedef typename CFG<AlphaT>::production_type production_type;

            PatternT *pattern;

            FLTL_FORCE_INLINE PatternBuilder(PatternT *_pattern)
                : pattern(_pattern)
            { }

            FLTL_CFG_PRODUCTION_PATTERN_EXTEND(variable_type, 0)
            FLTL_CFG_UNBOUND_PRODUCTION_PATTERN_EXTEND(variable_type, 0)

            FLTL_CFG_PRODUCTION_PATTERN_EXTEND(terminal_type, 0)
            FLTL_CFG_UNBOUND_PRODUCTION_PATTERN_EXTEND(terminal_type, 0)

            FLTL_CFG_PRODUCTION_PATTERN_EXTEND(symbol_type, 0)
            FLTL_CFG_UNBOUND_PRODUCTION_PATTERN_EXTEND(symbol_type, 0)

            FLTL_CFG_PRODUCTION_PATTERN_EXTEND(symbol_string_type, 0)
            FLTL_CFG_UNBOUND_PRODUCTION_PATTERN_EXTEND(symbol_string_type, 1)

            inline bool destructuring_bind(const production_type &prod) {
                return DestructuringBind<
                    AlphaT,
                    typename PatternT::lhs_type,
                    PatternT,
                    StringT
                >::bind(*pattern, prod);
            }
        };

        template <typename AlphaT, typename PatternT, typename StringT>
        class PatternBuilder<AlphaT,PatternT,StringT,1U> {
        public:

            typedef typename CFG<AlphaT>::variable_type variable_type;
            typedef typename CFG<AlphaT>::terminal_type terminal_type;
            typedef typename CFG<AlphaT>::symbol_type symbol_type;
            typedef typename CFG<AlphaT>::symbol_string_type symbol_string_type;
            typedef typename CFG<AlphaT>::production_type production_type;

            PatternT *pattern;

            FLTL_FORCE_INLINE PatternBuilder(PatternT *_pattern)
                : pattern(_pattern)
            { }

            FLTL_CFG_PRODUCTION_PATTERN_EXTEND(variable_type, 0)
            FLTL_CFG_UNBOUND_PRODUCTION_PATTERN_EXTEND(variable_type, 0)

            FLTL_CFG_PRODUCTION_PATTERN_EXTEND(terminal_type, 0)
            FLTL_CFG_UNBOUND_PRODUCTION_PATTERN_EXTEND(terminal_type, 0)

            FLTL_CFG_PRODUCTION_PATTERN_EXTEND(symbol_type, 0)

            FLTL_CFG_PRODUCTION_PATTERN_EXTEND(symbol_string_type, 0)

            inline bool destructuring_bind(const production_type &prod) {
                return DestructuringBind<
                    AlphaT,
                    typename PatternT::lhs_type,
                    PatternT,
                    StringT
                >::bind(*pattern, prod);
            }
        };
    }

    template <typename AlphaT, typename V>
    class Pattern {
    private:

        friend class CFG<AlphaT>;

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

        V var;

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
            next_slot->as_unbound_symbol = const_cast<Unbound<AlphaT, symbol_type> *>(expr);
            ++(next_slot);
        }

        void extend(const Unbound<AlphaT, terminal_type> *expr) throw() {
            next_slot->as_unbound_terminal = const_cast<Unbound<AlphaT, terminal_type> *>(expr);
            ++(next_slot);
        }

        void extend(const Unbound<AlphaT, variable_type> *expr) throw() {
            next_slot->as_unbound_variable = const_cast<Unbound<AlphaT, variable_type> *>(expr);
            ++(next_slot);
        }

        void extend(const Unbound<AlphaT, symbol_string_type> *expr) throw() {
            next_slot->as_unbound_symbol_string = const_cast<Unbound<AlphaT, symbol_string_type> *>(expr);
            ++(next_slot);
        }

    public:

        Pattern(V &_var) throw()
            : var(_var)
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
    };
}}}

#endif /* FLTL_PATTERN_HPP_ */
