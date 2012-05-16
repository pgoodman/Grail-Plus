/*
 * Pattern.hpp
 *
 *  Created on: May 14, 2012
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef Grail_Plus_HELPER_PATTERN_HPP_
#define Grail_Plus_HELPER_PATTERN_HPP_

#include "fltl/include/mpl/If.hpp"

namespace fltl { namespace pattern {

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
    class Factor<void, offset> {
    public:
        enum {
            WIDTH = 0,
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

    template <typename T1>
    class Catenation<void,T1> {
    public:
        enum {
            WIDTH = T1::WIDTH,
            OFFSET = 0,
            MIN_NUM_SYMBOLS = T1::MIN_NUM_SYMBOLS,
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

    template <typename T1, const unsigned offset>
    class GetFactor<Catenation<void,T1>, offset> {
    public:
        typedef typename GetFactor<
            T1,
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

    template <typename AlphaT, typename StringT, const unsigned offset, typename T0, typename T1>
    class Match;

    /// base case, nothing to match.
    template <typename AlphaT, typename StringT, const unsigned offset>
    class Match<AlphaT,StringT,offset, void, void> {
    public:

        template <typename SlotT, typename SymbolT>
        inline static bool bind(SlotT *, const SymbolT *, const unsigned len) throw() {
            return 0 == len;
        }
    };

    /// pattern reset default case
    template <typename AlphaT, typename StringT, typename CurrT,const unsigned offset>
    class ResetPattern {
    public:

        template <typename SlotT>
        inline static void reset(SlotT *slots) throw() {
            ResetPattern<
                AlphaT,
                StringT,
                typename pattern::GetFactor<StringT,offset + 1>::type,
                offset + 1
            >::reset(slots);
        }
    };

    /// pattern reset base case
    template <typename AlphaT, typename StringT,const unsigned offset>
    class ResetPattern<AlphaT,StringT,void,offset> {
    public:

        template <typename SlotT>
        inline static void reset(SlotT *) throw() { }
    };

    /// used for destructuring and binding a pattern. This encompasses an
    /// entire pattern match
    template <typename AlphaT, typename TagT, typename StringT>
    class DestructuringBind;
}}

#endif /* Grail_Plus_HELPER_PATTERN_HPP_ */
