/*
 * RemoveReference.hpp
 *
 *  Created on: May 8, 2012
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef Grail_Plus_REMOVEREFERENCE_HPP_
#define Grail_Plus_REMOVEREFERENCE_HPP_

namespace fltl { namespace mpl {

    template <typename T>
    class RemoveReference {
    public:
        typedef T type;
        typedef const T const_type;
    };

    template <typename T>
    class RemoveReference<T &> {
    public:
        typedef T type;
        typedef const T const_type;
    };
}}


#endif /* Grail_Plus_REMOVEREFERENCE_HPP_ */
