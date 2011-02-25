/*
 * Alphabet.hpp
 *
 *  Created on: Feb 24, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_ALPHABET_HPP_
#define FLTL_ALPHABET_HPP_

#include <functional>
#include <cstring>

namespace fltl { namespace trait {

    namespace detail {
        template <typename T>
        class AlphabetBase {
        public:
            typedef T alphabet_type;
            typedef std::less<T> less_type;

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

    template <typename T>
    class Alphabet : public T {
    public:
        using typename T::alphabet_type;
        using typename T::less_type;
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
