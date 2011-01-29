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

namespace fltl { namespace lib { namespace cfg {

    namespace detail {

        template <typename AlphaT, typename T>
        class PatternElem {

        };

        template <typename AlphaT, typename T0, typename T1>
        class PatternPair {

        };
    }

    /// pattern where the variable is bound
    template <typename AlphaT, typename RHS>
    class Pattern<
        AlphaT,
        detail::PatternElem<AlphaT, typename CFG<AlphaT>::variable_type>,
        RHS
    > {

    };

    /// pattern where the variable is unbound
    template <typename AlphaT, typename RHS>
    class Pattern<
        AlphaT,
        detail::PatternElem<
            AlphaT,
            UnboundSymbol<AlphaT, typename CFG<AlphaT>::variable_type>
        >,
        RHS
    > {

    };
}}}

#endif /* FLTL_PATTERN_HPP_ */
