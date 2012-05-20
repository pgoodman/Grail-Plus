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

        friend class TDOP<AlphaT>;
        friend class Term<AlphaT>;
        friend class Operator<AlphaT>;
        friend class OpaqueRule<AlphaT>;

        friend class detail::CategoryGenerator<AlphaT>;

        FLTL_TDOP_USE_TYPES(TDOP<AlphaT>);

        /// construction from other places, e.g. OpaqueRule.
        OpaqueCategory(unsigned num) throw()
            : Term<AlphaT>(static_cast<int32_t>(num))
        { }

        OpaqueCategory(Category<AlphaT> *cat) throw()
            : Term<AlphaT>(static_cast<int32_t>(cat->number))
        { }

    public:

        typedef category_type self_type;
        typedef category_tag tag_type;

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

        /// for creating operators with definite restrictions
        const operator_type
        operator()(const unsigned lower_bound) const throw() {
            return operator_type(*this, lower_bound);
        }

        const operator_type
        operator()(void) const throw() {
            return operator_type(*this);
        }

        /// for creating an unbound category
        const Unbound<AlphaT,category_tag>
        operator~(void) const throw() {
            return Unbound<AlphaT,category_tag>(this);
        }

        /// for creating a bound category, unbound lower bound
        const Bound<AlphaT,category_lb_tag>
        operator[](unsigned &lower_bound) const throw() {
            return Bound<AlphaT,category_lb_tag>(this, &lower_bound);
        }

        // an initial rule, with a definite category
        FLTL_TDOP_RULE_PATTERN(category_tag)

        /// properties
        unsigned number(void) const throw() {
            return static_cast<unsigned>(this->val);
        }
    };

}}


#endif /* Grail_Plus_TDOP_OPAQUECATEGORY_HPP_ */
