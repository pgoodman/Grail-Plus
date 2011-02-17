/*
 * CStringMap.hpp
 *
 *  Created on: Feb 16, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
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

namespace grail {

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
}

#endif /* FLTL_CSTRINGMAP_HPP_ */
