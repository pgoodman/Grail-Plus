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
#include <cstdlib>
#include <stdint.h>
#include <new>
#include <map>

#include "fltl/include/trait/Alphabet.hpp"
#include "fltl/include/trait/Uncopyable.hpp"

#include "fltl/include/helper/BlockAllocator.hpp"
#include "fltl/include/helper/StorageChain.hpp"
#include "fltl/include/helper/UnsafeCast.hpp"
#include "fltl/include/helper/Array.hpp"

#include "fltl/include/mpl/If.hpp"

#include "fltl/include/preprocessor/FORCE_INLINE.hpp"
#include "fltl/include/preprocessor/STATIC_ASSERT.hpp"

#define FLTL_TDOP_USE_TYPES_PREFIX_FUNC(type, prefix, func) \
    typedef typename type::alphabet_type func(prefix, alphabet_type); \
    typedef typename type::traits_type func(prefix, traits_type); \
    typedef typename type::symbol_type func(prefix, symbol_type); \
    typedef typename type::term_type func(prefix, term_type); \
    typedef typename type::category_type func(prefix, category_type); \
    typedef typename type::rule_type func(prefix, rule_type); \
    typedef typename type::operator_type func(prefix, operator_type); \
    typedef typename type::operator_string_type func(prefix, operator_string_type); \
    typedef typename type::pattern_type func(prefix, pattern_type); \
    typedef typename type::generator_type func(prefix, generator_type); \
    typedef type func(prefix, tdop_type)

#define FLTL_TDOP_NO_PREFIX(prefix, str) str
#define FLTL_TDOP_USE_PREFIX(prefix, str) prefix ## _ ## str

#define FLTL_TDOP_USE_TYPES(type) \
    FLTL_TDOP_USE_TYPES_PREFIX_FUNC(type, foo, FLTL_TDOP_NO_PREFIX)

#define FLTL_TDOP_USE_TYPES_PREFIX(type, prefix) \
    FLTL_TDOP_USE_TYPES_PREFIX_FUNC(type, prefix, FLTL_TDOP_USE_PREFIX)

#define FLTL_TDOP_RULE_PATTERN(tag) \
    FLTL_FORCE_INLINE tdop::detail::PatternBuilder<AlphaT,tag,void,true> \
    operator--(int) const throw() { \
        return tdop::detail::PatternBuilder<AlphaT,tag,void,true>( \
            tdop::detail::PatternData<AlphaT>::allocate( \
                const_cast<self_type *>(this)\
            ) \
        ); \
    }

namespace fltl {

    // forward declaration
    template <typename> class TDOP;

    namespace tdop {

        class rule_tag { };

        class category_tag { };
        class unbound_category_tag { };

        // (un)bound category, unbound lower bound
        class category_lb_tag { };
        class unbound_category_lb_tag { };

        class symbol_tag { };
        class unbound_symbol_tag { };
        class ubound_symbol_predicate_tag { };

        class term_tag { };
        class unbound_term_tag { };

        class operator_tag { };
        class unbound_operator_tag { };

        class operator_string_tag { };
        class unbound_operator_string_tag { };

        class any_operator_tag { };
        class any_operator_string_tag { };
        class any_operator_string_of_length_tag { };

        namespace detail {
            template <typename, typename, typename, const bool>
            class PatternBuilder;

            template <typename> class PatternData;

            template <typename> class CategoryGenerator;
            template <typename> class SymbolGenerator;
            template <typename> class RuleGenerator;
            template <typename,const bool> class PatternGenerator;
        }

        // forward eclaractions
        template <typename> class Category;
        template <typename> class OpaqueCategory;
        template <typename> class Symbol;
        template <typename> class Rule;
        template <typename> class OpaqueRule;
        template <typename> class Term;

        template <typename> class Operator;
        template <typename> class OperatorString;

        template <typename, typename> class Unbound;
        template <typename, typename> class Bound;

        template <typename> class AnyOperator;
        template <typename> class AnyOperatorString;
        template <typename> class AnyOperatorStringOfLength;

        template <typename> class Pattern;
        template <typename> class OpaquePattern;

        template <typename> class Generator;
    }
}

#include "fltl/include/tdop/Any.hpp"

#include "fltl/include/tdop/Category.hpp"

#include "fltl/include/tdop/Term.hpp"
#include "fltl/include/tdop/Symbol.hpp"
#include "fltl/include/tdop/OpaqueCategory.hpp"
#include "fltl/include/tdop/OpaqueRule.hpp"

#include "fltl/include/tdop/Rule.hpp"

#include "fltl/include/tdop/Operator.hpp"
#include "fltl/include/tdop/OperatorString.hpp"

#include "fltl/include/tdop/Unbound.hpp"
#include "fltl/include/tdop/Pattern.hpp"

#include "fltl/include/tdop/OpaquePattern.hpp"
#include "fltl/include/tdop/Generator.hpp"

namespace fltl {

    namespace detail {
        template <typename K, typename V>
        class AlphaMap : public std::map<
            K,
            V,
            typename trait::Alphabet<K>::less_type
        > { };
    }

    /// represents a TDOP "grammar" of parser categories and their rules.
    template <typename AlphaT>
    class TDOP : protected trait::Uncopyable {
    public:

        friend class tdop::detail::CategoryGenerator<AlphaT>;
        friend class tdop::detail::SymbolGenerator<AlphaT>;
        friend class tdop::detail::RuleGenerator<AlphaT>;
        friend class tdop::detail::PatternGenerator<AlphaT, true>;
        friend class tdop::detail::PatternGenerator<AlphaT, false>;

        /// extract the traits
        typedef trait::Alphabet<AlphaT> traits_type;
        typedef typename traits_type::alphabet_type alphabet_type;

        /// exposed types
        typedef tdop::OpaqueCategory<AlphaT> category_type;
        typedef tdop::OpaqueRule<AlphaT> rule_type;
        typedef tdop::Symbol<AlphaT> symbol_type;
        typedef tdop::Term<AlphaT> term_type;
        typedef tdop::Operator<AlphaT> operator_type;
        typedef tdop::OperatorString<AlphaT> operator_string_type;
        typedef tdop::OpaquePattern<AlphaT> pattern_type;
        typedef tdop::Generator<AlphaT> generator_type;

    private:

        /// internal types
        typedef tdop::Category<AlphaT> internal_category_type;
        typedef tdop::Rule<AlphaT> internal_rule_type;
        typedef TDOP<AlphaT> self_type;

        /// related to categories
        internal_category_type *unused_categories;
        uint32_t next_category_id;
        helper::Array<internal_category_type *> category_map;
        mutable detail::AlphaMap<const char *, category_type> named_category_map;
        internal_category_type *start_category;
        unsigned num_categories_;

        /// represents a symbol representative
        struct symbol_rep_type {
        public:
            union {
                alphabet_type as_symbol;
                const char *as_var;
            } u;

            enum {
                FREE,
                SYMBOL,
                VARIABLE_SYMBOL
            } kind;

            symbol_rep_type(void)
                : kind(FREE)
            {
                memset(&u, 0, sizeof u);
            }

            symbol_rep_type(alphabet_type a, tdop::symbol_tag) throw()
                : kind(SYMBOL)
            {
                memset(&u, 0, sizeof u);
                u.as_symbol = a;
            }

            symbol_rep_type(const char *a, tdop::unbound_symbol_tag) throw()
                : kind(VARIABLE_SYMBOL)
            {
                memset(&u, 0, sizeof u);
                u.as_var = a;
            }

            symbol_rep_type &operator=(const symbol_rep_type &that) throw() {
                if(SYMBOL == that.kind) {
                    u.as_symbol = that.u.as_symbol;
                } else if(VARIABLE_SYMBOL == that.kind) {
                    u.as_var = that.u.as_var;
                }
                kind = that.kind;
                return *this;
            }

            void clear(void) throw() {
                memset(&u, 0, sizeof u);
                kind = FREE;
            }

            ~symbol_rep_type(void) throw() {
                if(SYMBOL == kind) {
                    traits_type::destroy(u.as_symbol);
                } else if(VARIABLE_SYMBOL == kind) {
                    trait::Alphabet<const char *>::destroy(u.as_var);
                }
                memset(&u, 0, sizeof u);
                kind = FREE;
            }
        };

        /// related to to symbols
        mutable helper::Array<symbol_rep_type> symbol_map;
        detail::AlphaMap<AlphaT, symbol_type> symbol_map_inv;
        unsigned num_symbols_;

        /// related to rules
        unsigned num_rules_;

        /// related to all symbols, categories, and variable symbols
        mutable const char *auto_symbol_upper_bound;

        /// allocators
        static helper::BlockAllocator<internal_rule_type> rule_allocator;

    public:

        /// used for pattern matching
        tdop::AnyOperator<AlphaT> _;
        tdop::AnyOperatorString<AlphaT> __;

        /// constructors, destructors
        TDOP(void) throw()
            : trait::Uncopyable()

            // related to categories
            , unused_categories(0)
            , next_category_id(1U)
            , category_map(256U)
            , named_category_map()
            , start_category(0)
            , num_categories_(0U)

            // related to symbols
            , symbol_map(256U)
            , symbol_map_inv()
            , num_symbols_(0U)

            // related to rules
            , num_rules_(0U)

            // related to all symbols
            , auto_symbol_upper_bound("$0")

            // related to patterns
            , _()
            , __()
        {
            symbol_rep_type dummy_sym;

            // categories/symbols start at 1, so the 0th spot must be NULL
            category_map.append(0);
            symbol_map.append(dummy_sym);
        }

        /// destructor
        ~TDOP(void) throw() {

            // clear out all categories
            for(uint32_t i(0U); i < next_category_id; ++i) {
                internal_category_type *&cat(category_map[i]);
                if(0 != cat) {
                    internal_category_type::decref(cat);
                    cat = 0;
                }
            }

            auto_symbol_upper_bound = 0;
        }

        /// set the initial category. this is the cateogry that the parser
        /// begins parsing with. the default initial parsing lower bound is
        /// always zero.
        void set_initial_category(const category_type cat) throw() {
            start_category = find_category(cat);
        }

        /// get the initial category
        category_type get_initial_category(void) const throw() {
            if(0 == start_category) {
                category_type cat;
                return cat;
            } else {
                category_type cat(start_category);
                return cat;
            }
        }

        /// get a category's name
        const char *get_name(category_type cat_) const throw() {
            internal_category_type *cat(find_category(cat_));

            assert(0 != cat);

            if(0 == cat->name) {
                cat->name = next_symbol_upper_bound();
                named_category_map[cat->name] = cat;
            }

            return cat->name;
        }

        /// get a parser category by name. If no such parser category exists
        /// then a category is created.
        const category_type get_category(const char *name) throw() {
            assert(is_valid_symbol_name(name));

            typename detail::AlphaMap<const char *, category_type>::iterator loc(
                named_category_map.find(name)
            );

            // no category with this name
            if(0 == name
            || named_category_map.end() == loc) {

                category_type cat_extern(add_category());

                const char *name_copy(trait::Alphabet<
                    const char *
                >::copy(name));

                internal_category_type *cat_intern(find_category(cat_extern));

                cat_intern->name = name_copy;
                named_category_map[name_copy] = cat_extern;

                // check if it's an upper bound
                if('$' == *name) {
                    if(0 == auto_symbol_upper_bound
                    || 0 < strcmp(name, auto_symbol_upper_bound)) {
                        auto_symbol_upper_bound = name_copy;
                    }
                }

                return cat_extern;

            // this category already exists
            } else {
                return (*loc).second;
            }
        }

        /// add a new parser category to the TDOP machine.
        const category_type add_category(void) throw() {
            internal_category_type *cat(unused_categories);
            uint32_t cat_id(0);

            // need to allocate new one
            if(0 == cat) {
                cat = internal_category_type::allocate();
                cat_id = next_category_id++;
                category_map.append(cat);

            // free list is non-empty
            } else {
                unused_categories = cat->next;
                cat_id = cat->number;
                category_map.set(cat_id, cat);
            }

            internal_category_type *prev(find_category(cat_id, -1));
            internal_category_type *next(find_category(cat_id, 1));

            cat->number = cat_id;
            cat->prev = prev;
            cat->next = next;
            cat->num_extension_rules = 0;
            cat->num_initial_rules = 0;
            cat->first_initial_rule = 0;
            cat->first_extension_rule = 0;
            cat->name = 0;

            if(0 != prev) {
                prev->next = cat;
            }

            if(0 != next) {
                next->prev = cat;
            }

            if(0 == start_category) {
                start_category = cat;
            }

            ++num_categories_;

            category_type cat_(cat);
            return cat_;
        }

        void remove_category(category_type cat) throw(); // TODO
        void unsafe_remove_category(category_type cat) throw(); // TODO

        /// add an initial rule into a TDOP category
        const rule_type add_initial_rule(
            category_type cat_,
            operator_string_type str
        ) throw() {
            internal_category_type *cat(find_category(cat_));

            assert(0 != cat);
            assert(0 != str.length());

            internal_rule_type rule_;
            rule_.category = cat;
            rule_.upper_bound = internal_rule_type::INITIAL_RULE_UPPER_BOUND;
            rule_.str = str;
            rule_type ret(add_rule(
                &rule_,
                &(cat->first_initial_rule),
                &(cat->num_initial_rules)
            ));
            return ret;
        }

        /// add an extension rule into a TDOP category
        const rule_type add_extension_rule(
            category_type cat_,
            unsigned upper_bound,
            operator_string_type str
        ) throw() {
            internal_category_type *cat(find_category(cat_));

            assert(0 != cat);
            assert(0 != str.length());

            internal_rule_type rule_;
            rule_.category = cat;
            rule_.upper_bound = static_cast<int32_t>(upper_bound);
            rule_.str = str;
            rule_type ret(add_rule(
                &rule_,
                &(cat->first_extension_rule),
                &(cat->num_extension_rules)
            ));
            return ret;
        }

        /// remove a rule
        void remove_rule(rule_type &rule) const throw() {
            if(0 != rule.rule) {
                rule.rule->is_deleted = true;
            }

            internal_rule_type::decref(rule.rule);
        }

        /// get the alphabetic representation of a symbol
        const alphabet_type get_alpha(symbol_type sym) const throw() {
            const symbol_rep_type &rep(symbol_map.get(sym.number()));
            assert(symbol_rep_type::SYMBOL == rep.kind);
            return rep.u.as_symbol;
        }

        /// get the representation of a symbol, given its alphabetic
        /// representation, or return
        const symbol_type get_symbol(const alphabet_type sym_alpha_) throw() {
            typename detail::AlphaMap<AlphaT, symbol_type>::iterator sym_pos(
                symbol_map_inv.find(sym_alpha_)
            );

            // symbol exists, yay!
            if(symbol_map_inv.end() != sym_pos) {
                return sym_pos->second;
            }

            const alphabet_type sym_alpha(traits_type::copy(sym_alpha_));
            symbol_type sym(++num_symbols_);

            symbol_map_inv[sym_alpha] = sym;

            symbol_rep_type sym_rep(sym_alpha, tdop::symbol_tag());
            symbol_map.append(sym_rep);
            sym_rep.clear();

            return sym;
        }

        const symbol_type add_variable_symbol(void) throw(); // TODO

        unsigned num_categories(void) const throw() {
            return num_categories_;
        }

        unsigned num_symbols(void) const throw() {
            return num_symbols_;
        }

        unsigned num_rules(void) const throw() {
            return num_rules_;
        }

        unsigned num_rules(category_type cat_) const throw() {
            internal_category_type *cat(find_category(cat_));
            assert(0 != cat);
            return cat->num_extension_rules + cat->num_initial_rules;
        }
        unsigned num_initial_rules(category_type cat_) const throw() {
            internal_category_type *cat(find_category(cat_));
            assert(0 != cat);
            return cat->num_initial_rules;
        }
        unsigned num_extension_rules(category_type cat_) const throw() {
            internal_category_type *cat(find_category(cat_));
            assert(0 != cat);
            return cat->num_extension_rules;
        }

        /// pattern matching for basics (all of some type of thing)
        generator_type search(tdop::Unbound<AlphaT,tdop::category_tag> cat) throw() {
            typedef tdop::detail::CategoryGenerator<AlphaT> category_generator;
            generator_type gen;
            gen.machine = this;
            gen.binder.category = cat.expr;
            gen.match_ = category_generator::match;
            gen.reset_ = category_generator::reset;
            gen.free_ = category_generator::free;
            return gen;
        }
        generator_type search(tdop::Unbound<AlphaT,tdop::symbol_tag> sym) throw() {
            typedef tdop::detail::SymbolGenerator<AlphaT> symbol_generator;
            generator_type gen;
            gen.machine = this;
            gen.binder.symbol = sym.expr;
            gen.match_ = symbol_generator::match;
            gen.reset_ = symbol_generator::reset;
            gen.free_ = symbol_generator::free;
            return gen;
        }
        generator_type search(tdop::Unbound<AlphaT,tdop::rule_tag> rule) throw() {
            typedef tdop::detail::RuleGenerator<AlphaT> rule_generator;
            generator_type gen;
            gen.machine = this;
            gen.binder.rule = rule.expr;
            gen.match_ = rule_generator::match;
            gen.reset_ = rule_generator::reset;
            gen.free_ = rule_generator::free;
            return gen;
        }

        /// pattern-based generators
        generator_type search(
            tdop::Unbound<AlphaT,tdop::rule_tag> rule,
            pattern_type &pattern
        ) throw() {
            typedef tdop::detail::PatternGenerator<AlphaT,false> pattern_generator;
            generator_type gen;
            gen.machine = this;
            gen.binder.rule = rule.expr;
            gen.bind = pattern.bind;
            gen.pattern = pattern.pattern;

            tdop::detail::PatternData<AlphaT>::incref(gen.pattern);

            gen.match_ = pattern_generator::match;
            gen.reset_ = pattern_generator::reset;
            gen.free_ = pattern_generator::free;
            return gen;
        }

        /// pattern-based generators
        generator_type search(
            pattern_type &pattern
        ) throw() {
            typedef tdop::detail::PatternGenerator<AlphaT,false> pattern_generator;
            generator_type gen;
            gen.machine = this;
            gen.bind = pattern.bind;
            gen.pattern = pattern.pattern;

            tdop::detail::PatternData<AlphaT>::incref(gen.pattern);

            gen.match_ = pattern_generator::match;
            gen.reset_ = pattern_generator::reset;
            gen.free_ = pattern_generator::free;
            return gen;
        }

        /// pattern-based generators
        template <
            typename A,
            typename B, // category tag
            typename C,
            const bool D
        >
        generator_type search(
            tdop::Unbound<AlphaT,tdop::rule_tag> rule,
            const tdop::detail::PatternBuilder<A,B,C,D> pattern
        ) throw() {
            typedef tdop::detail::PatternGenerator<
                AlphaT,
                mpl::IfTypesEqual<B,tdop::category_tag>::RESULT ||
                mpl::IfTypesEqual<B,tdop::category_lb_tag>::RESULT
            > pattern_generator;

            generator_type gen;
            gen.machine = this;
            gen.binder.rule = rule.expr;
            gen.bind = pattern::DestructuringBind<A,B,C>::bind;
            gen.pattern = pattern.pattern;

            tdop::detail::PatternData<AlphaT>::incref(gen.pattern);

            gen.match_ = pattern_generator::match;
            gen.reset_ = pattern_generator::reset;
            gen.free_ = pattern_generator::free;
            return gen;
        }

        /// pattern-based generators
        template <
            typename A,
            typename B, // category tag
            typename C,
            const bool D
        >
        generator_type search(
            const tdop::detail::PatternBuilder<A,B,C,D> pattern
        ) throw() {
            typedef tdop::detail::PatternGenerator<
                AlphaT,
                mpl::IfTypesEqual<B,tdop::category_tag>::RESULT ||
                mpl::IfTypesEqual<B,tdop::category_lb_tag>::RESULT
            > pattern_generator;
            generator_type gen;
            gen.machine = this;
            gen.bind = pattern::DestructuringBind<A,B,C>::bind;
            gen.pattern = pattern.pattern;

            tdop::detail::PatternData<AlphaT>::incref(gen.pattern);

            gen.match_ = pattern_generator::match;
            gen.reset_ = pattern_generator::reset;
            gen.free_ = pattern_generator::free;
            return gen;
        }

    private:

        /// go find the next parser category in some direction (within the
        /// category map)
        internal_category_type *find_category(
            const uint32_t id_,
            const int32_t increment
        ) throw() {
            const int32_t id(static_cast<int32_t>(id_));
            const int32_t next_category_id_(static_cast<int32_t>(next_category_id));
            internal_category_type *next(0);
            for(int32_t i(id + increment);
                i < next_category_id_ && 0 < i;
                i += increment) {

                next = category_map.get(static_cast<unsigned>(i));
                if(0 != next) {
                    break;
                }
            }
            return next;
        }

        /// find a category, based on an opaque category representation
        internal_category_type *find_category(category_type cat) const throw() {
            return category_map.get(static_cast<uint32_t>(cat.val));
        }

        /// used for checking if a symbol name (a variable name, variable
        /// terminal name) is correctly formatted. this doesn't do complete
        /// or proper checking, it is mainly an extra check to ensure that
        /// imported auto-generated symbol names are indeed correctly
        /// formatted.
        static bool is_valid_symbol_name(const char *name) throw() {
            if(0 == name || ('$' != *name && '\0' != *name)) {
                return true;
            }

            for(const char *curr(name + 1); '\0' != *curr; ++curr) {
                if(!isdigit(*curr)) {
                    return false;
                }
            }

            return true;
        }

        /// add a parser rule into a TDOP cateogory. this is used for both
        /// initial and extension rules.
        internal_rule_type *add_rule(
            internal_rule_type *rule_,
            internal_rule_type **rule_list,
            unsigned *cat_num_rules
        ) throw() {
            internal_rule_type *rule(0);
            internal_rule_type *prev(0);
            internal_rule_type *next(0);

            for(next = *rule_list;
                0 != next;
                prev = next, next = next->next) {

                // next < rule
                if(next->is_less_than(rule_)) {
                    continue;

                // !(next < rule) ==> next >= rule
                // rule < next
                } else if(rule_->is_less_than(next)) {
                    break;

                // rule == next
                } else {
                    rule = next;
                    goto done;
                }
            }

            ++num_rules_;
            ++(*cat_num_rules);

            // rule is either at the end, or there are no rules
            rule = rule_allocator.allocate();
            memcpy(rule, rule_, sizeof *rule_);
            memset(rule_, 0, sizeof *rule_); // a bit unsafe :-P

            // chain the rule in
            rule->prev = prev;
            rule->next = next;

            if(0 != prev) {
                prev->next = rule;
            }

            if(0 != next) {
                next->prev = rule;
            }

            // (re)initialize this rule list
            // if list is empty, then *rule_list == 0 and next == 0
            // if rule is the new head, then next == *rule_list
            if(next == *rule_list) {
                *rule_list = rule;
            }

        done:

            rule->is_deleted = false;

            return rule;
        }

        /// get the next automatic symbol/category name
        const char *next_symbol_upper_bound(void) const throw() {
            const unsigned long prev_ub(strtoul(
               &(auto_symbol_upper_bound[1]), 0, 10
           ));

           char buffer[100] = {'\0'};
           sprintf(buffer, "$%lu", prev_ub + 1);

           const char *next(trait::Alphabet<const char *>::copy(buffer));

           auto_symbol_upper_bound = next;

           return next;
        }

        /// return the first category
        inline internal_category_type *first_category(void) throw() {
            return find_category(0, 1);
        }
    };

    /// static initialization of rule allocator
    template <typename AlphaT>
    helper::BlockAllocator<typename TDOP<AlphaT>::internal_rule_type>
    TDOP<AlphaT>::rule_allocator;
}

#endif /* Grail_Plus_TDOP_HPP_ */
