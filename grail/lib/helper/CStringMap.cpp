/*
 * CStringMap.cpp
 *
 *  Created on: Feb 16, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef FLTL_CSTRINGMAP_CPP_
#define FLTL_CSTRINGMAP_CPP_

#include "grail/include/helper/CStringMap.hpp"

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
