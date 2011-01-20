/*
 * Tuple.hpp
 *
 *  Created on: Sep 16, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef FLTL_STL_TUPLE_HPP_
#define FLTL_STL_TUPLE_HPP_

#include <iostream>
#include <cstdarg>

#include "fltl/include/preprocessor/TEMPLATE_VARIABLE_LIMIT.hpp"
#include "fltl/include/preprocessor/ENUMERATE_PARAMS.hpp"
#include "fltl/include/preprocessor/ENUMERATE_VALUE_PARAMS.hpp"

#include "fltl/include/mpl/Unit.hpp"
#include "fltl/include/mpl/Sequence.hpp"
#include "fltl/include/mpl/SizeOf.hpp"

#include "fltl/include/helper/VarArgPromotion.hpp"

#include "fltl/include/trait/Uncopyable.hpp"

#define FLTL_TUPLE_TYPENAME_DEFAULT_LIST \
    typename T0 \
    FLTL_ENUMERATE_VALUE_PARAMS(FLTL_TEMPLATE_VARIABLE_LIMIT,T,typename,= mpl::Unit)

#define FLTL_TUPLE_TYPENAME_LIST \
    typename T0 FLTL_ENUMERATE_PARAMS(FLTL_TEMPLATE_VARIABLE_LIMIT, typename T)

#define FLTL_TUPLE_TYPE_PARAM_LIST \
    T0 FLTL_ENUMERATE_PARAMS(FLTL_TEMPLATE_VARIABLE_LIMIT,T)

#define FLTL_TUPLE_BEFRIEND_PRINTER(n, _) \
    friend class detail::TuplePrintStorage<n, self_type>;

#define FLTL_TUPLE_PADDING_TYPE_METHODS \
    TuplePaddedType(void) : value() { } \
    TuplePaddedType(const self_type &other) : value(other.value) { } \
    self_type &operator=(const self_type &other) { \
        value = other.value; \
        return *this; \
    } \

#define FLTL_TUPLE_INITIALIZE_NTH(n, var) \
    case n: Assign<T ## n, typename helper::VarArgPromotion<T ## n>::type, n>::apply(*this, var); break;

namespace fltl { namespace lib {

    /// forward-declaration of tuple type. the tuple type contains one of
    /// each of the types T0, ..., TN. All types in the tuple are expected
    /// to have default constructors. The tuple type does not effect an
    /// efficient packing of the data, in fact, it adds padding in some
    /// cases. the result is that a tuple can be larger than a class/struct
    /// of equivalent definition. The benefit of the tuple is that its easy
    /// to use.
    ///
    /// Note: - the mpl::Unit type is not meant to be stored within a
    ///         tuple and will result in a compiler error.
    template <FLTL_TUPLE_TYPENAME_DEFAULT_LIST>
    class Tuple;

    namespace detail {

        enum {
            TUPLE_ALIGN_BYTE_LEN = sizeof(unsigned)
        };

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
            typedef TuplePaddedType<T, true> self_type;
            T value;

            FLTL_TUPLE_PADDING_TYPE_METHODS
        };

        template <typename T>
        class TuplePaddedType<T, false> {
        public:
            typedef TuplePaddedType<T, true> self_type;
            T value;

            char padding[TUPLE_ALIGN_BYTE_LEN - (
                mpl::SizeOf<T>::VALUE % TUPLE_ALIGN_BYTE_LEN
            )];

            FLTL_TUPLE_PADDING_TYPE_METHODS
        };

        /// the size of an object stored inside a tuple
        template <typename T>
        class TupleSizeOf {
        public:
            enum {
                VALUE = sizeof(TuplePaddedType<
                    T,
                    0 == (mpl::SizeOf<T>::VALUE % TUPLE_ALIGN_BYTE_LEN)
                >)
            };
        };

        /// Recursive type for computing the byte offset of something in
        /// a tuple's storage
        template <const unsigned i, typename SequenceType>
        class TupleOffsetOf {
        public:
            enum {
                VALUE = (TupleSizeOf<
                    typename SequenceType::template At<i - 1>::type
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

        /// print out an individual value from inside the tuple. this handles
        /// printing all but the first value in the tuple.
        template <typename ValueType, typename TupleType, const unsigned i>
        class TupleTypePrinter {
        public:
            inline static void apply(std::ostream &os, const TupleType &tuple) {
                os << ", " << tuple.template get<i>();
            }
        };

        template <typename TupleType, const unsigned i>
        class TupleTypePrinter<mpl::Unit, TupleType, i> {
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
                typedef typename TupleType::sequence_type::template At<
                    i
                >::type value_type;

                // print the value
                TupleTypePrinter<value_type, TupleType, i>::apply(os, tuple);
            }
        };

        /// base case of recursion, print the first value.
        template <typename TupleType>
        class TuplePrintStorage<0, TupleType> {
        public:
            inline static void apply(std::ostream &os, const TupleType &tuple) {
                os << tuple.template get<0>();
            }
        };
    }

    /// forward declaration of stream function
    template <FLTL_TUPLE_TYPENAME_LIST >
    std::ostream &
    operator<<(std::ostream &, const Tuple<FLTL_TUPLE_TYPE_PARAM_LIST> &);

    template <FLTL_TUPLE_TYPENAME_LIST>
    class Tuple {
    public:

        /// basic useful typedefs
        typedef Tuple<FLTL_TUPLE_TYPE_PARAM_LIST> self_type;
        typedef mpl::Sequence<FLTL_TUPLE_TYPE_PARAM_LIST> sequence_type;

    private:

        FLTL_TUPLE_BEFRIEND_PRINTER(0, void)
        FLTL_REPEAT_LEFT(
            FLTL_TEMPLATE_VARIABLE_LIMIT,
            FLTL_TUPLE_BEFRIEND_PRINTER,
            void
        )

        /// assignment type for arguments
        template <typename ArgType, typename PromotedType, const unsigned i>
        class Assign {
        public:
            inline static void apply(self_type &tuple, va_list &args) {
                tuple.get<i>() = static_cast<ArgType>(
                    va_arg(args, PromotedType)
                );
            }
        };

        template <typename PromotedType, const unsigned i>
        class Assign<mpl::Unit, PromotedType, i> {
        public:
            inline static void apply(self_type &, va_list &) { }
        };

        /// define how the types are stored. storage for types T_0, T_1, ...,
        /// T_N-1 is defined by the recurrence:
        ///
        /// STORAGE(i) = T_i-1 * STORAGE(i + 1), for i < N
        /// STORAGE(i) = T_i-1, for i >= N
        template <typename PrevType, typename CurrType, const unsigned i>
        class Storage {
        public:

            typedef Storage<PrevType,CurrType,i> self_type;

            /// first value to store
            detail::TuplePaddedType<
                PrevType,
                0 == (sizeof(PrevType) % sizeof(unsigned))
            > first;

            /// second value to store
            Storage<
                CurrType,
                typename sequence_type::template At<i+1,mpl::Unit>::type,
                i + 1
            > rest;

            /// constructors
            Storage(void) : first(), rest() { }
            Storage(const self_type &other)
             : first(other.first), rest(other.rest) { }

            self_type &operator=(const self_type &other) {
                first = other.first;
                rest = other.rest;
                return *this;
            }
        };

        /// base case of induction for the storage type
        template <typename PrevType, const unsigned i>
        class Storage<PrevType, mpl::Unit, i> {
        public:

            typedef Storage<PrevType, mpl::Unit, i> self_type;

            detail::TuplePaddedType<
                PrevType,
                0 == (sizeof(PrevType) % sizeof(unsigned))
            > first;

            /// constructors
            Storage(void) : first() { }
            Storage(const self_type &other)
             : first(other.first) { }

            self_type &operator=(const self_type &other) {
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
        Tuple(const self_type &other) : storage(other.storage) { }

        /// initial value constructor, expects *all* values to be set. if
        /// not all values are set then unsafe things can happen!
        ///
        /// Note: - all arguments are passed by *value*
        ///       - this method isn't type safe and so passing the wrong
        ///         number of arguments won't be found by the compiler.
        Tuple(const T0 val0, ...) {

            get<0>() = val0;

            va_list tuple_args;
            va_start(tuple_args, val0);
            for(unsigned i(1); i < sequence_type::Length::VALUE; ++i) {
                switch(i) {
                    FLTL_REPEAT_LEFT(
                        FLTL_TEMPLATE_VARIABLE_LIMIT,
                        FLTL_TUPLE_INITIALIZE_NTH,
                        tuple_args
                    )
                }
            }
            va_end(tuple_args);
        }

        ~Tuple(void) { }

        /// copy assignment operator
        self_type &operator=(const self_type &other) {
            storage = other.storage;
            return *this;
        }

        /// get a reference to the element at a given index
        template <const unsigned i>
        inline typename sequence_type::template At<i>::type &
        get(void) throw() {
            typedef typename sequence_type::template At<i>::type value_type;
            return *reinterpret_cast<value_type *>(
                reinterpret_cast<char *>(&storage)
              + detail::TupleOffsetOf<i, sequence_type>::VALUE
            );
        }

        /// get a const reference to the element at a given index
        template <unsigned i>
        inline const typename sequence_type::template At<i>::type &
        get(void) const throw() {
            typedef typename sequence_type::template At<i>::type value_type;
            return *reinterpret_cast<const value_type *>(
                reinterpret_cast<const char *>(&storage)
              + detail::TupleOffsetOf<i, sequence_type>::VALUE
            );
        }
    };

    /// stream the tuple to output
    template <FLTL_TUPLE_TYPENAME_LIST >
    std::ostream &
    operator<<(std::ostream &os,
               const Tuple<FLTL_TUPLE_TYPE_PARAM_LIST> &tuple) {

        typedef Tuple<FLTL_TUPLE_TYPE_PARAM_LIST> tuple_type;

        os << '[';
        detail::TuplePrintStorage<
            tuple_type::sequence_type::Length::VALUE - 1,
            tuple_type
        >::apply(os, tuple);
        return os << ']';
    }

    /// simple interface for getting an element out of a tuple.

    template <const unsigned i, FLTL_TUPLE_TYPENAME_LIST >
    inline
    typename mpl::Sequence<FLTL_TUPLE_TYPE_PARAM_LIST>::\
    template At<i>::type &
    get(Tuple<FLTL_TUPLE_TYPE_PARAM_LIST> &tuple) throw() {
        return tuple.template get<i>();
    }

    template <const unsigned i, FLTL_TUPLE_TYPENAME_LIST >
    inline
    const typename mpl::Sequence<FLTL_TUPLE_TYPE_PARAM_LIST>::\
    template At<i>::type &
    get(const Tuple<FLTL_TUPLE_TYPE_PARAM_LIST> &tuple) throw() {
        return tuple.template get<i>();
    }
}}

#endif /* FLTL_STL_TUPLE_HPP_ */
