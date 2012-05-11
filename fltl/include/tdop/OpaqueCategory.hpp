/*
 * OpaqueCategory.hpp
 *
 *  Created on: May 10, 2012
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef Grail_Plus_TDOP_OPAQUECATEGORY_HPP_
#define Grail_Plus_TDOP_OPAQUECATEGORY_HPP_

namespace fltl { namespace tdop {

    template <typename AlphaT>
    class OpaqueCategory {
    private:

        friend class Term<AlphaT>;

        FLTL_TDOP_USE_TYPES(TDOP<AlphaT>);

        Category<AlphaT> *category;

    public:

        const operator_type operator()(void) throw() {
            // TODO
        }

        const operator_type operator()(const unsigned lower_bound) throw() {
            // TODO
        }

    };

}}


#endif /* Grail_Plus_TDOP_OPAQUECATEGORY_HPP_ */
