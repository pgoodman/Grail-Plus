/*
 * CStringMap.hpp
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

#ifndef FLTL_CSTRINGMAP_HPP_
#define FLTL_CSTRINGMAP_HPP_

#include <map>
#include <utility>
#include <functional>
#include <cstring>

namespace std {

    template <>
    struct less<std::pair<const char *, const char *> > : binary_function<
        std::pair<const char *, const char *>,
        std::pair<const char *, const char *>,
        bool
    > {
        bool operator() (
            const std::pair<const char *, const char *>& x,
            const std::pair<const char *, const char *>& y
        ) const;
    };
}

namespace grail { namespace helper {

    /// alternative to std::map<std::string>, mainly because std::string
    /// has issues in GCC 4.1.2
    template <typename T>
    class CStringMap {
    private:

        std::map<std::pair<const char *, const char *>, T> mm;

    public:

        T &get(const char *left, const char *right) throw() {
            return mm[std::make_pair(left,right)];
        }

        T &get(const char *str) throw() {
            return mm[std::make_pair(str, str + strlen(str))];
        }

        T &get(std::pair<const char *, const char *> pp) throw() {
            return mm[pp];
        }

        void set(const char *left, const char *right, const T &val) throw() {
            mm[std::make_pair(left,right)] = val;
        }

        void set(const char *str, const T &val) throw() {
            mm[std::make_pair(str, str + strlen(str))] = val;
        }

        void set(std::pair<const char *, const char *> pp, const T &val) throw() {
            mm[pp] = val;
        }

        bool contains(const char *left, const char *right) throw() {
            return 0 != mm.count(std::make_pair(left,right));
        }

        bool contains(const char *str) throw() {
            return 0 != mm.count(std::make_pair(str, str + strlen(str)));
        }

        bool contains(std::pair<const char *, const char *> pp) throw() {
            return 0 != mm.count(pp);
        }
    };
}}

#endif /* FLTL_CSTRINGMAP_HPP_ */
