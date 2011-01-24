/*
 * AlignmentOf.hpp
 *
 *  Created on: Jan 17, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_ALIGNMENTOF_HPP_
#define FLTL_ALIGNMENTOF_HPP_

namespace fltl { namespace trait {

    namespace detail {
        template <typename T>
        class AlignmentFinder {
        public:
            char foo;
            T bar;
        };
    }

    template <typename T>
    class AlignmentOf {
    public:
        enum {
            VALUE = sizeof(detail::AlignmentFinder<T>) - sizeof(T)
        };
    };
}}

#endif /* FLTL_ALIGNMENTOF_HPP_ */
