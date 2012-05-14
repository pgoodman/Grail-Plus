/*
 * Any.hpp
 *
 *  Created on: May 12, 2012
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef Grail_Plus_TDOP_ANY_HPP_
#define Grail_Plus_TDOP_ANY_HPP_

namespace fltl { namespace tdop {

    template <typename AlphaT>
    class AnyOperator {
    public:
        // TODO
    };

    template <typename AlphaT>
    class AnyOperatorString {
    public:

        AnyOperatorStringOfLength<AlphaT>
        operator()(unsigned &len) const throw() {
            return AnyOperatorStringOfLength<AlphaT>(len);
        }
    };

    template <typename AlphaT>
    class AnyOperatorStringOfLength {
    private:

        friend class AnyOperatorString<AlphaT>;

        typedef AnyOperatorStringOfLength<AlphaT> self_type;

        unsigned *length;

        AnyOperatorStringOfLength(unsigned *len) throw()
            : length(len)
        { }

    public:

        AnyOperatorStringOfLength(const self_type &that) throw()
            : length(that.length)
        { }
    };
}}


#endif /* Grail_Plus_TDOP_ANY_HPP_ */
