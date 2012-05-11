/*
 * Rule.hpp
 *
 *  Created on: May 10, 2012
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef Grail_Plus_TDOP_RULE_HPP_
#define Grail_Plus_TDOP_RULE_HPP_

namespace fltl { namespace tdop {

    /// represents a parser rule. This encompasses both initial and extension
    /// rules. An initial rule is has a termina/symbol left corner. An extension
    /// rule has as its left corner another rule. The extenstion rule is
    /// anchored by a symbol, or a symbol predicate.
    template <typename AlphaT>
    class Rule {
    private:

        enum {
            INITIAL_RULE_UPPER_BOUND = -1
        };

        int32_t upper_bound;

    public:


    };

}}

#endif /* Grail_Plus_TDOP_RULE_HPP_ */
