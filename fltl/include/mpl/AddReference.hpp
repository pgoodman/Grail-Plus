/*
 * AddReference.hpp
 *
 *  Created on: Sep 15, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef FLTL_ADDREFERENCE_HPP_
#define FLTL_ADDREFERENCE_HPP_

namespace fltl { namespace mpl {

    template <typename T>
    class AddReference  {
    public:
        typedef T &type;
    };

    template <typename T>
    class AddReference<T &> {
    public:
        typedef T &type;
    };
}}

#endif /* FLTL_ADDREFERENCE_HPP_ */
