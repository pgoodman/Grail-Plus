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

namespace fltl { namespace cfg {

    /// pattern type, nicely encapsulates a destructuring production
    /// pattern
    template <typename AlphaT>
    class OpaquePattern {
    private:

        friend class CFG<AlphaT>;

        typedef Generator<AlphaT> generator_type;
        typedef OpaqueProduction<AlphaT> production_type;
        typedef OpaquePattern<AlphaT> self_type;

        detail::PatternData<AlphaT> *pattern;

        /// match a pattern; used by match()
        bool (*match_pattern)(detail::PatternData<AlphaT> *, const production_type &);

        /// generator functions so that patterns can be used in generators
        bool (*gen_next)(generator_type *);
        void (*gen_reset)(generator_type *);
        void (*gen_free)(generator_type *);

        /// fail to match a null pattern
        static bool default_match_pattern(
            detail::PatternData<AlphaT> *,
            const production_type &
        ) throw() {
            return false;
        }

        /// fail to generate anything on a null pattern
        static bool default_gen_next(generator_type *) throw() {
            return false;
        }

        /// do nothing to reset a null generator
        static void default_gen_reset(generator_type *) throw() { }

    public:

        OpaquePattern(void) throw()
            : pattern(0)
            , match_pattern(&default_match_pattern)
            , gen_next(&default_gen_next)
            , gen_reset(&default_gen_reset)
            , gen_free(&default_gen_reset)
        { }

        OpaquePattern(self_type &that) throw()
            : pattern(that.pattern)
            , match_pattern(that.match_pattern)
            , gen_next(that.gen_next)
            , gen_reset(that.gen_reset)
            , gen_free(that.gen_free)
        {
            detail::PatternData<AlphaT>::incref(pattern);
        }


        template <typename PatternT, typename StringT, const unsigned state>
        OpaquePattern(
            detail::PatternBuilder<AlphaT,PatternT,StringT,state> pattern_builder
        ) throw()
            : pattern(pattern_builder.pattern)
            , match_pattern(&(detail::PatternBuilder<AlphaT,PatternT,StringT,state>::static_match))
            , gen_next(&(detail::PatternGenerator<
                AlphaT,
                detail::PatternBuilder<AlphaT,PatternT,StringT,state>
            >::bind_next_pattern))
            , gen_reset(&(detail::PatternGenerator<
                AlphaT,
                detail::PatternBuilder<AlphaT,PatternT,StringT,state>
            >::reset_next_pattern))
            , gen_free(&(detail::SimpleGenerator<AlphaT>::free_next_production))
        {
            detail::PatternData<AlphaT>::incref(pattern);
        }

        ~OpaquePattern(void) throw() {
            if(0 != pattern) {
                detail::PatternData<AlphaT>::decref(pattern);
                pattern = 0;
            }
        }

        // make a pattern from a builder
        template <typename PatternT, typename StringT, const unsigned state>
        self_type &operator=(
            detail::PatternBuilder<AlphaT,PatternT,StringT,state> pattern_builder
        ) throw() {

            typedef detail::PatternBuilder<AlphaT,PatternT,StringT,state>
                    builder_type;

            // manage refcounts
            if(0 != pattern) {
                detail::PatternData<AlphaT>::decref(pattern);
            }

            pattern = pattern_builder.pattern;
            detail::PatternData<AlphaT>::incref(pattern);

            // add in the pattern
            match_pattern = &(builder_type::static_match);

            gen_next = &(detail::PatternGenerator<
                AlphaT,
                builder_type
            >::bind_next_pattern);

            gen_reset = &(detail::PatternGenerator<
                AlphaT,
                builder_type
            >::reset_next_pattern);

            gen_free = &(detail::SimpleGenerator<
                AlphaT
            >::free_next_production);

            return *this;
        }

        self_type &operator=(self_type &that) throw() {

            // manage refcounts
            if(0 != pattern) {
                detail::PatternData<AlphaT>::decref(pattern);
            }

            pattern = that.pattern;
            detail::PatternData<AlphaT>::incref(pattern);

            match_pattern = that.match_pattern;
            gen_next = that.gen_next;
            gen_reset = that.gen_reset;
            gen_free = that.gen_free;

            return *this;
        }

        /// try to match against the pattern
        inline bool match(const production_type &prod) const throw() {
            return match_pattern(pattern, prod);
        }
    };

}}

#endif /* FLTL_OPAQUEPATTERN_HPP_ */
