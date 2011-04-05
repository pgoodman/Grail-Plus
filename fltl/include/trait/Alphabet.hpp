/*
 * Alphabet.hpp
 *
 *  Created on: Feb 24, 2011
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

#ifndef FLTL_ALPHABET_HPP_
#define FLTL_ALPHABET_HPP_

#include <functional>
#include <cstring>

#include "fltl/include/mpl/UserOperators.hpp"

namespace fltl { namespace trait {

    namespace detail {

        template <typename T>
        class AlphabetBase {
        public:
            typedef T alphabet_type;
            typedef std::less<T> less_type;
            typedef mpl::global_scope_type scope_type;

            static T copy(const T &that) throw() {
                return that;
            }

            static void destroy(T &) throw() {
                return;
            }

            static void unserialize(const char *, T &) throw() {
                assert(false && "Unimplemented.");
            }
        };
    }

    /// alphabet type
    template <typename T>
    class Alphabet : public T {
    public:
        using typename T::alphabet_type;
        using typename T::less_type;
        using typename T::scope_type;
    };

    template<>
    class Alphabet<char> : public detail::AlphabetBase<char> { };

    template<>
    class Alphabet<unsigned char> : public detail::AlphabetBase<unsigned char> { };

    template<>
    class Alphabet<short> : public detail::AlphabetBase<short> { };

    template<>
    class Alphabet<unsigned short> : public detail::AlphabetBase<unsigned short> { };

    template<>
    class Alphabet<int> : public detail::AlphabetBase<int> { };

    template<>
    class Alphabet<unsigned> : public detail::AlphabetBase<unsigned> { };

    template<>
    class Alphabet<long> : public detail::AlphabetBase<long> { };

    template<>
    class Alphabet<unsigned long> : public detail::AlphabetBase<unsigned long> { };

    template<>
    class Alphabet<const char *> {
    public:
        typedef const char *alphabet_type;
        typedef mpl::global_scope_type scope_type;

        typedef struct {
        public:
            bool operator()(const char *a, const char *b) const throw() {
                return 0 < strcmp(a, b);
            }
        } less_type;

        static const char *copy(const char *that) throw() {
            const size_t len(strlen(that));
            char *cpy(new char[len + 1]);
            strcpy(cpy, that);
            return cpy;
        }

        static void destroy(const char *that) throw() {
            if(0 != that) {
                delete [] that;
            }
        }

        static void unserialize(const char *from, const char *&to) throw() {
            to = from;
        }
    };
}}

#endif /* FLTL_ALPHABET_HPP_ */
