/*
 * Lambda.hpp
 *
 *  Created on: May 23, 2012
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef Grail_Plus_FLTL_MPL_LAMBDA_HPP_
#define Grail_Plus_FLTL_MPL_LAMBDA_HPP_

#include "fltl/include/helper/UnsafeCast.hpp"

namespace fltl { namespace mpl { namespace predicate {

    template <typename L, typename R>
    class BinaryPredicate {
    private:

        typedef BinaryPredicate<L,R> self_type;
        typedef bool (operator_type)(const self_type *);
        L *left;
        R *right;
        operator_type *op;

    public:

        BinaryPredicate(L &left_, R &right_, operator_type *op_) throw()
            : left(&left_)
            , right(&right_)
            , op(op_)
        { }

        bool operator()(void) const throw() {
            return op(this);
        }

        static bool less_than(const self_type *self) throw() {
            return *(self->left) < *(self->right);
        }
    };

    template <typename L, typename R>
    inline BinaryPredicate<L,R>
    less_than(L &left, R &right) throw() {
        return BinaryPredicate<L,R>(left, right, BinaryPredicate<L,R>::less_than);
    }
}}}

#endif /* Grail_Plus_FLTL_MPL_LAMBDA_HPP_ */
