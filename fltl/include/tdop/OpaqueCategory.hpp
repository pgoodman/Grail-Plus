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
    class OpaqueCategory : public Term<AlphaT> {
    private:

        friend class Term<AlphaT>;
        friend class Operator<AlphaT>;

        FLTL_TDOP_USE_TYPES(TDOP<AlphaT>);

        /// construction from other places, e.g. OpaqueRule.
        OpaqueCategory(unsigned num) throw()
            : Term<AlphaT>(static_cast<int32_t>(num))
        { }

    public:

        /// constructors, destructors
        OpaqueCategory(void) throw()
            : Term<AlphaT>(Term<AlphaT>::DEFAULT_CATEGORY)
        { }

        OpaqueCategory(const category_type &that) throw()
            : Term<AlphaT>(that)
        { }

        OpaqueCategory(const term_type &that) throw() {
            assert(that.is_category());
            this->val = that.val;
        }

        /// assignment operators
        category_type operator=(const category_type &that) throw() {
            this->val = that.val;
            return *this;
        }

        category_type operator=(const term_type &that) throw() {
            assert(that.is_category());
            this->val = that.val;
            return *this;
        }

        /// pattern matching
        /*
        const operator_type operator()(void) throw() {
            // TODO
        }

        const operator_type operator()(const unsigned lower_bound) throw() {
            // TODO
        }*/
    };

}}


#endif /* Grail_Plus_TDOP_OPAQUECATEGORY_HPP_ */
