/*
 * TDOP.hpp
 *
 *  Created on: May 10, 2012
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef Grail_Plus_TDOP_HPP_
#define Grail_Plus_TDOP_HPP_

#include <cassert>
#include <cstring>
#include <stdint.h>
#include <new>

#include "fltl/include/trait/Alphabet.hpp"
#include "fltl/include/trait/Uncopyable.hpp"

#include "fltl/include/helper/BlockAllocator.hpp"
#include "fltl/include/helper/StorageChain.hpp"
#include "fltl/include/helper/UnsafeCast.hpp"

#define FLTL_TDOP_USE_TYPES_PREFIX_FUNC(type, prefix, func) \
    typedef typename type::alphabet_type func(prefix, alphabet_type); \
    typedef typename type::traits_type func(prefix, traits_type); \
    typedef typename type::symbol_type func(prefix, symbol_type); \
    typedef typename type::term_type func(prefix, term_type); \
    typedef typename type::category_type func(prefix, category_type); \
    typedef typename type::rule_type func(prefix, rule_type); \
    typedef typename type::operator_type func(prefix, operator_type); \
    typedef typename type::operator_string_type func(prefix, operator_string_type); \
    typedef type func(prefix, tdop_type)

#define FLTL_TDOP_NO_PREFIX(prefix, str) str
#define FLTL_TDOP_USE_PREFIX(prefix, str) prefix ## _ ## str

#define FLTL_TDOP_USE_TYPES(type) \
    FLTL_TDOP_USE_TYPES_PREFIX_FUNC(type, foo, FLTL_TDOP_NO_PREFIX)

#define FLTL_TDOP_USE_TYPES_PREFIX(type, prefix) \
    FLTL_TDOP_USE_TYPES_PREFIX_FUNC(type, prefix, FLTL_TDOP_USE_PREFIX)

namespace fltl {

    // forward declaration
    template <typename> class TDOP;

    namespace tdop {

        // forward eclaractions
        template <typename> class Category;
        template <typename> class OpaqueCategory;
        template <typename> class Symbol;
        template <typename> class Rule;
        template <typename> class OpaqueRule;
        template <typename> class Term;

        template <typename> class Operator;
        template <typename> class OperatorString;

        template <typename> class Unbound;
        template <typename> class Any;
        template <typename> class AnyOperatorString;
        template <typename> class AnyOperatorStringOfLength;
        template <typename> class Pattern;
    }
}

#include "fltl/include/tdop/Category.hpp"
#include "fltl/include/tdop/Rule.hpp"
#include "fltl/include/tdop/Symbol.hpp"

#include "fltl/include/tdop/OpaqueCategory.hpp"
#include "fltl/include/tdop/OpaqueRule.hpp"
#include "fltl/include/tdop/Term.hpp"
#include "fltl/include/tdop/Operator.hpp"
#include "fltl/include/tdop/OperatorString.hpp"

#include "fltl/include/tdop/Unbound.hpp"
#include "fltl/include/tdop/Pattern.hpp"

namespace fltl {

    /// represents a TDOP "grammar" of parser categories and their rules.
    template <typename AlphaT>
    class TDOP : protected trait::Uncopyable {
    public:

        /// extract the traits
        typedef trait::Alphabet<AlphaT> traits_type;
        typedef typename traits_type::alphabet_type alphabet_type;

        typedef tdop::OpaqueCategory<AlphaT> category_type;
        typedef tdop::OpaqueRule<AlphaT> rule_type;
        typedef tdop::Symbol<AlphaT> symbol_type;
        typedef tdop::Term<AlphaT> term_type;
        typedef tdop::Operator<AlphaT> operator_type;
        typedef tdop::OperatorString<AlphaT> operator_string_type;

    private:

        typedef tdop::Category<AlphaT> internal_category_type;
        typedef tdop::Rule<AlphaT> internal_rule_type;

        typedef TDOP<AlphaT> self_type;

    public:

        TDOP(void) throw();
        ~TDOP(void) throw();

        /// set the initial category
        void set_initial_category(const category_type) throw();

        const category_type add_category(const char *name) throw();
        void remove_category(category_type cat) throw();
        void unsafe_remove_category(category_type cat) throw();

        const rule_type add_initial_rule(category_type cat, operator_string_type str) throw();
        const rule_type add_extension_rule(category_type cat, unsigned upper_bound, operator_string_type str) throw();

        const symbol_type get_symbol(const alphabet_type sym) throw();
        const symbol_type add_variable_symbol(void) throw();

        const unsigned num_categories(void) const throw();
        const unsigned num_rules(category_type cat) const throw();
        const unsigned num_initial_rules(category_type cat) const throw();
        const unsigned num_extension_rules(category_type cat) const throw();
    };

}

#endif /* Grail_Plus_TDOP_HPP_ */
