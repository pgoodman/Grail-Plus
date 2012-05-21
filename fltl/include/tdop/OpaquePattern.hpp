/*
 * OpaquePattern.hpp
 *
 *  Created on: May 18, 2012
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef Grail_Plus_TDOP_OPAQUEPATTERN_HPP_
#define Grail_Plus_TDOP_OPAQUEPATTERN_HPP_

namespace fltl { namespace tdop {

    /// represents an external representation of a TDOP parser rule pattern
    template <typename AlphaT>
    class OpaquePattern {
    private:

        friend class TDOP<AlphaT>;

        typedef detail::PatternData<AlphaT> data_type;
        typedef OpaquePattern<AlphaT> self_type;

        data_type *pattern;
        bool (*bind)(data_type *, const OpaqueRule<AlphaT> &);

    public:

        /// constructors
        template <
            typename A,
            typename B,
            typename C,
            const bool D // true iff an operator string can be appended
        >
        OpaquePattern(const detail::PatternBuilder<A,B,C,D> pat) throw()
            : pattern(pat.pattern)
            , bind(pattern::DestructuringBind<A,B,C>::bind)
        {
            data_type::incref(pattern);
        }

        OpaquePattern(const self_type &that) throw()
            : pattern(that.pattern)
            , bind(that.bind)
        {
            data_type::incref(pattern);
        }

        ~OpaquePattern(void) throw() {
            data_type::decref(pattern);
        }

        /// assignment operators
        template <
            typename A,
            typename B,
            typename C,
            const bool D // true iff an operator string can be appended
        >
        self_type &operator=(const detail::PatternBuilder<A,B,C,D> that) throw() {
            data_type::decref(pattern);
            pattern = that.pattern;
            bind = pattern::DestructuringBind<A,B,C>::bind;
            data_type::incref(pattern);
            return *this;
        }

        self_type &operator=(const self_type &that) throw() {
            data_type::decref(pattern);
            pattern = that.pattern;
            bind = that.bind;
            data_type::incref(pattern);
            return *this;
        }

        /// match a rule
        bool match(const OpaqueRule<AlphaT> &rule) const throw() {
            if(0 == pattern || !rule.is_valid()) {
                return false;
            }

            return bind(pattern, rule);
        }
    };

}}

#endif /* Grail_Plus_TDOP_OPAQUEPATTERN_HPP_ */
