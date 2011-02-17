/*
 * CStringMap.cpp
 *
 *  Created on: Feb 16, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_CSTRINGMAP_CPP_
#define FLTL_CSTRINGMAP_CPP_

#include "grail/include/CStringMap.hpp"

namespace std {

    bool less<std::pair<const char *, const char *> >::operator() (
        const std::pair<const char *, const char *>& x,
        const std::pair<const char *, const char *>& y
    ) const {
        const size_t ll(static_cast<unsigned>(x.second - x.first));
        const size_t rr(static_cast<unsigned>(y.second - y.first));
        if(ll < rr) {
            return true;
        } else if(ll == rr) {
            return 0 > strncmp(x.first, y.first, ll);
        } else {
            return false;
        }
    }
}

#endif /* FLTL_CSTRINGMAP_CPP_ */
