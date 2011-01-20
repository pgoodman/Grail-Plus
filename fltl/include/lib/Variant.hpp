/*
 * Variant.hpp
 *
 *  Created on: Sep 14, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef FLTL_STL_VARIANT_HPP_
#define FLTL_STL_VARIANT_HPP_

#include <cassert>
#include <cstddef>
#include <iostream>

#include "fltl/include/preprocessor/TEMPLATE_VARIABLE_LIMIT.hpp"
#include "fltl/include/preprocessor/CATENATE.hpp"
#include "fltl/include/preprocessor/ENUMERATE_PARAMS.hpp"
#include "fltl/include/preprocessor/EVAL.hpp"
#include "fltl/include/preprocessor/REPEAT_LEFT.hpp"

#include "fltl/include/mpl/Max.hpp"
#include "fltl/include/mpl/Sequence.hpp"
#include "fltl/include/mpl/SizeOf.hpp"
#include "fltl/include/mpl/Destroyer.hpp"
#include "fltl/include/mpl/Initializer.hpp"

#include "fltl/include/trait/Uncopyable.hpp"

/// an expansion of a single parameterized type with a default type of Unit
#define FLTL_VARIANT_ENUMERATED_TPL_PARAM(n, p) \
    , typename FLTL_CATENATE(p, n)=VariantUnit ## n

/// an expansion of a single value inside an enumerated type
#define FLTL_VARIANT_ENUMERATED_ENUM_PARAM(n, p) \
    , TYPE_ ## p ## n = n

/// macro for computing the size of a parameterized type by its index
#define FLTL_VARIANT_TYPE_SIZE(n, _) \
    , sizeof(T ## n)

/// define the anonymous sum unit types that are used to fill in the gaps
/// of sum type. these are anonymous so that the programmer cannot place
/// a gap in-between two parameterized concrete types.
#define FLTL_MAKE_VARIANT_UNIT(n, _) \
    class VariantUnit ## n : private trait::Uncopyable { \
    private: \
        VariantUnit ## n(void) { } \
    };

/// define the specializations of the unit types so that no work is done
/// to initialize or destroy the sum unit types.
#define FLTL_MAKE_VARIANT_UNIT_SPEC(n, _) \
    template <> \
    class Destroyer<VariantUnit ## n> { \
    public: \
        inline static void destroy(VariantUnit ## n *) { } \
        inline static void destroy(void *) { } \
    }; \
    template <> \
    class Initializer<VariantUnit ## n> { \
    public: \
        inline static void initialize(void *) { } \
    }; \
    template <> \
    class SizeOf<VariantUnit ## n> { \
    public: \
        enum { \
            VALUE = 0 \
        }; \
    };

/// create the copy constructors and assignment operators
#define FLTL_VARIANT_TYPE_METHOD(n, _) \
    Variant(const T ## n &val) : type_tag(TYPE_T ## n) { \
        mpl::Initializer<T ## n>::initialize(&storage); \
        *reinterpret_cast<T ## n *>(&storage) = val; \
    } \
    \
    self_t &operator=(const T ## n &val) { \
        if(TYPE_T ## n != type_tag) { \
            type_tag = TYPE_T ## n ; \
            destroy(); \
            mpl::Initializer<T ## n>::initialize(&storage); \
        } \
        *reinterpret_cast<T ## n *>(&storage) = val; \
        return *this; \
    }

#define FLTL_VARIANT_TYPE_PARAM_LIST \
    T0 FLTL_ENUMERATE_PARAMS(FLTL_TEMPLATE_VARIABLE_LIMIT, T)

#define FLTL_VARIANT_TYPENAME_LIST \
    typename T0 FLTL_ENUMERATE_PARAMS( \
        FLTL_TEMPLATE_VARIABLE_LIMIT, \
        typename T \
    )

/// used to build the switch statement so as to be able to properly destroy
/// the held value
#define FLTL_VARIANT_DESTROY_MEM(n, _) \
    case TYPE_T ## n : mpl::Destroyer<T ## n>::destroy(&storage); break;

/// used to build the switch statement to properly initialize and copy the
/// memory.
#define FLTL_VARIANT_COPY_MEM(n, p) \
    case TYPE_T ## n : \
        mpl::Initializer<T ## n>::initialize(&storage); \
        *reinterpret_cast<T ## n *>(&storage) = p; \
        break;

#define FLTL_VARIANT_TYPE_EXTRACT_ID(n, _, rest) \
    (ExtractId<Q, T ## n, (unsigned) TYPE_T ## n, (rest)>::ID)

/// specialization of extract id class for the unit types. this passes
/// through the input so that the unit types are seen as type mismatches
#define FLTL_VARIANT_TYPE_SPEC_EXTRACT_ID(n, _) \
    template <typename Q, const unsigned id, const unsigned input> \
    class ExtractId<Q, VariantUnit ## n, id, input> { \
    public: \
        enum { ID = input }; \
    };\
    \
    template <typename Q> \
    class StreamType<Q, VariantUnit ## n> { \
    public: \
        typedef const char type; \
    };

#define FLTL_VARIANT_STREAM_SWITCH(n, ss) \
    case variant_t::TYPE_T ## n : ss << sum.template streamGet<T ## n>(); break;

namespace fltl {

    namespace {

        /// type used for calculating how much extra space is needed to add
        /// on to the end of the storage so as to be able to hold a value of
        /// any of the types in the Variant

        template <typename T, const size_t k>
        class VariantPadding {
        public:
            enum { VALUE = sizeof(T) - k };
        };

        template <typename T>
        class VariantPadding<T, 0UL> {
        public:
            enum { VALUE = 0 };
        };

        /// make the unit classes. these classes are meant to be distinct.
        /// also, they are in the anonymous namespace and so they cannot be
        /// used by someone outside of this file, i.e. no one can ever
        /// make a sum that explicity uses one of the VariantUnit classes.
        FLTL_REPEAT_LEFT(
            FLTL_TEMPLATE_VARIABLE_LIMIT,
            FLTL_MAKE_VARIANT_UNIT,
            void
        )
    }

    namespace mpl {

        /// create the specializations of the sum types for Initializer,
        /// Destroyer, and SizeOf.
        FLTL_REPEAT_LEFT(
            FLTL_TEMPLATE_VARIABLE_LIMIT,
            FLTL_MAKE_VARIANT_UNIT_SPEC,
            void
        )
    }
}

namespace fltl { namespace lib {

    /// forward declaration of sum type
    template <typename T0 \
              FLTL_REPEAT_LEFT(
                  FLTL_TEMPLATE_VARIABLE_LIMIT,
                  FLTL_VARIANT_ENUMERATED_TPL_PARAM,
                  T
              )>
    class Variant;

    /// forward declaration of stream function
    template <FLTL_VARIANT_TYPENAME_LIST >
    std::ostream &
    operator<<(std::ostream &, const Variant<FLTL_VARIANT_TYPE_PARAM_LIST> &);

    /// Definition for a tagged variant/union/sum type.
    ///
    /// Note: - the sum type expects all non-Unit types to be
    ///         distinct.
    ///       - types in the sum are expected to have default initializers.
    ///       - types in the sum are expected to have copy constructors.
    template < FLTL_VARIANT_TYPENAME_LIST >
    class Variant {
    public:

        /// the type of this sum
        typedef Variant< FLTL_VARIANT_TYPE_PARAM_LIST > self_t;

    private:

        /// streaming operator
        friend std::ostream &operator<< <FLTL_VARIANT_TYPE_PARAM_LIST>(
            std::ostream &,
            const self_t &
        );

        /// enumeration type defining valid accessors
        typedef enum {
            TYPE_T0 = 0
            FLTL_REPEAT_LEFT(
                FLTL_TEMPLATE_VARIABLE_LIMIT,
                FLTL_VARIANT_ENUMERATED_ENUM_PARAM,
                T
            ),
            TYPE_UNKNOWN
        } tag_t;

        typedef unsigned storage_t;

        enum {

            STORAGE_SIZE = sizeof(storage_t),

            /// the maximum size of all object sizes in this sum type
            MAX_OBJECT_SIZE = mpl::Max<
                sizeof(storage_t),
                sizeof(T0)
                FLTL_REPEAT_LEFT(
                    FLTL_TEMPLATE_VARIABLE_LIMIT,
                    FLTL_VARIANT_TYPE_SIZE,
                    STORAGE_SIZE
                )
            >::VALUE,

            /// calculate the how many values of storage_t are needed to
            /// store the largest object of the sum
            MOS_DIV = MAX_OBJECT_SIZE / STORAGE_SIZE,
            MOS_MOD = MAX_OBJECT_SIZE % STORAGE_SIZE,

            PADDING = VariantPadding<storage_t, MOS_MOD>::VALUE,
            MEMORY_SIZE = MOS_DIV + PADDING
        };

        /// type tag
        tag_t type_tag;

        /// memory in which a value of any of the types T_i will be stored
        storage_t storage[MEMORY_SIZE];

        /// call the proper destructor of whatever type is being pointed to.
        void destroy(void) {
            switch(type_tag) {
                FLTL_VARIANT_DESTROY_MEM(0, void)
                FLTL_REPEAT_LEFT(
                    FLTL_TEMPLATE_VARIABLE_LIMIT,
                    FLTL_VARIANT_DESTROY_MEM,
                    void
                )
                case TYPE_UNKNOWN: break;
            }
        }

    public:

        /// default construction, initialize to the first type in type
        /// parameter list.
        Variant(void) : type_tag(TYPE_T0) {
            mpl::Initializer<T0>::initialize(&storage);
        }

        /// copy construction, default initializes the value to the tagged
        /// type and then calls the copy assignment operator.
        Variant(const self_t &other) : type_tag(other.type_tag) {
            switch(type_tag) {
                FLTL_VARIANT_COPY_MEM(0, other.storage)
                FLTL_REPEAT_LEFT(
                    FLTL_TEMPLATE_VARIABLE_LIMIT,
                    FLTL_VARIANT_COPY_MEM,
                    other.storage
                )
                case TYPE_UNKNOWN: break;
            }
        }

        /// create basic methods such as copy constructors and assignment
        /// operators.
        FLTL_VARIANT_TYPE_METHOD(0, void)
        FLTL_REPEAT_LEFT(
            FLTL_TEMPLATE_VARIABLE_LIMIT,
            FLTL_VARIANT_TYPE_METHOD,
            void
        )

        /// destructor, destroy the proper object.
        ~Variant(void) {
            destroy();
        }

    private:

        /// class used for extracting a value of a particular type from a
        /// sum
        template <typename Q>
        class ExtractType {
        public:
            typedef mpl::Sequence<FLTL_VARIANT_TYPE_PARAM_LIST> sequence_t;
            typedef typename sequence_t::template Select<Q>::type type;
        };

        /// create a mapping between types
        template <typename Q, typename C,
                  const unsigned id, const unsigned input>
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
            typedef const A type;
        };

        /// create default mappings for the unit types to pass through input
        /// and also default stream types for the unit classes
        FLTL_REPEAT_LEFT(
            FLTL_TEMPLATE_VARIABLE_LIMIT,
            FLTL_VARIANT_TYPE_SPEC_EXTRACT_ID,
            void
        )

        /// get a reference to a value in the sum
        template <typename Q>
        typename StreamType<Q, Q>::type &streamGet(void) const throw() {
            return *reinterpret_cast<
                typename StreamType<Q, Q>::type *
            >(&storage);
        }

    public:

        /// check type containment
        template <typename Q>
        bool hasType(void) const throw() {
            return type_tag == static_cast<tag_t>(FLTL_FOLD_LEFT(
                FLTL_TEMPLATE_VARIABLE_LIMIT,
                FLTL_VARIANT_TYPE_EXTRACT_ID,
                FLTL_PACK_0,
                FLTL_VARIANT_TYPE_EXTRACT_ID(
                    0,
                    void,
                    (unsigned) TYPE_UNKNOWN
                )
            ));
        }

        /// get a reference to a value in the sum
        template <typename Q>
        Q &get(void) throw() {
            assert(hasType<Q>());
            return *reinterpret_cast<
                typename ExtractType<Q>::type *
            >(&storage);
        }

        /// get a reference to a value in the sum
        template <typename Q>
        const Q &get(void) const throw() {
            assert(hasType<Q>());
            return *reinterpret_cast<
                typename ExtractType<Q>::type *
            >(&storage);
        }
    };

    /// get a reference to a value in the sum. this performs runtime checking
    /// to make sure that the type can be extracted.
    template <typename Q, FLTL_VARIANT_TYPENAME_LIST >
    inline Q &get(Variant<FLTL_VARIANT_TYPE_PARAM_LIST> &sum) throw() {
        return sum.template get<Q>();
    }

    /// get a const reference to a value in the sum
    template <typename Q, FLTL_VARIANT_TYPENAME_LIST >
    inline const Q &get(const Variant<FLTL_VARIANT_TYPE_PARAM_LIST> &sum)
    throw() {
        return sum.template get<Q>();
    }

    /// streaming operator
    template <FLTL_VARIANT_TYPENAME_LIST >
    std::ostream &
    operator<<(std::ostream &os,
               const Variant<FLTL_VARIANT_TYPE_PARAM_LIST> &sum
              ) {
        typedef Variant<FLTL_VARIANT_TYPE_PARAM_LIST> variant_t;
        os << '(' << static_cast<unsigned>(sum.type_tag) << ", ";
        switch(sum.type_tag) {
            FLTL_VARIANT_STREAM_SWITCH(0, os)
            FLTL_REPEAT_LEFT(
                FLTL_TEMPLATE_VARIABLE_LIMIT,
                FLTL_VARIANT_STREAM_SWITCH,
                os
            )
            case variant_t::TYPE_UNKNOWN: break;
        }
        return os << ')';
    }
}}

#endif /* FLTL_STL_VARIANT_HPP_ */
