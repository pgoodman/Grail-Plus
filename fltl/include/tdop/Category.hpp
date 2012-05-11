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

    template <typename AlphaT>
    class Category {
    private:

        friend class Term<AlphaT>;

        void incref(void) throw();
        void decref(void) throw();

    public:
    };

}}

#endif /* Grail_Plus_TDOP_CATEGORY_HPP_ */
