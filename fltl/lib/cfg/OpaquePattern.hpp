/*
 * OpaquePattern.hpp
 *
 *  Created on: Feb 4, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_OPAQUEPATTERN_HPP_
#define FLTL_OPAQUEPATTERN_HPP_

namespace fltl { namespace lib { namespace cfg {

    /// pattern type, nicely encapsulates a destructuring production
    /// pattern
    template <typename AlphaT>
    class OpaquePattern {
    private:

        friend class CFG<AlphaT>;

        typedef Generator<AlphaT> generator_type;
        typedef OpaqueProduction<AlphaT> production_type;
        typedef OpaquePattern<AlphaT> self_type;

        cfg::PatternData<AlphaT> *pattern;
        bool (*match_pattern)(cfg::PatternData<AlphaT> *, const production_type &);
        bool (*gen_next)(generator_type *);
        void (*gen_reset)(generator_type *);

    public:

        OpaquePattern(self_type &that) throw()
            : pattern(that.pattern)
            , match_pattern(that.match_pattern)
            , gen_next(that.gen_next)
            , gen_reset(that.gen_reset)
        {
            cfg::PatternData<AlphaT>::incref(pattern);
        }

        template <typename PatternT, typename StringT, const unsigned state>
        OpaquePattern(
            cfg::detail::PatternBuilder<AlphaT,PatternT,StringT,state> pattern_builder
        ) throw()
            : pattern(pattern_builder.pattern)
            , match_pattern(&(cfg::detail::PatternBuilder<AlphaT,PatternT,StringT,state>::static_match))
            , gen_next(&(cfg::detail::PatternGenerator<
                AlphaT,
                cfg::detail::PatternBuilder<AlphaT,PatternT,StringT,state>
            >::bind_next_pattern))
            , gen_reset(&(cfg::detail::PatternGenerator<
                AlphaT,
                cfg::detail::PatternBuilder<AlphaT,PatternT,StringT,state>
            >::reset_next_pattern))
        {
            cfg::PatternData<AlphaT>::incref(pattern);
        }

        ~OpaquePattern(void) throw() {
            cfg::PatternData<AlphaT>::decref(pattern);
            pattern = 0;
        }

        template <typename PatternT, typename StringT, const unsigned state>
        self_type &operator=(
            cfg::detail::PatternBuilder<AlphaT,PatternT,StringT,state> pattern_builder
        ) throw() {

            // manage refcounts
            cfg::PatternData<AlphaT>::decref(pattern);
            pattern = pattern_builder.pattern;
            cfg::PatternData<AlphaT>::incref(pattern);

            // add in the pattern
            match_pattern = &(cfg::detail::PatternBuilder<AlphaT,PatternT,StringT,state>::static_match);

            gen_next = &(cfg::detail::PatternGenerator<
                AlphaT,
                cfg::detail::PatternBuilder<AlphaT,PatternT,StringT,state>
            >::bind_next_pattern);

            gen_reset = &(cfg::detail::PatternGenerator<
                AlphaT,
                cfg::detail::PatternBuilder<AlphaT,PatternT,StringT,state>
            >::reset_next_pattern);

            return *this;
        }

        self_type &operator=(self_type &that) throw() {

            // manage refcounts
            cfg::PatternData<AlphaT>::decref(pattern);
            pattern = that.pattern;
            cfg::PatternData<AlphaT>::incref(pattern);

            match_pattern = that.match_pattern;
            gen_next = that.gen_next;
            gen_reset = that.gen_reset;

            return *this;
        }

        /// try to match against the pattern
        inline bool match(const production_type &prod) const throw() {
            return match_pattern(pattern, prod);
        }
    };

}}}

#endif /* FLTL_OPAQUEPATTERN_HPP_ */
