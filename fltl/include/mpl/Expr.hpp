/*
 * Expr.hpp
 *
 *  Created on: Sep 19, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef FLTL_MPL_EXPR_HPP_
#define FLTL_MPL_EXPR_HPP_

#include "fltl/include/trait/Uncopyable.hpp"

#include "fltl/include/mpl/Unit.hpp"

#define FLTL_BUILD_EXPR_TYPE(name) \
    class name { \
    public: \
        static const char *PRINT_NAME;\
    };\
    const char *name::PRINT_NAME(#name)

#define FLTL_BUILD_EXPR_TYPE_FUNC_1(c, f) \
    Expr<c, Expr<TagT, T0, T1> > f(void) const { \
        return Static<Expr<c, Expr<TagT, T0, T1> > >::VALUE; \
    }

#define FLTL_BUILD_EXPR_TYPE_FUNC_2(c, f) \
    template <typename R> \
    Expr< \
        c, \
        Expr<TagT, T0, T1>, \
        Expr<typename R::tag_t, typename R::first_t, typename R::second_t> \
    > f(R) const { \
        return Static< \
            Expr< \
                c, \
                Expr<TagT, T0, T1>, \
                Expr<typename R::tag_t, typename R::first_t, typename R::second_t> \
            > \
        >::VALUE; \
    }

namespace fltl { namespace mpl {

    namespace {

        /// produces a value of a given type
        template <typename T>
        class Static {
        public:
            static const T VALUE;
        };
    }

    /// expression template classes for common operators
    namespace expr {

        template <typename TagT, typename T0 = mpl::Unit, typename T1 = mpl::Unit>
        class Expr;

        FLTL_BUILD_EXPR_TYPE(Equal);
        FLTL_BUILD_EXPR_TYPE(NotEqual);
        FLTL_BUILD_EXPR_TYPE(LogicalAnd);
        FLTL_BUILD_EXPR_TYPE(LogicalOr);
        FLTL_BUILD_EXPR_TYPE(BitwiseAnd);
        FLTL_BUILD_EXPR_TYPE(BitwiseOr);

        FLTL_BUILD_EXPR_TYPE(LogicalNot);
        FLTL_BUILD_EXPR_TYPE(BitwiseNot);

        FLTL_BUILD_EXPR_TYPE(Ref);
        FLTL_BUILD_EXPR_TYPE(DerefByPtr);

        FLTL_BUILD_EXPR_TYPE(ShiftLeft);
        FLTL_BUILD_EXPR_TYPE(ShiftRight);

        FLTL_BUILD_EXPR_TYPE(Plus);
        FLTL_BUILD_EXPR_TYPE(Minus);
        FLTL_BUILD_EXPR_TYPE(Multiply);
        FLTL_BUILD_EXPR_TYPE(Divide);

        /// Base expression template
        template <typename TagT, typename T0, typename T1>
        class Expr {
        public:

            typedef TagT tag_t;
            typedef T0 first_t;
            typedef T1 second_t;

            typedef const Expr<TagT, T0, T1> self_t;

            FLTL_BUILD_EXPR_TYPE_FUNC_2(Equal, operator==)
            FLTL_BUILD_EXPR_TYPE_FUNC_2(NotEqual, operator!=)

            FLTL_BUILD_EXPR_TYPE_FUNC_2(LogicalAnd, operator&&)
            FLTL_BUILD_EXPR_TYPE_FUNC_2(LogicalOr, operator||)

            FLTL_BUILD_EXPR_TYPE_FUNC_2(BitwiseAnd, operator&)
            FLTL_BUILD_EXPR_TYPE_FUNC_2(BitwiseOr, operator|)

            FLTL_BUILD_EXPR_TYPE_FUNC_1(LogicalNot, operator!)
            FLTL_BUILD_EXPR_TYPE_FUNC_1(BitwiseNot, operator~)

            FLTL_BUILD_EXPR_TYPE_FUNC_1(Ref, operator&)
            FLTL_BUILD_EXPR_TYPE_FUNC_2(DerefByPtr, operator->*)

            FLTL_BUILD_EXPR_TYPE_FUNC_2(ShiftLeft, operator<<)
            FLTL_BUILD_EXPR_TYPE_FUNC_2(ShiftRight, operator>>)

            FLTL_BUILD_EXPR_TYPE_FUNC_2(Plus, operator+)
            FLTL_BUILD_EXPR_TYPE_FUNC_2(Minus, operator-)
            FLTL_BUILD_EXPR_TYPE_FUNC_2(Multiply, operator*)
            FLTL_BUILD_EXPR_TYPE_FUNC_2(Divide, operator/)
        };

        template <const int n>
        class Integer {
        public:
            typedef const Integer<n> self_t;

            enum {
                VALUE = n
            };
        };

        template <typename ExprT>
        class Printer {
        public:
            inline static void print(void) {
                std::cout << '(' <<  ExprT::tag_t::PRINT_NAME << " ";
                Printer<typename ExprT::first_t>::print();
                std::cout << ' ';
                Printer<typename ExprT::second_t>::print();
                std::cout << ')';
            }
        };

        template <>
        class Printer<mpl::Unit> {
        public:
            inline static void print(void) {
                std::cout << "()";
            }
        };

        template <const int n>
        class Printer<Integer<n> > {
        public:
            inline static void print(void) {
                std::cout << "(int " << n << ")";
            }
        };

        /// used to express the syntax of valid expression languages
        template <typename ContextT=mpl::Unit>
        class Structure {

        };
    }
}}

#endif /* FLTL_MPL_EXPR_HPP_ */
