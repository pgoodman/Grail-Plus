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

#define FLTL_CFG_PRODUCTION_PATTERN_EXTEND(type, state) \
    FLTL_FORCE_INLINE Pattern<AlphaT,detail::MatchPair<T, detail::MatchSingle<type> >, state> \
    operator+(type expr) throw() { \
        return Pattern<AlphaT,detail::MatchPair<T, detail::MatchSingle<type> >, state>( \
            detail::MatchPair<T, detail::MatchSingle<type> >( \
                pattern, \
                detail::MatchSingle<type>(expr) \
            ) \
        ); \
    }

#ifndef _FLTL_CFG_UNBOUND
#define _FLTL_CFG_UNBOUND(type) cfg::Unbound<AlphaT,type>
#endif

#define FLTL_CFG_UNBOUND_PRODUCTION_PATTERN_EXTEND(type,state) \
    FLTL_CFG_PRODUCTION_PATTERN_EXTEND(_FLTL_CFG_UNBOUND(type),state)

namespace fltl { namespace lib { namespace cfg {

    namespace detail {

        template <typename T>
        class MatchSingle {
        public:
            T expr;

            inline MatchSingle(T &_expr) throw()
                : expr(_expr)
            { }

            inline MatchSingle(const MatchSingle<T> &that) throw()
                : expr(that.expr)
            { }
        };

        template <typename T0, typename T1>
        class MatchPair {
        public:
            T0 expr0;
            T1 expr1;

            inline MatchPair(T0 &_expr0, T1 _expr1) throw()
                : expr0(_expr0)
                , expr1(_expr1)
            { }

            inline MatchPair(const MatchPair<T0,T1> &that)
                : expr0(that.expr0)
                , expr1(that.expr1)
            { }
        };
    }

    template <typename AlphaT, typename T>
    class Pattern<AlphaT, T, 0U> {
    public:

        typedef typename CFG<AlphaT>::variable_type variable_type;
        typedef typename CFG<AlphaT>::terminal_type terminal_type;
        typedef typename CFG<AlphaT>::symbol_type symbol_type;
        typedef typename CFG<AlphaT>::symbol_string_type symbol_string_type;

        T pattern;

        FLTL_FORCE_INLINE Pattern(T expr) throw()
            : pattern(expr)
        { }

        FLTL_CFG_PRODUCTION_PATTERN_EXTEND(variable_type, 0)
        FLTL_CFG_UNBOUND_PRODUCTION_PATTERN_EXTEND(variable_type, 0)

        FLTL_CFG_PRODUCTION_PATTERN_EXTEND(terminal_type, 0)
        FLTL_CFG_UNBOUND_PRODUCTION_PATTERN_EXTEND(terminal_type, 0)

        FLTL_CFG_PRODUCTION_PATTERN_EXTEND(symbol_type, 0)
        FLTL_CFG_UNBOUND_PRODUCTION_PATTERN_EXTEND(symbol_type, 0)

        FLTL_CFG_PRODUCTION_PATTERN_EXTEND(symbol_string_type, 0)
        FLTL_CFG_UNBOUND_PRODUCTION_PATTERN_EXTEND(symbol_string_type, 1)
    };

    template <typename AlphaT, typename T>
    class Pattern<AlphaT, T, 1U> {
    public:

        typedef typename CFG<AlphaT>::variable_type variable_type;
        typedef typename CFG<AlphaT>::terminal_type terminal_type;
        typedef typename CFG<AlphaT>::symbol_type symbol_type;
        typedef typename CFG<AlphaT>::symbol_string_type symbol_string_type;

        T pattern;

        FLTL_FORCE_INLINE Pattern(T expr) throw()
            : pattern(expr)
        { }

        FLTL_CFG_PRODUCTION_PATTERN_EXTEND(variable_type, 0)
        FLTL_CFG_UNBOUND_PRODUCTION_PATTERN_EXTEND(variable_type, 0)

        FLTL_CFG_PRODUCTION_PATTERN_EXTEND(terminal_type, 0)
        FLTL_CFG_UNBOUND_PRODUCTION_PATTERN_EXTEND(terminal_type, 0)

        FLTL_CFG_PRODUCTION_PATTERN_EXTEND(symbol_type, 0)

        FLTL_CFG_PRODUCTION_PATTERN_EXTEND(symbol_string_type, 0)

    };
}}}

#endif /* FLTL_PATTERN_HPP_ */
