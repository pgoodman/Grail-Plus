/*
 * OperatorOverload.hpp
 *
 *  Created on: Mar 31, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef GrailPlus_OPERATOROVERLOAD_HPP_
#define GrailPlus_OPERATOROVERLOAD_HPP_

/// enable the user-defined operators feature; this will slow down
/// compilation time significantly
#ifndef FLTL_FEATURE_USER_DEFINED_OPERATORS
#define FLTL_FEATURE_USER_DEFINED_OPERATORS 0
#endif

#include "fltl/include/preprocessor/FORCE_INLINE.hpp"

#define FLTL_MAKE_USER_OP0(name) \
    template <typename AlphaT, typename OpTagT, typename OpScopeT> \
    class Op ## name { \
    public: \
        typedef void return_type; \
        FLTL_FORCE_INLINE static void run(const void *) throw() { } \
    }

#define FLTL_MAKE_USER_OP1(name) \
    template < \
        typename AlphaT, \
        typename OpTagT, \
        typename OpScopeT, \
        typename OpParamT=unusable_type \
    > \
    class Op ## name { \
    public: \
        typedef void return_type; \
        typedef const OpParamT param_type; \
        FLTL_FORCE_INLINE static void run( \
            const void *, \
            const OpParamT \
        ) throw() { } \
    }

#define FLTL_USER_OPERATOR0(trait,tag,name,func) \
    FLTL_USER_OPERATOR0_TYPE_SUFFIX(trait,tag,name,func,void,const throw())

#define FLTL_USER_OPERATOR0_MUT(trait,tag,name,func) \
    FLTL_USER_OPERATOR0_TYPE_SUFFIX(trait,tag,name,func,void,throw())

#define FLTL_USER_OPERATOR0_TYPE(trait,tag,name,func,tt) \
    FLTL_USER_OPERATOR0_TYPE_SUFFIX(trait,tag,name,func,tt,const throw())

#define FLTL_USER_OPERATOR0_MUT_TYPE(trait,tag,name,func,tt) \
    FLTL_USER_OPERATOR0_TYPE_SUFFIX(trait,tag,name,func,tt,throw())

#define FLTL_USER_OPERATOR0_TYPE_SUFFIX(traits,tag,name,func,tt,suffix) \
    FLTL_FORCE_INLINE typename fltl::mpl::Op ## name< \
        traits, \
        tag, \
        typename fltl::trait::Alphabet<traits>::scope_type \
    >::return_type \
    func (tt) suffix { \
        return fltl::mpl::Op ## name< \
            traits, \
            tag, \
            typename fltl::trait::Alphabet<traits>::scope_type \
        >::run(this); \
    }

#define FLTL_USER_OPERATOR1(trait,tag,name,func) \
    FLTL_USER_OPERATOR1_SUFFIX(trait,tag,name,func,const throw() )

#define FLTL_USER_OPERATOR1_MUT(trait,tag,name,func) \
    FLTL_USER_OPERATOR1_SUFFIX(trait,tag,name,func, throw() )

#define FLTL_USER_OPERATOR1_SUFFIX(traits,tag,name,func,suffix) \
    template <typename OpParamT> \
    FLTL_FORCE_INLINE typename fltl::mpl::Op ## name< \
        traits, \
        tag, \
        typename fltl::trait::Alphabet<traits>::scope_type, \
        OpParamT \
    >::return_type \
    func (OpParamT p) suffix { \
        return fltl::mpl::Op ## name< \
            traits, \
            tag, \
            typename fltl::trait::Alphabet<traits>::scope_type, \
            OpParamT \
        >::run(this,p); \
    }

#define FLTL_USER_BITWISE_OPERATORS_UNARY(traits,tag) \
    FLTL_USER_OPERATOR0_MUT(traits, tag, UnaryBitwiseNot, operator~)

#define FLTL_USER_BITWISE_OPERATORS_BINARY(traits,tag) \
    FLTL_USER_OPERATOR1(traits, tag, BinaryBitwiseAnd, operator&) \
    FLTL_USER_OPERATOR1(traits, tag, BinaryBitwiseOr, operator|) \
    FLTL_USER_OPERATOR1(traits, tag, BinaryBitwiseXor, operator^) \
    FLTL_USER_OPERATOR1_MUT(traits, tag, BinaryBitwiseAndAssign, operator&=) \
    FLTL_USER_OPERATOR1_MUT(traits, tag, BinaryBitwiseOrAssign, operator|=) \
    FLTL_USER_OPERATOR1_MUT(traits, tag, BinaryBitwiseXorAssign, operator^=) \
    FLTL_USER_OPERATOR1_MUT(traits, tag, BinaryShiftLeft, operator<<) \
    FLTL_USER_OPERATOR1_MUT(traits, tag, BinaryShiftRight, operator>>) \
    FLTL_USER_OPERATOR1_MUT(traits, tag, BinaryShiftLeftAssign, operator<<=) \
    FLTL_USER_OPERATOR1_MUT(traits, tag, BinaryShiftRightAssign, operator>>=)

#define FLTL_USER_BITWISE_OPERATORS(traits, tag) \
    FLTL_USER_BITWISE_OPERATORS_UNARY(traits, tag) \
    FLTL_USER_BITWISE_OPERATORS_BINARY(traits, tag)

#define FLTL_USER_LOGICAL_OPERATORS_UNARY(traits, tag) \
    FLTL_USER_OPERATOR0_MUT(traits, tag, UnaryLogicalNot, operator!)

#define FLTL_USER_LOGICAL_OPERATORS_BINARY(traits, tag) \
    FLTL_USER_OPERATOR1(traits, tag, BinaryLogicalAnd, operator&&) \
    FLTL_USER_OPERATOR1(traits, tag, BinaryLogicalOr, operator||)

#define FLTL_USER_LOGICAL_OPERATORS(traits, tag) \
    FLTL_USER_LOGICAL_OPERATORS_UNARY(traits, tag) \
    FLTL_USER_LOGICAL_OPERATORS_BINARY(traits, tag)

#define FLTL_USER_RELATION_OPERATORS(traits, tag) \
    FLTL_USER_OPERATOR1(traits, tag, BinaryEq, operator==) \
    FLTL_USER_OPERATOR1(traits, tag, BinaryNotEq, operator!=) \
    FLTL_USER_OPERATOR1(traits, tag, BinaryGt, operator>) \
    FLTL_USER_OPERATOR1(traits, tag, BinaryLt, operator<) \
    FLTL_USER_OPERATOR1(traits, tag, BinaryGtEq, operator>=) \
    FLTL_USER_OPERATOR1(traits, tag, BinaryLtEq, operator<=)

#define FLTL_USER_ARITHMETIC_OPERATORS_UNARY(traits,tag) \
    FLTL_USER_OPERATOR0_MUT(traits, tag, UnaryPlus, operator+) \
    FLTL_USER_OPERATOR0_MUT(traits, tag, UnaryMinus, operator-) \
    FLTL_USER_OPERATOR0_MUT(traits, tag, UnaryPreIncrement, operator++) \
    FLTL_USER_OPERATOR0_MUT(traits, tag, UnaryPreDecrement, operator--) \
    FLTL_USER_OPERATOR0_MUT_TYPE(traits, tag, UnaryPostIncrement, operator++, int) \
    FLTL_USER_OPERATOR0_MUT_TYPE(traits, tag, UnaryPostDecrement, operator--, int)

#define FLTL_USER_ARITHMETIC_OPERATORS_BINARY(traits, tag) \
    FLTL_USER_OPERATOR1(traits, tag, BinaryPlus, operator+) \
    FLTL_USER_OPERATOR1(traits, tag, BinaryMinus, operator-) \
    FLTL_USER_OPERATOR1(traits, tag, BinaryMult, operator*) \
    FLTL_USER_OPERATOR1(traits, tag, BinaryDiv, operator/) \
    FLTL_USER_OPERATOR1(traits, tag, BinaryMod, operator%)

#define FLTL_USER_ARITHMETIC_OPERATORS(traits, tag) \
    FLTL_USER_ARITHMETIC_OPERATORS_UNARY(traits, tag) \
    FLTL_USER_ARITHMETIC_OPERATORS_BINARY(traits, tag)

#define FLTL_USER_OPERATORS(traits, tag) \
    FLTL_USER_BITWISE_OPERATORS(traits, tag) \
    FLTL_USER_LOGICAL_OPERATORS(traits, tag) \
    FLTL_USER_RELATION_OPERATORS(traits, tag) \
    FLTL_USER_ARITHMETIC_OPERATORS(traits, tag)

namespace fltl { namespace mpl {

    namespace {
        class unusable_type {
        private:
            explicit unusable_type(void) throw() { }
            unusable_type &operator=(const unusable_type &) throw() {
                return *this;
            }
        public:
            unusable_type(const unusable_type &) throw() { }
        };
    }

    // tag for the "global" scope
    class global_scope_type { };

    FLTL_MAKE_USER_OP0(UnaryPlus);
    FLTL_MAKE_USER_OP0(UnaryMinus);
    FLTL_MAKE_USER_OP0(UnaryPreIncrement);
    FLTL_MAKE_USER_OP0(UnaryPostIncrement);
    FLTL_MAKE_USER_OP0(UnaryPreDecrement);
    FLTL_MAKE_USER_OP0(UnaryPostDecrement);
    FLTL_MAKE_USER_OP0(UnaryLogicalNot);
    FLTL_MAKE_USER_OP0(UnaryBitwiseNot);
    FLTL_MAKE_USER_OP0(UnaryIndirection);
    FLTL_MAKE_USER_OP0(UnaryReference);

    FLTL_MAKE_USER_OP0(BinaryIndirection); // a->b

    FLTL_MAKE_USER_OP1(BinaryPlus);
    FLTL_MAKE_USER_OP1(BinaryMinus);
    FLTL_MAKE_USER_OP1(BinaryMult);
    FLTL_MAKE_USER_OP1(BinaryDiv);
    FLTL_MAKE_USER_OP1(BinaryMod);

    FLTL_MAKE_USER_OP1(BinaryEq);
    FLTL_MAKE_USER_OP1(BinaryNotEq);
    FLTL_MAKE_USER_OP1(BinaryGt);
    FLTL_MAKE_USER_OP1(BinaryLt);
    FLTL_MAKE_USER_OP1(BinaryGtEq);
    FLTL_MAKE_USER_OP1(BinaryLtEq);
    FLTL_MAKE_USER_OP1(BinaryLogicalAnd); // a && b
    FLTL_MAKE_USER_OP1(BinaryLogicalOr); // a || b
    FLTL_MAKE_USER_OP1(BinaryBitwiseAnd); // a & b
    FLTL_MAKE_USER_OP1(BinaryBitwiseOr); // a | b
    FLTL_MAKE_USER_OP1(BinaryBitwiseXor); // a ^ b
    FLTL_MAKE_USER_OP1(BinaryShiftLeft); // a << b
    FLTL_MAKE_USER_OP1(BinaryShiftRight); // a >> b
    FLTL_MAKE_USER_OP1(BinaryShiftLeftAssign);// a <<= b
    FLTL_MAKE_USER_OP1(BinaryShiftRightAssign); // a >>= b
    FLTL_MAKE_USER_OP1(BinaryPlusAssign); // a += b
    FLTL_MAKE_USER_OP1(BinaryMinusAssign); // a -= b
    FLTL_MAKE_USER_OP1(BinaryMultAssign); // a *= b
    FLTL_MAKE_USER_OP1(BinaryDivAssign); // a /= b
    FLTL_MAKE_USER_OP1(BinaryModAssign); // a %= b
    FLTL_MAKE_USER_OP1(BinaryBitwiseAndAssign); // a &= b
    FLTL_MAKE_USER_OP1(BinaryBitwiseOrAssign); // a |= b
    FLTL_MAKE_USER_OP1(BinaryBitwiseXorAssign); // a ^= b

    FLTL_MAKE_USER_OP1(BinarySubscript); // a[b]
    FLTL_MAKE_USER_OP1(BinaryMemberByPointer); // a->*b
}}

#endif /* GrailPlus_OPERATOROVERLOAD_HPP_ */
