/*
 * Tuple.hpp
 *
 *  Created on: Sep 16, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef CFTL_TUPLE_HPP_
#define CFTL_TUPLE_HPP_

#include <iostream>
#include <cstdarg>

#include "src/include/preprocessor/ENUMERATE_PARAMS.hpp"
#include "src/include/preprocessor/ENUMERATE_VALUE_PARAMS.hpp"

#include "src/include/mpl/Unit.hpp"
#include "src/include/mpl/Sequence.hpp"
#include "src/include/mpl/SizeOf.hpp"

#ifndef CFTL_TUPLE_OVER_TYPES_LIMIT
#define CFTL_TUPLE_OVER_TYPES_LIMIT 7
#endif

#define CFTL_TUPLE_TYPENAME_DEFAULT_LIST \
    typename T0 \
    CFTL_ENUMERATE_VALUE_PARAMS(CFTL_TUPLE_OVER_TYPES_LIMIT,T,typename,= TupleUnit)

#define CFTL_TUPLE_TYPENAME_LIST \
    typename T0 CFTL_ENUMERATE_PARAMS(CFTL_TUPLE_OVER_TYPES_LIMIT, typename T)

#define CFTL_TUPLE_TYPE_PARAM_LIST \
    T0 CFTL_ENUMERATE_PARAMS(CFTL_TUPLE_OVER_TYPES_LIMIT,T)

#define CFTL_TUPLE_BEFRIEND_PRINTER(n, _) \
    friend class TuplePrintStorage<n, self_t>;

#define CFTL_TUPLE_PADDING_TYPE_METHODS \
    TuplePaddedType(void) : value() { } \
    TuplePaddedType(const self_t &other) : value(other.value) { } \
    self_t &operator=(const self_t &other) { \
        value = other.value; \
        return *this; \
    } \

#define CFTL_TUPLE_INITIALIZE_NTH(n, var) \
    case n: Assign<T ## n, n>::apply(*this, var); break;

namespace cftl {

    namespace {

        /// hidden unit type that cannot be used outside of this module.
        class TupleUnit {
        private:
            TupleUnit(void) { }
            TupleUnit(const TupleUnit &) { }
            TupleUnit &operator=(const TupleUnit &) { return *this; }
        };
    }

    /// forward-declaration of tuple type. the tuple type contains one of
    /// each of the types T0, ..., TN. All types in the tuple are expected
    /// to have default constructors. The tuple type does not effect an
    /// efficient packing of the data, in fact, it adds padding in some
    /// cases. the result is that a tuple can be larger than a class/struct
    /// of equivalent definition. The benefit of the tuple is that its easy
    /// to use.
    template <CFTL_TUPLE_TYPENAME_DEFAULT_LIST>
    class Tuple;

    namespace {

        /// pad the representation of a type to fit into integer chunked
        /// sizes. the bool parameter is whether or not the type neatly
        /// fits.
        ///
        /// the consequence of using this is that the tuple will be wider
        /// as a result of types that need less space than integers. what
        /// this does get us is more predictability in terms of how the
        /// compiler lays out the memory of the tuple.
        template <typename, const bool>
        class TuplePaddedType;

        template <typename T>
        class TuplePaddedType<T, true> {
        public:
            typedef TuplePaddedType<T, true> self_t;
            T value;

            CFTL_TUPLE_PADDING_TYPE_METHODS
        };

        template <typename T>
        class TuplePaddedType<T, false> {
        public:
            typedef TuplePaddedType<T, true> self_t;
            T value;

            char padding[sizeof(unsigned) - (sizeof(T) % sizeof(unsigned))];

            CFTL_TUPLE_PADDING_TYPE_METHODS
        };

        /// the size of an object stored inside a tuple
        template <typename T>
        class TupleSizeOf {
        public:
            enum {
                VALUE = sizeof(TuplePaddedType<
                    T,
                    0 == (sizeof(T) % sizeof(unsigned))>
                )
            };
        };

        /// Recursive type for computing the byte offset of something in
        /// a tuple's storage
        template <const unsigned i, typename SequenceType>
        class TupleOffsetOf {
        public:
            enum {
                VALUE = (TupleSizeOf<
                    typename SequenceType::template Index<i - 1>::type_t
                >::VALUE + TupleOffsetOf<i - 1, SequenceType>::VALUE)
            };
        };

        /// base case for queries for the first element in the tuple
        template <typename SequenceType>
        class TupleOffsetOf<0, SequenceType> {
        public:
            enum {
                VALUE = 0
            };
        };

        /// print out an individual value from inside the tuple
        template <typename ValueType, typename TupleType, const unsigned i>
        class TupleTypePrinter {
        public:
            inline static void apply(std::ostream &os, const TupleType &tuple) {
                os << ", " << tuple.template get<i>();
            }
        };

        template <typename TupleType, const unsigned i>
        class TupleTypePrinter<TupleUnit, TupleType, i> {
        public:
            inline static void apply(std::ostream &, const TupleType &) { }
        };

        /// type for recursively printing the contents of a tuple
        template <const unsigned i, typename TupleType>
        class TuplePrintStorage {
        public:
            inline static void apply(std::ostream &os, const TupleType &tuple) {

                // recursively print all values that come before this one
                // first
                TuplePrintStorage<i - 1, TupleType>::apply(os, tuple);

                // the type of the value to be printed
                typedef typename TupleType::sequence_t::template Index<
                    i
                >::type_t value_t;

                // print the value
                TupleTypePrinter<value_t, TupleType, i>::apply(os, tuple);
            }
        };

        /// base case of recursion
        template <typename TupleType>
        class TuplePrintStorage<0, TupleType> {
        public:
            inline static void apply(std::ostream &os, const TupleType &tuple) {
                os << tuple.template get<0>();
            }
        };
    }

    /// size of the hidden tuple unit type
    template <>
    class SizeOf<TupleUnit> {
    public:
        enum {
            VALUE = 0
        };
    };

    /// forward declaration of stream function
    template <CFTL_TUPLE_TYPENAME_LIST >
    std::ostream &
    operator<<(std::ostream &, const Tuple<CFTL_TUPLE_TYPE_PARAM_LIST> &);

    template <CFTL_TUPLE_TYPENAME_LIST>
    class Tuple {
    public:

        /// basic useful typedefs
        typedef Tuple<CFTL_TUPLE_TYPE_PARAM_LIST> self_t;
        typedef Sequence<CFTL_TUPLE_TYPE_PARAM_LIST> sequence_t;

    private:

        CFTL_TUPLE_BEFRIEND_PRINTER(0, void)
        CFTL_REPEAT_LEFT(
            CFTL_TUPLE_OVER_TYPES_LIMIT,
            CFTL_TUPLE_BEFRIEND_PRINTER,
            void
        )

        /// define how the types are stored. storage for types T_0, T_1, ...,
        /// T_N-1 is defined by the recurrence:
        ///
        /// STORAGE(i) = T_i-1 * STORAGE(i + 1), for i < N
        /// STORAGE(i) = T_i-1, for i >= N
        template <typename PrevType, typename CurrType, const unsigned i>
        class Storage {
        public:

            typedef Storage<PrevType,CurrType,i> self_t;

            /// first value to store
            TuplePaddedType<
                PrevType,
                0 == (sizeof(PrevType) % sizeof(unsigned))
            > first;

            /// second value to store
            Storage<
                CurrType,
                typename sequence_t::template Index<i+1,TupleUnit>::type_t,
                i + 1
            > rest;

            /// constructors
            Storage(void) : first(), rest() { }
            Storage(const self_t &other)
             : first(other.first), rest(other.rest) { }

            self_t &operator=(const self_t &other) {
                first = other.first;
                rest = other.rest;
                return *this;
            }
        };

        /// assignment type for arguments
        template <typename ArgType, const unsigned i>
        class Assign {
        public:
            static void apply(self_t &tuple, va_list &args) {
                tuple.get<i>() = va_arg(args, ArgType);
            }
        };

        template <const unsigned i>
        class Assign<TupleUnit, i> {
        public:
            static void apply(self_t &, va_list &) { }
        };

        /// base case of induction for the storage type
        template <typename PrevType, const unsigned i>
        class Storage<PrevType, TupleUnit, i> {
        public:

            typedef Storage<PrevType, TupleUnit, i> self_t;

            TuplePaddedType<
                PrevType,
                0 == (sizeof(PrevType) % sizeof(unsigned))
            > first;

            /// constructors
            Storage(void) : first() { }
            Storage(const self_t &other)
             : first(other.first) { }

            self_t &operator=(const self_t &other) {
                first = other.first;
                return *this;
            }
        };

        /// where all of the values of the tuple are stored
        Storage<T0, T1, 1> storage;

    public:

        /// default initialize a tuple
        Tuple(void) : storage() { }

        /// copy constructor, recursively copies.
        Tuple(const self_t &other) : storage(other.storage) { }

        /// initial value constructor, expects *all* values to be set. if
        /// not all values are set then unsafe things can happen!
        ///
        /// Note: all arguments are passed by *value*
        Tuple(const T0 val0, ...) {

            get<0>() = val0;

            va_list tuple_args;
            va_start(tuple_args, val0);
            for(unsigned i(1); i < sequence_t::Length::VALUE; ++i) {
                switch(i) {
                    CFTL_REPEAT_LEFT(
                        CFTL_TUPLE_OVER_TYPES_LIMIT,
                        CFTL_TUPLE_INITIALIZE_NTH,
                        tuple_args
                    )
                }
            }
            va_end(tuple_args);
        }

        /// copy assignment operator
        self_t &operator=(const self_t &other) {
            storage = other.storage;
            return *this;
        }

        /// get a reference to the element at a given index
        template <const unsigned i>
        inline typename sequence_t::template Index<i>::type_t &
        get(void) {
            typedef typename sequence_t::template Index<i>::type_t value_t;
            return *reinterpret_cast<value_t *>(
                reinterpret_cast<char *>(&storage)
              + TupleOffsetOf<i, sequence_t>::VALUE
            );
        }

        /// get a const reference to the element at a given index
        template <unsigned i>
        inline const typename sequence_t::template Index<i>::type_t &
        get(void) const {
            typedef typename sequence_t::template Index<i>::type_t value_t;
            return *reinterpret_cast<const value_t *>(
                reinterpret_cast<const char *>(&storage)
              + TupleOffsetOf<i, sequence_t>::VALUE
            );
        }
    };

    /// stream the tuple to output
    template <CFTL_TUPLE_TYPENAME_LIST >
    std::ostream &
    operator<<(std::ostream &os,
               const Tuple<CFTL_TUPLE_TYPE_PARAM_LIST> &tuple) {

        typedef Tuple<CFTL_TUPLE_TYPE_PARAM_LIST> tuple_t;

        os << '[';
        TuplePrintStorage<
            tuple_t::sequence_t::Length::VALUE - 1,
            tuple_t
        >::apply(os, tuple);
        return os << ']';
    }

    /// simple interface for getting an element out of a tuple.

    template <const unsigned i, CFTL_TUPLE_TYPENAME_LIST >
    inline
    typename Sequence<CFTL_TUPLE_TYPE_PARAM_LIST>::template Index<i>::type_t &
    get(Tuple<CFTL_TUPLE_TYPE_PARAM_LIST> &tuple) {
        return tuple.template get<i>();
    }

    template <const unsigned i, CFTL_TUPLE_TYPENAME_LIST >
    inline
    const typename Sequence<CFTL_TUPLE_TYPE_PARAM_LIST>::\
    template Index<i>::type_t &
    get(const Tuple<CFTL_TUPLE_TYPE_PARAM_LIST> &tuple) {
        return tuple.template get<i>();
    }
}

#endif /* CFTL_TUPLE_HPP_ */
