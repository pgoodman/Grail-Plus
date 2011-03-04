/*
 * RemoveConst.hpp
 *
 *  Created on: Mar 3, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_REMOVECONST_HPP_
#define FLTL_REMOVECONST_HPP_

namespace fltl { namespace mpl {

    template <typename T>
    class RemoveConst {
    public:
        typedef T type;
    };

    template <typename T>
    class RemoveConst<const T> {
    public:
        typedef T type;
    };

}}

#endif /* FLTL_REMOVECONST_HPP_ */
