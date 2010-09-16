/*
 * Variant.hpp
 *
 *  Created on: Sep 14, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef CFTL_STL_VARIANT_HPP_
#define CFTL_STL_VARIANT_HPP_

#include <cassert>
#include <cstddef>
#include <iostream>

#include "src/include/preprocessor/CATENATE.hpp"
#include "src/include/preprocessor/ENUMERATE_PARAMS.hpp"
#include "src/include/preprocessor/EVAL.hpp"
#include "src/include/preprocessor/INCREMENT.hpp"
#include "src/include/preprocessor/REPEAT_LEFT.hpp"
#include "src/include/preprocessor/UNPACK.hpp"

#include "src/include/mpl/Max.hpp"
#include "src/include/mpl/Sequence.hpp"
#include "src/include/mpl/SizeOf.hpp"

#include "src/include/mpl/Destroyer.hpp"
#include "src/include/mpl/Initializer.hpp"

/// the default number of types that can be summed over
#ifndef CFTL_VARIANT_OVER_TYPES_LIMIT
#define CFTL_VARIANT_OVER_TYPES_LIMIT 7
#endif

/// an expansion of a single parameterized type with a default type of Unit
#define CFTL_VARIANT_ENUMERATED_TPL_PARAM(n, p) \
    , typename CFTL_CATENATE(p, n)=VariantUnit ## n

/// an expansion of a single value inside an enumerated type
#define CFTL_VARIANT_ENUMERATED_ENUM_PARAM(n, p) \
    , TYPE_ ## p ## n = n

/// macro for computing the size of a parameterized type by its index
#define CFTL_VARIANT_TYPE_SIZE(n, _) \
    , sizeof(T ## n)

/// define the anonymous sum unit types that are used to fill in the gaps
/// of sum type. these are anonymous so that the programmer cannot place
/// a gap in-between two parameterized concrete types.
#define CFTL_MAKE_VARIANT_UNIT(n, _) \
    class VariantUnit ## n { \
    private: \
        VariantUnit ## n(void) { } \
        VariantUnit ## n(const VariantUnit ## n &) { } \
        void operator=(const VariantUnit ## n &) { } \
    };

/// define the specializations of the unit types so that no work is done
/// to initialize or destroy the sum unit types.
#define CFTL_MAKE_VARIANT_UNIT_SPEC(n, _) \
    template <> \
    class Destroyer<VariantUnit ## n> { \
    public: \
        static void destroy(VariantUnit ## n *) { } \
        static void destroy(void *) { } \
    }; \
    template <> \
    class Initializer<VariantUnit ## n> { \
    public: \
        static void initialize(void *) { } \
    }; \
    template <> \
    class SizeOf<VariantUnit ## n> { \
    public: \
        enum { \
            VALUE = 0 \
        }; \
    };

/// create the copy constructors and assignment operators
#define CFTL_VARIANT_TYPE_METHOD(n, _) \
    Variant(const T ## n &val) : type_tag(TYPE_T ## n) { \
        Initializer<T ## n>::initialize(&storage); \
        *reinterpret_cast<T ## n *>(&storage) = val; \
    } \
    \
    self_t &operator=(const T ## n &val) { \
        if(TYPE_T ## n != type_tag) { \
            type_tag = TYPE_T ## n ; \
            destroy(); \
            Initializer<T ## n>::initialize(&storage); \
        } \
        *reinterpret_cast<T ## n *>(&storage) = val; \
        return *this; \
    }

#define CFTL_VARIANT_TYPE_PARAM_LIST \
    T0 CFTL_ENUMERATE_PARAMS(CFTL_VARIANT_OVER_TYPES_LIMIT, T)

#define CFTL_VARIANT_TYPENAME_LIST \
    typename T0 CFTL_ENUMERATE_PARAMS(CFTL_VARIANT_OVER_TYPES_LIMIT, typename T)

/// used to build the switch statement so as to be able to properly destroy
/// the held value
#define CFTL_VARIANT_DESTROY_MEM(n, _) \
    case TYPE_T ## n : Destroyer<T ## n>::destroy(&storage); break;

/// used to build the switch statement to properly initialize and copy the
/// memory.
#define CFTL_VARIANT_COPY_MEM(n, p) \
    case TYPE_T ## n : \
        Initializer<T ## n>::initialize(&storage); \
        *reinterpret_cast<T ## n *>(&storage) = p; \
        break;

#define CFTL_VARIANT_TYPE_EXTRACT_ID(n, _, rest) \
    (ExtractId<Q, T ## n, (unsigned) TYPE_T ## n, (rest)>::ID)

/// specialization of extract id class for the unit types. this passes
/// through the input so that the unit types are seen as type mismatches
#define CFTL_VARIANT_TYPE_SPEC_EXTRACT_ID(n, _) \
    template <typename Q, const unsigned id, const unsigned input> \
    class ExtractId<Q, VariantUnit ## n, id, input> { \
    public: \
        enum { ID = input }; \
    };\
    \
    template <typename Q> \
    class StreamType<Q, VariantUnit ## n> { \
    public: \
        typedef const char type_t; \
    };

#define CFTL_VARIANT_STREAM_SWITCH(n, ss) \
    case variant_t::TYPE_T ## n : ss << sum.template streamGet<T ## n>(); break;

namespace cftl {

    namespace {

        /// type used for calculating how much extra space is needed to add
        /// on to the end of the storage so as to be able to hold a value of
        /// any of the types in the Variant

        template <typename T, const std::size_t k>
        class Padding {
        public:
            enum { VALUE = sizeof(T) - k };
        };

        template <typename T>
        class Padding<T, 0UL> {
        public:
            enum { VALUE = 0 };
        };

        /// make the unit classes. these classes are meant to be distinct.
        /// also, they are in the anonymous namespace and so they cannot be
        /// used by someone outside of this file, i.e. no one can ever
        /// make a sum that explicity uses one of the VariantUnit classes.
        CFTL_REPEAT_LEFT(CFTL_VARIANT_OVER_TYPES_LIMIT, CFTL_MAKE_VARIANT_UNIT, void)
    }

    /// create the specializations of the sum types for Initializer and
    /// Destroyer.
    CFTL_REPEAT_LEFT(
        CFTL_VARIANT_OVER_TYPES_LIMIT,
        CFTL_MAKE_VARIANT_UNIT_SPEC,
        void
    )

    /// forward declaration of sum type
    template <typename T0 \
              CFTL_REPEAT_LEFT(
                  CFTL_VARIANT_OVER_TYPES_LIMIT,
                  CFTL_VARIANT_ENUMERATED_TPL_PARAM,
                  T
              )>
    class Variant;

    /// forward declaration of stream function
    template <CFTL_VARIANT_TYPENAME_LIST >
    std::ostream &
    operator<<(std::ostream &, const Variant<CFTL_VARIANT_TYPE_PARAM_LIST> &);

    /// Definition for a tagged variant/union/sum type.
    ///
    /// Note: - the sum type expects all non-Unit types to be
    ///         distinct.
    ///       - types in the sum are expected to have default initializers.
    ///       - types in the sum are expected to have copy constructors.
    template < CFTL_VARIANT_TYPENAME_LIST >
    class Variant {
    public:

        /// the type of this sum
        typedef Variant< CFTL_VARIANT_TYPE_PARAM_LIST > self_t;

    private:

        /// streaming operator
        friend std::ostream &operator<< <CFTL_VARIANT_TYPE_PARAM_LIST>(
            std::ostream &,
            const self_t &
        );

        /// enumeration type defining valid accessors
        typedef enum {
            TYPE_T0 = 0
            CFTL_REPEAT_LEFT(
                CFTL_VARIANT_OVER_TYPES_LIMIT,
                CFTL_VARIANT_ENUMERATED_ENUM_PARAM,
                T
            ),
            TYPE_UNKNOWN
        } tag_t;

        typedef unsigned storage_t;

        enum {

            STORAGE_SIZE = sizeof(storage_t),

            /// the maximum size of all object sizes in this sum type
            MAX_OBJECT_SIZE = Max<
                sizeof(storage_t),
                sizeof(T0)
                CFTL_REPEAT_LEFT(
                    CFTL_VARIANT_OVER_TYPES_LIMIT,
                    CFTL_VARIANT_TYPE_SIZE,
                    STORAGE_SIZE
                )
            >::VALUE,

            /// calculate the how many values of storage_t are needed to
            /// store the largest object of the sum
            MOS_DIV = MAX_OBJECT_SIZE / STORAGE_SIZE,
            MOS_MOD = MAX_OBJECT_SIZE % STORAGE_SIZE,

            PADDING = Padding<storage_t, MOS_MOD>::VALUE,
            MEMORY_SIZE = MOS_DIV + PADDING
        };

        /// type tag
        tag_t type_tag;

        /// memory in which a value of any of the types T_i will be stored
        storage_t storage[MEMORY_SIZE];

        /// call the proper destructor of whatever type is being pointed to.
        void destroy(void) {
            switch(type_tag) {
                CFTL_VARIANT_DESTROY_MEM(0, void)
                CFTL_REPEAT_LEFT(
                    CFTL_VARIANT_OVER_TYPES_LIMIT,
                    CFTL_VARIANT_DESTROY_MEM,
                    void
                )
                case TYPE_UNKNOWN: break;
            }
        }

    public:

        /// default construction, initialize to the first type in type
        /// parameter list.
        Variant(void) : type_tag(TYPE_T0) {
            Initializer<T0>::initialize(&storage);
        }

        /// copy construction, default initializes the value to the tagged
        /// type and then calls the copy assignment operator.
        Variant(const self_t &other) : type_tag(other.type_tag) {
            switch(type_tag) {
                CFTL_VARIANT_COPY_MEM(0, other.storage)
                CFTL_REPEAT_LEFT(
                    CFTL_VARIANT_OVER_TYPES_LIMIT,
                    CFTL_VARIANT_COPY_MEM,
                    other.storage
                )
                case TYPE_UNKNOWN: break;
            }
        }

        /// create basic methods such as copy constructors and assignment
        /// operators.
        CFTL_VARIANT_TYPE_METHOD(0, void)
        CFTL_REPEAT_LEFT(
            CFTL_VARIANT_OVER_TYPES_LIMIT,
            CFTL_VARIANT_TYPE_METHOD,
            void
        )

    private:

        /// class used for extracting a value of a particular type from a
        /// sum
        template <typename Q>
        class ExtractType {
        public:
            typedef typename Sequence<CFTL_VARIANT_TYPE_PARAM_LIST>:: \
            template Select<Q>::type_t type_t;
        };

        /// create a mapping between types
        template <typename Q, typename C, const unsigned id, const unsigned input>
        class ExtractId {
        public:
            enum { ID = input };
        };

        /// we made a type name match, but not a const-correct match
        template <typename Q, const unsigned id, const unsigned input>
        class ExtractId<Q, const Q, id, input> {
        public:
            enum { ID = input };
        };

        /// we made a type match, give out the id that we want
        template <typename Q, const unsigned id, const unsigned input>
        class ExtractId<Q, Q, id, input> {
        public:
            enum { ID = id };
        };

        template <typename Q, typename A>
        class StreamType {
        public:
            typedef const A type_t;
        };

        /// create default mappings for the unit types to pass through input
        /// and also default stream types for the unit classes
        CFTL_REPEAT_LEFT(
            CFTL_VARIANT_OVER_TYPES_LIMIT,
            CFTL_VARIANT_TYPE_SPEC_EXTRACT_ID,
            void
        )

        /// get a reference to a value in the sum
        template <typename Q>
        typename StreamType<Q, Q>::type_t &streamGet(void) const {
            return *reinterpret_cast<
                typename StreamType<Q, Q>::type_t *
            >(&storage);
        }

    public:

        /// check type containment
        template <typename Q>
        bool hasType(void) const {
            return type_tag == static_cast<tag_t>(CFTL_FOLD_LEFT(
                CFTL_VARIANT_OVER_TYPES_LIMIT,
                CFTL_VARIANT_TYPE_EXTRACT_ID,
                CFTL_PACK_0,
                CFTL_VARIANT_TYPE_EXTRACT_ID(0, void, (unsigned) TYPE_UNKNOWN)
            ));
        }

        /// get a reference to a value in the sum
        template <typename Q>
        Q &get(void) {
            assert(hasType<Q>());
            return *reinterpret_cast<
                typename ExtractType<Q>::type_t *
            >(&storage);
        }

        /// get a reference to a value in the sum
        template <typename Q>
        const Q &get(void) const {
            assert(hasType<Q>());
            return *reinterpret_cast<
                typename ExtractType<Q>::type_t *
            >(&storage);
        }
    };

    /// get a reference to a value in the sum. this performs runtime checking
    /// to make sure that the type can be extracted.
    template <typename Q, CFTL_VARIANT_TYPENAME_LIST >
    inline Q &get(Variant<CFTL_VARIANT_TYPE_PARAM_LIST> &sum) {
        return sum.template get<Q>();
    }

    /// get a const reference to a value in the sum
    template <typename Q, CFTL_VARIANT_TYPENAME_LIST >
    inline const Q &get(const Variant<CFTL_VARIANT_TYPE_PARAM_LIST> &sum) {
        return sum.template get<Q>();
    }

    /// streaming operator
    template <CFTL_VARIANT_TYPENAME_LIST >
    std::ostream &
    operator<<(std::ostream &os, const Variant<CFTL_VARIANT_TYPE_PARAM_LIST> &sum) {
        typedef Variant<CFTL_VARIANT_TYPE_PARAM_LIST> variant_t;
        os << '(' << static_cast<unsigned>(sum.type_tag) << ", ";
        switch(sum.type_tag) {
            CFTL_VARIANT_STREAM_SWITCH(0, os)
            CFTL_REPEAT_LEFT(
                CFTL_VARIANT_OVER_TYPES_LIMIT,
                CFTL_VARIANT_STREAM_SWITCH,
                os
            )
            case variant_t::TYPE_UNKNOWN: break;
        }
        return os << ')';
    }
}

#endif /* CFTL_STL_VARIANT_HPP_ */
