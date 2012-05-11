/*
 * Operator.hpp
 *
 *  Created on: May 11, 2012
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef Grail_Plus_TDOP_OPERATOR_HPP_
#define Grail_Plus_TDOP_OPERATOR_HPP_

namespace fltl { namespace tdop {

    /// represents an operator within a rule
    template <typename AlphaT>
    class Operator {
    private:

        friend class OpaqueCategory<AlphaT>;
        friend class Symbol<AlphaT>;

        FLTL_TDOP_USE_TYPES(TDOP<AlphaT>);

        enum {
            BOUND_SYMBOL = 0
        };

        term_type term;
        uint32_t lower_bound;

    public:

        bool match(term_type &) const throw();
        bool match(category_type &) const throw();
        bool match(category_type &, unsigned &) const throw();

        bool is_predicate(void) const throw();
        bool is_bound(void) const throw();
    };

}}

#endif /* Grail_Plus_TDOP_OPERATOR_HPP_ */
