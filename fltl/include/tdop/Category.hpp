/*
 * Category.hpp
 *
 *  Created on: May 10, 2012
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef Grail_Plus_TDOP_CATEGORY_HPP_
#define Grail_Plus_TDOP_CATEGORY_HPP_

namespace fltl { namespace tdop {

    /// a TDOP parser category of rules.
    template <typename AlphaT>
    class Category {
    private:

        friend class Term<AlphaT>;
        friend class OpaqueCategory<AlphaT>;
        friend class OpaqueRule<AlphaT>;
        friend class Rule<AlphaT>;

        typedef Category<AlphaT> self_type;

        /// the "number"/id of this parsercategory
        uint32_t number;

        /// previous and next categories within this TDOP machine
        self_type *prev;
        self_type *next;

        /// first initial and extension rules
        Rule<AlphaT> *first_initial_rule;
        Rule<AlphaT> *last_extension_rule;

        unsigned num_initial_rules;
        unsigned num_extension_rules;

    public:

        // TODO
    };

}}

#endif /* Grail_Plus_TDOP_CATEGORY_HPP_ */
