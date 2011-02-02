/*
 * ContextFreeGrammar.hpp
 *
 *  Created on: Sep 19, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef FLTL_LIB_CONTEXTFREEGRAMMAR_HPP_
#define FLTL_LIB_CONTEXTFREEGRAMMAR_HPP_

#include <cassert>
#include <cstring>
#include <map>
#include <utility>
#include <stdint.h>

#include "fltl/include/helper/Align.hpp"
#include "fltl/include/helper/Array.hpp"
#include "fltl/include/helper/BlockAllocator.hpp"
#include "fltl/include/helper/StorageChain.hpp"
#include "fltl/include/helper/UnsafeCast.hpp"

#include "fltl/include/mpl/If.hpp"
#include "fltl/include/mpl/Static.hpp"

#include "fltl/include/preprocessor/CATENATE.hpp"
#include "fltl/include/preprocessor/FORCE_INLINE.hpp"
#include "fltl/include/preprocessor/REPEAT_LEFT.hpp"

#include "fltl/include/trait/Uncopyable.hpp"

/// make a method for pattern builder
#define _FLTL_CFG_UNBOUND(type) cfg::Unbound<AlphaT,type>

#define FLTL_CFG_PRODUCTION_PATTERN \
    FLTL_FORCE_INLINE cfg::Pattern<AlphaT,self_type> \
    operator--(int) const throw() { \
        return cfg::Pattern<AlphaT,self_type>(*const_cast<self_type *>(this)); \
    }

namespace fltl { namespace lib {

    // forward declaration
    template <typename AlphaT>
    class CFG;

    namespace cfg {

        /// type used to represent symbols (terminals, non-terminals)
        /// of a grammar.
        typedef int32_t internal_sym_type;

        // forward declarations
        template <typename> class Variable;
        template <typename> class Production;
        template <typename> class OpaqueProduction;
        template <typename> class ProductionBuilder;
        template <typename> class Symbol;
        template <typename> class SymbolString;
        template <typename, typename> class Unbound;
        template <typename> class Generator;

        template <typename, typename> class Pattern;
        template <typename> class AnySymbol;
        template <typename> class AnySymbolString;

        namespace detail {

            // forward declarations
            template <typename, const unsigned>
            struct SymbolStringAllocator;

            template <typename>
            class SimpleGenerator;

            template <typename, typename>
            class PatternGenerator;

            template <typename, typename, typename, const unsigned>
            class PatternBuilder;

            template <typename, typename, const unsigned, typename, typename>
            class Match2;

            template <typename, typename, typename, const unsigned>
            class ResetPattern;

            template <typename, typename, typename, typename>
            class DestructuringBind;
        }
    }

}}

#include "fltl/lib/cfg/Symbol.hpp"
#include "fltl/lib/cfg/Production.hpp"
#include "fltl/lib/cfg/Variable.hpp"

namespace fltl { namespace lib {

    /// context-free grammar type.
    ///
    /// Assumptions:
    ///     - AlphaT has a strict weak ordering.
    ///     - AlphaT is default constructible
    ///     - AlphaT is copy constructible
    template <typename AlphaT>
    class CFG : protected trait::Uncopyable {
    private:

        // friend declarations
        template <typename, typename> friend class QueryBuilder;
        friend class cfg::Variable<AlphaT>;
        friend class cfg::ProductionBuilder<AlphaT>;
        friend class cfg::Production<AlphaT>;
        friend class cfg::detail::SimpleGenerator<AlphaT>;

        template <typename, typename>
        friend class cfg::detail::PatternGenerator;

        template <typename, const unsigned>
        friend class cfg::detail::SymbolStringAllocator;

        /// the next variable id that can be assigned, goes toward +inf
        cfg::internal_sym_type next_variable_id;

        /// the next terminal id that can be assigned, goes toward -inf
        cfg::internal_sym_type next_terminal_id;

        /// injective mapping between non-zero negative integers and pointers
        /// to the parameterized alphabet type. the association between
        /// terminals and their representations needs to be maintained.
        helper::Array<AlphaT> terminal_map;
        std::map<AlphaT, cfg::internal_sym_type> terminal_map_inv;

        /// injective mapping between non-zero positive integers and pointers
        /// to the structure containing the productions related to the
        /// variable.
        helper::Array<cfg::Variable<AlphaT> *> variable_map;

        /// number of productions
        unsigned num_productions_;

        /// the first production in the variable, i.e. the first production
        /// of the smallest variable.
        cfg::Production<AlphaT> *first_production;

        /// the start variable
        cfg::Variable<AlphaT> *start_variable;

        /// allocator for variables
        static helper::StorageChain<helper::BlockAllocator<
            cfg::Variable<AlphaT>
        > > variable_allocator;

        /// allocator for productions
        static helper::StorageChain<helper::BlockAllocator<
            cfg::Production<AlphaT>
        > > production_allocator;

    public:

        /// arbitrary symbol (terminal, non-terminal) of a grammar
        typedef cfg::Symbol<AlphaT> symbol_type;

        /// type of a production builder
        typedef cfg::ProductionBuilder<AlphaT> production_builder_type;

        /// represents a production
        typedef cfg::OpaqueProduction<AlphaT> production_type;

        /// string of symbols
        typedef cfg::SymbolString<AlphaT> symbol_string_type;

        /// generator of search results
        typedef cfg::Generator<AlphaT> generator_type;

        /// represents a terminal of a grammar
        class terminal_type : public cfg::Symbol<AlphaT> {
        private:
            friend class CFG<AlphaT>;

            explicit terminal_type(const cfg::internal_sym_type _value) throw()
                : cfg::Symbol<AlphaT>(_value)
            { }

        public:

            terminal_type(void) throw()
                : cfg::Symbol<AlphaT>(-1)
            { }

            /// return an "unbound" version of this symbol
            /// note: *not* const!!
            cfg::Unbound<AlphaT,terminal_type> operator~(void) throw() {
                return cfg::Unbound<AlphaT,terminal_type>(this);
            }
        };

        /// represents a non-terminal of a grammar
        class variable_type : public cfg::Symbol<AlphaT> {
        private:

            friend class CFG<AlphaT>;
            friend class cfg::OpaqueProduction<AlphaT>;

            typedef variable_type self_type;

            explicit variable_type(const cfg::internal_sym_type _value) throw()
                : cfg::Symbol<AlphaT>(_value)
            { }

        public:

            variable_type(void) throw()
                : cfg::Symbol<AlphaT>(1)
            { }

            /// return an "unbound" version of this symbol
            /// note: *not* const!!
            cfg::Unbound<AlphaT,variable_type> operator~(void) throw() {
                return cfg::Unbound<AlphaT,variable_type>(this);
            }

            FLTL_CFG_PRODUCTION_PATTERN
        };

        /// pattern type, nicely encapsulates a destructuring production
        /// pattern
        class pattern_type {
        private:

            friend class CFG<AlphaT>;

            void *pattern;
            bool (*match_pattern)(void *, const production_type &);
            bool (*gen_next)(generator_type *);
            void (*gen_reset)(generator_type *);

        public:

            template <typename PatternT, typename StringT, const unsigned state>
            pattern_type(
                cfg::detail::PatternBuilder<AlphaT,PatternT,StringT,state> pattern_builder
            ) throw()
                : pattern(reinterpret_cast<void *>(pattern_builder.pattern))
                , match_pattern(&(cfg::detail::PatternBuilder<AlphaT,PatternT,StringT,state>::static_match))
                , gen_next(&(cfg::detail::PatternGenerator<
                    AlphaT,
                    cfg::detail::PatternBuilder<AlphaT,PatternT,StringT,state>
                >::bind_next_pattern))
                , gen_reset(&(cfg::detail::PatternGenerator<
                    AlphaT,
                    cfg::detail::PatternBuilder<AlphaT,PatternT,StringT,state>
                >::reset_next_pattern))
            { }

            ~pattern_type(void) throw() {
                pattern = 0;
            }

            template <typename PatternT, typename StringT, const unsigned state>
            pattern_type &operator=(
                cfg::detail::PatternBuilder<AlphaT,PatternT,StringT,state> pattern_builder
            ) throw() {
                pattern = reinterpret_cast<void *>(pattern_builder.pattern);
                match_pattern = &(cfg::detail::PatternBuilder<AlphaT,PatternT,StringT,state>::static_match);
                gen_next = &(cfg::detail::PatternGenerator<
                    AlphaT,
                    cfg::detail::PatternBuilder<AlphaT,PatternT,StringT,state>
                >::bind_next_pattern);
                gen_reset = &(cfg::detail::PatternGenerator<
                    AlphaT,
                    cfg::detail::PatternBuilder<AlphaT,PatternT,StringT,state>
                >::reset_next_pattern);
                return *this;
            }

            /// try to match against the pattern
            inline bool match(const production_type &prod) const throw() {
                return match_pattern(pattern, prod);
            }
        };

        /// short forms
        typedef symbol_type sym_t;
        typedef production_builder_type prod_builder_t;
        typedef terminal_type term_t;
        typedef variable_type var_t;
        typedef production_type prod_t;
        typedef symbol_string_type sym_str_t;
        typedef generator_type generator_t;
        typedef pattern_type pattern_t;

        /// represents any single symbol in a production
        const cfg::AnySymbol<AlphaT> _;
        const cfg::AnySymbolString<AlphaT> __;

        /// constructor
        CFG(void) throw()
            : trait::Uncopyable()
            , next_variable_id(1)
            , next_terminal_id(-1)
            , terminal_map()
            , terminal_map_inv()
            , variable_map()
            , num_productions_(0)
            , first_production(0)
            , start_variable(0)
            , _()
            , __()
        {
            terminal_map.reserve(256U);
            variable_map.reserve(256U);

            terminal_map.append(mpl::Static<AlphaT>::VALUE);
            variable_map.append(variable_allocator->allocate());
        }

        /// destructor
        ~CFG(void) throw() {

            // free the variables
            for(cfg::Variable<AlphaT> *var(variable_map.get(0)), *next_var(0);
                0 != var;
                var = next_var) {

                next_var = var->next;
                variable_map.set(var->id, 0);
                variable_allocator->deallocate(var);
            }

            first_production = 0;
            num_productions_ = 0;
        }

        const variable_type get_start_variable(void) const throw() {
            assert(
                0 != start_variable &&
                "This grammar doesn't have a start variable."
            );

            return variable_type(start_variable->id);
        }

        /// change the start variable
        void set_start_variable(const variable_type &var) throw() {
            start_variable = get_variable(var);
        }

        /// add a new variable to the
        const variable_type add_variable(void) throw() {
            cfg::Variable<AlphaT> *var(variable_allocator->allocate());
            var->init(next_variable_id, variable_map.back());

            cfg::internal_sym_type var_id(next_variable_id);

            ++next_variable_id;
            variable_map.back()->next = var;
            variable_map.append(var);

            if(0 == start_variable) {
                start_variable = var;
            }

            if(0 == first_production) {
                first_production = var->first_production;
            }

            // because each variable starts with an epsilon production
            ++num_productions_;

            variable_type ret(var_id);
            return ret;
        }

        /// get the terminal reference for a particular terminal.
        const terminal_type get_terminal(const AlphaT term) throw() {
            cfg::internal_sym_type &term_id(terminal_map_inv[term]);

            if(0 == term_id) {
                term_id = next_terminal_id;
                --next_terminal_id;
                terminal_map.append(term);
            }

            terminal_type ret(term_id);
            return ret;
        }

        /// add a production to the grammar from a symbol string
        const production_type add_production(
            const variable_type _var,
            symbol_string_type str
        ) throw() {

            cfg::Variable<AlphaT> *var(get_variable(_var));
            cfg::Production<AlphaT> *prod(production_allocator->allocate());

            prod->var = var;
            prod->symbols.assign(str);

            // this variable has productions already
            if(&(var->null_production) != var->first_production) {

                // make sure the production is unique
                for(cfg::Production<AlphaT> *related_prod(var->first_production);
                    0 != related_prod;
                    related_prod = related_prod->next) {
                    if(related_prod->is_equivalent_to(*prod)) {

                        // was the related production deleted?
                        if(related_prod->is_deleted) {
                            related_prod->is_deleted = false;
                            cfg::Production<AlphaT>::hold(prod);
                        }

                        production_allocator->deallocate(prod);
                        return production_type(related_prod);
                    }
                }

                // only increment the number of productions if the variable
                // has non-default productions
                ++num_productions_;
            }

            // keep track of the first production
            if(0 != first_production) {
                if(var->id <= first_production->var->id) {
                    first_production = prod;
                }
            } else {
                first_production = prod;
            }

            // inductive step: add the production to the current variable
            cfg::Production<AlphaT>::hold(prod);
            var->add_production(prod);
            return production_type(prod);
        }

        /// add a production to the grammar that has the sames symbols as
        /// another production
        inline const production_type add_production(
            const variable_type _var,
            production_type _prod
        ) throw() {
            return add_production(_var, _prod.symbols());
        }

        /// add a production to the grammar from a symbol
        inline const production_type add_production(
            const variable_type _var,
            const symbol_type _sym
        ) throw() {
            return add_production(_var, _sym + epsilon());
        }

        /// add a production to the grammar from a production builder
        inline const production_type add_production(
            const variable_type _var,
            production_builder_type &builder
        ) throw() {
            return add_production(_var, builder.symbols());
        }

        /// remove a production from the grammar
        void remove_production(production_type &_prod) throw() {

            assert(
                0 != _prod.production &&
                "Cannot remove invalid production."
            );

            assert(
                !_prod.production->is_deleted &&
                "Cannot remove production that has already been removed."
            );

            cfg::Production<AlphaT> *prod(_prod.production);
            cfg::Variable<AlphaT> *var(prod->var);

            assert(
                (prod == &(var->null_production) ||
                 &(var->null_production) != var->first_production) &&
                "The variable is in an invalid state."
            );

            // was this the first production?
            if(first_production == prod) {
                cfg::Production<AlphaT> *next_prod(prod->next);
                cfg::Variable<AlphaT> *curr_var(var);

                // we are removing a default production, which actually
                // doesn't remove it. if this production is a default and
                // the first production, then keep it as the first
                // production
                if(&(var->null_production) == prod) {
                    goto done;
                }

                // go look for the next production
                while(true) {
                    for(; 0 != next_prod; next_prod = next_prod->next) {
                        if(!next_prod->is_deleted) {
                            goto found_next_prod;
                        }
                    }

                    curr_var = curr_var->next;

                    if(0 == curr_var) {
                        goto found_next_prod;
                    }

                    next_prod = curr_var->first_production;
                }

            found_next_prod:

                first_production = next_prod;
            }

            // this production is not a default production, mark it as
            // deleted and decrement its reference counter
            if(&(var->null_production) != prod) {
                prod->is_deleted = true;
                cfg::Production<AlphaT>::release(prod);
                --num_productions_;

                // go look to see if there are any non-deleted productions
                // that are still related to this variable
                for(cfg::Production<AlphaT> *pp(var->first_production);
                    0 != pp;
                    pp = pp->next) {

                    // there is at least one non-deleted production still
                    // related to this variable; we don't need to add the
                    // default null production back in
                    if(!pp->is_deleted) {
                        goto done;
                    }
                }

                // put the null production up front
                ++num_productions_;
                var->first_production->prev = &(var->null_production);
                var->null_production.next = var->first_production;
                var->first_production = &(var->null_production);
            }

        done:

            prod = 0;
        }

        /// get the variable representing the empty string, epsilon
        inline const symbol_string_type &epsilon(void) const throw() {
            return mpl::Static<symbol_string_type>::VALUE;
        }

        /// get the number of variables in this CFG
        inline unsigned num_variables(void) const throw() {
            return variable_map.size() - 1U;
        }

        /// get the number of productions in the CFG
        inline unsigned num_productions(void) const throw() {
            return num_productions_;
        }

        /// get the number of terminals in the CFG; note: not all terminals
        /// are necessarily reachable
        inline unsigned num_terminals(void) const throw() {
            return terminal_map.size() - 1U;
        }

        /// create a variable generator
        inline generator_type
        search(cfg::Unbound<AlphaT, variable_type> sym) throw() {
            generator_type gen(
                this,
                reinterpret_cast<void *>(sym.symbol), // binder
                reinterpret_cast<void *>(0), // pattern
                &(cfg::detail::SimpleGenerator<AlphaT>::bind_next_variable),
                &(cfg::detail::SimpleGenerator<AlphaT>::reset_next_variable)
            );

            gen.cursor.variable = variable_map.get(0);

            return gen;
        }

        /// create a terminal generator
        inline generator_type
        search(cfg::Unbound<AlphaT, terminal_type> sym) throw() {
            generator_type gen(
                this,
                reinterpret_cast<void *>(sym.symbol), // binder
                reinterpret_cast<void *>(0), // pattern
                &(cfg::detail::SimpleGenerator<AlphaT>::bind_next_terminal),
                &(cfg::detail::SimpleGenerator<AlphaT>::reset_next_terminal)
            );

            gen.cursor.terminal_offset = 1U;

            return gen;
        }

        /// create a production generator
        inline generator_type
        search(cfg::Unbound<AlphaT, production_type> uprod) throw() {

            if(0 == first_production) {
                return cfg::Generator<AlphaT>();
            }

            generator_type gen(
                this,
                reinterpret_cast<void *>(uprod.prod), // binder
                reinterpret_cast<void *>(0), // pattern
                &(cfg::detail::SimpleGenerator<AlphaT>::bind_next_production),
                &(cfg::detail::SimpleGenerator<AlphaT>::reset_next_production)
            );

            gen.cursor.production = first_production;

            return gen;
        }

        /// return an empty generator for symbols
        inline generator_type
        search(cfg::Unbound<AlphaT, symbol_type>) throw() {
            return cfg::Generator<AlphaT>();
        }

        /// go find all productions that fit a certain pattern. bind each
        /// production to uprod, and also destructure each production
        /// according to the pattern.
        template <typename PatternT, typename StringT, const unsigned state>
        inline generator_type
        search(
            cfg::Unbound<AlphaT, production_type> uprod,
            cfg::detail::PatternBuilder<AlphaT,PatternT,StringT,state> pattern_builder
        ) throw() {
            generator_type gen(
                this,
                reinterpret_cast<void *>(uprod.prod), // binder
                reinterpret_cast<void *>(pattern_builder.pattern), // pattern
                &(cfg::detail::PatternGenerator<
                    AlphaT,
                    cfg::detail::PatternBuilder<AlphaT,PatternT,StringT,state>
                >::bind_next_pattern),
                &(cfg::detail::PatternGenerator<
                    AlphaT,
                    cfg::detail::PatternBuilder<AlphaT,PatternT,StringT,state>
                >::reset_next_pattern)
            );

            gen.cursor.production = first_production;

            return gen;
        }

        /// go find all productions that fit a certain pattern. destructure
        /// each production according to the pattern.
        template <typename PatternT, typename StringT, const unsigned state>
        inline generator_type
        search(
            cfg::detail::PatternBuilder<AlphaT,PatternT,StringT,state> pattern_builder
        ) throw() {
            generator_type gen(
                this,
                reinterpret_cast<void *>(0), // binder
                reinterpret_cast<void *>(pattern_builder.pattern), // pattern
                &(cfg::detail::PatternGenerator<
                    AlphaT,
                    cfg::detail::PatternBuilder<AlphaT,PatternT,StringT,state>
                >::bind_next_pattern),
                &(cfg::detail::PatternGenerator<
                    AlphaT,
                    cfg::detail::PatternBuilder<AlphaT,PatternT,StringT,state>
                >::reset_next_pattern)
            );

            gen.cursor.production = first_production;

            return gen;
        }

        /// go find all productions that fit a certain pattern. destructure
        /// each production according to the pattern.
        inline generator_type search(pattern_type &pattern) throw() {
            generator_type gen(
                this,
                reinterpret_cast<void *>(0), // binder
                reinterpret_cast<void *>(pattern.pattern), // pattern
                pattern.gen_next,
                pattern.gen_reset
            );

            gen.cursor.production = first_production;

            return gen;
        }

        /// go find all productions that fit a certain pattern. bind each
        /// production to uprod, and also destructure each production
        /// according to the pattern.
        inline generator_type search(
            cfg::Unbound<AlphaT, production_type> uprod,
            pattern_type &pattern
        ) throw() {
            generator_type gen(
                this,
                reinterpret_cast<void *>(uprod.prod), // binder
                reinterpret_cast<void *>(pattern.pattern), // pattern
                pattern.gen_next,
                pattern.gen_reset
            );

            gen.cursor.production = first_production;

            return gen;
        }

    private:

        inline cfg::Variable<AlphaT> *
        get_variable(const variable_type _var) throw() {
            assert(
                0 < _var.value &&
                _var.value < next_variable_id &&
                "Invalid variable passed to add_production()."
            );

            cfg::Variable<AlphaT> *var(variable_map.get(_var.value));

            assert(
                0 != var &&
                "Invalid variable passed to add_production()."
            );

            return var;
        }

    public:

        void debug(const production_type &prod) throw() {
            if(!prod.is_valid()) {
                printf("<empty production>\n");
            } else {
                printf("\033[33m%d\033[0m -> ", prod.variable().value);
                debug(prod.symbols());
            }
        }

        void debug(const symbol_string_type &syms) throw() {
            for(unsigned i(0); i < syms.length(); ++i) {
                if(0 < syms[i].value) {
                    printf("\033[33m%d ", syms[i].value);
                } else {
                    printf("\033[35m%c ", terminal_map.get(-1 * syms[i].value));
                }
            }
            printf("\033[0m\n");
        }

        void debug(const symbol_type &sym) throw() {
            if(0 == sym.value) {
                printf("\x27\n");
            } else if(0 < sym.value) {
                printf("\033[33m%d\033[0m\n", sym.value);
            } else {
                printf("\033[35m%c\033[0m\n", terminal_map.get(-1 * sym.value));
            }
        }
    };

    // initialize the static variables
    template <typename AlphaT>
    helper::StorageChain<helper::BlockAllocator<
        cfg::Variable<AlphaT>
    > > CFG<AlphaT>::variable_allocator;

    template <typename AlphaT>
    helper::StorageChain<helper::BlockAllocator<
        cfg::Production<AlphaT>
    > > CFG<AlphaT>::production_allocator(CFG<AlphaT>::variable_allocator);
}}

#include "fltl/lib/cfg/ProductionBuilder.hpp"
#include "fltl/lib/cfg/SymbolString.hpp"
#include "fltl/lib/cfg/OpaqueProduction.hpp"
#include "fltl/lib/cfg/Unbound.hpp"
#include "fltl/lib/cfg/Generator.hpp"
#include "fltl/lib/cfg/Pattern.hpp"

#endif /* FLTL_LIB_CONTEXTFREEGRAMMAR_HPP_ */
