/*
 * ContextFreeGrammar.hpp
 *
 *  Created on: Sep 19, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef FLTL_LIB_CONTEXTFREEGRAMMAR_HPP_
#define FLTL_LIB_CONTEXTFREEGRAMMAR_HPP_

#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <map>
#include <list>
#include <utility>
#include <stdint.h>
#include <functional>

#include "fltl/include/helper/Align.hpp"
#include "fltl/include/helper/Array.hpp"
#include "fltl/include/helper/BlockAllocator.hpp"
#include "fltl/include/helper/StorageChain.hpp"
#include "fltl/include/helper/UnsafeCast.hpp"

#include "fltl/include/mpl/If.hpp"
#include "fltl/include/mpl/Static.hpp"
#include "fltl/include/mpl/UserOperators.hpp"

#include "fltl/include/preprocessor/CATENATE.hpp"
#include "fltl/include/preprocessor/COLOR.hpp"
#include "fltl/include/preprocessor/FORCE_INLINE.hpp"
#include "fltl/include/preprocessor/REPEAT_LEFT.hpp"

#include "fltl/include/trait/Alphabet.hpp"
#include "fltl/include/trait/Uncopyable.hpp"

/// make a method for pattern builder
#define _FLTL_CFG_UNBOUND(tag) cfg::Unbound<AlphaT,tag>

#define FLTL_CFG_PRODUCTION_PATTERN(tag) \
    FLTL_FORCE_INLINE cfg::Pattern<AlphaT,tag> \
    operator--(int) const throw() { \
        return cfg::Pattern<AlphaT,tag>( \
            cfg::detail::PatternData<AlphaT>::allocate( \
                const_cast<self_type *>(this)\
            ) \
        ); \
    }

#define FLTL_CFG_USE_TYPES_PREFIX_FUNC(type, prefix, func) \
    typedef typename type::alphabet_type func(prefix, alphabet_type); \
    typedef typename type::traits_type func(prefix, traits_type); \
    typedef typename type::symbol_type func(prefix, symbol_type); \
    typedef typename type::symbol_buffer_type func(prefix, symbol_buffer_type); \
    typedef typename type::terminal_type func(prefix, terminal_type); \
    typedef typename type::variable_type func(prefix, variable_type); \
    typedef typename type::production_type func(prefix, production_type); \
    typedef typename type::symbol_string_type func(prefix, symbol_string_type); \
    typedef typename type::generator_type func(prefix, generator_type); \
    typedef typename type::pattern_type func(prefix, pattern_type); \
    typedef type func(prefix, cfg_type)

#define FLTL_CFG_NO_PREFIX(prefix, str) str
#define FLTL_CFG_USE_PREFIX(prefix, str) prefix ## _ ## str

#define FLTL_CFG_USE_TYPES(type) \
    FLTL_CFG_USE_TYPES_PREFIX_FUNC(type, foo, FLTL_CFG_NO_PREFIX)

#define FLTL_CFG_USE_TYPES_PREFIX(type, prefix) \
    FLTL_CFG_USE_TYPES_PREFIX_FUNC(type, prefix, FLTL_CFG_USE_PREFIX)

namespace fltl {

    // forward declaration
    template <typename> class CFG;

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
        template <typename> class TerminalSymbol;
        template <typename> class VariableSymbol;
        template <typename, typename> class Unbound;
        template <typename> class Generator;
        template <typename> class OpaquePattern;

        template <typename, typename> class Pattern;
        template <typename> class AnySymbol;
        template <typename> class AnySymbolString;
        template <typename> class AnySymbolStringOfLength;

        class symbol_tag { };
        class terminal_tag { };
        class variable_tag { };
        class symbol_string_tag { };
        class unbound_symbol_tag { };
        class unbound_variable_tag { };
        class unbound_terminal_tag { };
        class unbound_symbol_string_tag { };
        class any_symbol_tag { };
        class any_symbol_string_tag { };
        class any_symbol_string_of_length_tag { };
        class production_tag { };

        namespace detail {

            // forward declarations
            template <typename, const unsigned>
            class SymbolStringAllocator;

            template <typename>
            class SimpleGenerator;

            template <typename>
            class PatternData;

            template <typename, typename>
            class PatternGenerator;

            template <typename, typename, typename, const unsigned>
            class PatternBuilder;

            template <typename, typename>
            class PatternLiteral;

            template <typename, typename, const unsigned, typename, typename>
            class Match2;

            template <typename, typename, typename, const unsigned>
            class ResetPattern;

            template <typename, typename, typename,const unsigned>
            class DecRefCounts;

            template <typename, typename, typename>
            class DestructuringBind;
        }

        /// offsets into CFG symbol strings
        namespace str {
            enum {
                REF_COUNT = 0,
                HASH = 1,
                LENGTH = 2,
                FIRST_SYMBOL = 3
            };
        }
    }

}

#include "fltl/include/cfg/Symbol.hpp"
#include "fltl/include/cfg/TerminalSymbol.hpp"
#include "fltl/include/cfg/VariableSymbol.hpp"
#include "fltl/include/cfg/Production.hpp"
#include "fltl/include/cfg/Variable.hpp"

namespace fltl {

    /// context-free grammar type.
    ///
    /// Assumptions:
    ///     - AlphaT has a strict weak ordering.
    ///     - AlphaT is default constructible
    ///     - AlphaT is copy constructible
    template <typename AlphaT>
    class CFG : protected trait::Uncopyable {
    public:

        /// extract the traits
        typedef trait::Alphabet<AlphaT> traits_type;
        typedef typename traits_type::alphabet_type alphabet_type;

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
        mutable helper::Array<std::pair<alphabet_type, const char *> > terminal_map;
        typedef std::map<
            alphabet_type,
            cfg::internal_sym_type,
            typename traits_type::less_type
        > terminal_map_inv_type;
        terminal_map_inv_type terminal_map_inv;

        /// injective mapping between strings and terminal types representing
        /// variable terminals.
        typedef std::map<
            const char *,
            cfg::TerminalSymbol<AlphaT>,
            trait::Alphabet<const char *>::less_type
        > variable_terminal_map_type;
        variable_terminal_map_type variable_terminal_map;

        /// injective mapping between non-zero positive integers and pointers
        /// to the structure containing the productions related to the
        /// variable.
        mutable helper::Array<cfg::Variable<AlphaT> *> variable_map;
        typedef std::map<
            const char *,
            cfg::VariableSymbol<AlphaT>,
            trait::Alphabet<const char *>::less_type
        > named_variable_map_type;
        named_variable_map_type named_variable_map;

        /// unused variables
        cfg::Variable<AlphaT> *unused_variables;

        /// represents an upper-bound on any auto-generated symbol name
        mutable const char *auto_symbol_upper_bound;

        /// number of productions and variables
        unsigned num_productions_;
        unsigned num_variables_;

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

        // copy constructor
        CFG(const CFG<AlphaT> &) throw() { assert(false); }
        CFG<AlphaT> &operator=(const CFG<AlphaT> &) throw() {
            assert(false);
            return *this;
        }

        typedef CFG<AlphaT> self_type;

    public:

        /// arbitrary symbol (terminal, non-terminal) of a grammar
        typedef cfg::Symbol<AlphaT> symbol_type;

        /// type of a production builder
        typedef cfg::ProductionBuilder<AlphaT> symbol_buffer_type;

        /// represents a production
        typedef cfg::OpaqueProduction<AlphaT> production_type;

        /// string of symbols
        typedef cfg::SymbolString<AlphaT> symbol_string_type;

        /// generator of search results
        typedef cfg::Generator<AlphaT> generator_type;

        /// represents a terminal of a grammar
        typedef cfg::TerminalSymbol<AlphaT> terminal_type;

        /// represents a non-terminal of a grammar
        typedef cfg::VariableSymbol<AlphaT> variable_type;

        typedef cfg::OpaquePattern<AlphaT> pattern_type;

        /// short forms
        typedef symbol_type sym_t;
        typedef symbol_buffer_type sym_buff_t;
        typedef terminal_type term_t;
        typedef variable_type var_t;
        typedef production_type prod_t;
        typedef symbol_string_type sym_str_t;
        typedef generator_type generator_t;
        typedef pattern_type pattern_t;

        /// represents any single symbol in a production
        cfg::AnySymbol<AlphaT> _;

        /// represents and string of zero or more symbols in a production
        cfg::AnySymbolString<AlphaT> __;

        /// constructor
        CFG(void) throw()
            : trait::Uncopyable()
            , next_variable_id(1)
            , next_terminal_id(-1)
            , terminal_map(256U)
            , terminal_map_inv()
            , variable_terminal_map()
            , variable_map(256U)
            , named_variable_map()
            , unused_variables(0)
            , num_productions_(0)
            , num_variables_(0)
            , first_production(0)
            , start_variable(0)
            , _()
            , __()
        {
            static const char * const UB("$0");
            static const char * const EPSILON("epsilon");

            auto_symbol_upper_bound = UB;

            terminal_map.append(std::make_pair<alphabet_type,const char *>(
                mpl::Static<AlphaT>::VALUE,
                EPSILON
            ));
            variable_map.append(0);
        }

        /// destructor
        ~CFG(void) throw() {

            unsigned j(0);

            // free the variables
            const unsigned max(static_cast<unsigned>(next_variable_id));
            for(unsigned i(1U); i < max; ++i) {
                if(0 != variable_map.get(i)) {
                    variable_allocator->deallocate(variable_map.get(i));
                    variable_map.set(i, 0);
                    ++j;
                }
            }

            // free the terminals
            for(unsigned i(1U); i < terminal_map.size(); ++i) {
                std::pair<alphabet_type,const char *> &pp(
                    terminal_map.get(i)
                );

                traits_type::destroy(pp.first);
                trait::Alphabet<const char *>::destroy(pp.second);
            }

            for(cfg::Variable<AlphaT> *var(unused_variables), *next_var(0);
                0 != var;
                var = next_var) {

                next_var = var->next;
                variable_allocator->deallocate(var);
                ++j;
            }

            first_production = 0;
            unused_variables = 0;
            num_productions_ = 0;
            auto_symbol_upper_bound = 0;
        }

        /// get the starting variable for this grammar
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

        /// get a variable symbol. a variable symbol is either a variable
        /// or a variable terminal.
        const symbol_type get_variable_symbol(const char *name) throw() {

            assert(is_valid_symbol_name(name));

            typename named_variable_map_type::iterator var_loc(
                named_variable_map.find(name)
            );

            // it's a variable
            if(named_variable_map.end() != var_loc) {
                return (*var_loc).second;
            }

            typename variable_terminal_map_type::iterator term_loc(
                variable_terminal_map.find(name)
            );

            // it's a variable terminal
            if(variable_terminal_map.end() != term_loc) {
                return (*term_loc).second;
            }

            // create a variable terminal for it
            terminal_type term(next_terminal_id);
            --next_terminal_id;
            const char *name_copy(trait::Alphabet<const char *>::copy(name));
            terminal_map.append(std::make_pair(
                mpl::Static<alphabet_type>::VALUE,
                name_copy
            ));
            variable_terminal_map[name_copy] = term;

            // check if it's an upper bound
            if('$' == *name) {
                if(0 < strcmp(name, auto_symbol_upper_bound)) {
                    auto_symbol_upper_bound = name_copy;
                }
            }

            return term;
        }

        /// add in a new variable terminal with an automatically generated
        /// name
        const terminal_type add_variable_terminal(void) throw() {
            const unsigned long prev_ub(strtoul(
                &(auto_symbol_upper_bound[1]), 0, 10
            ));

            char buffer[1024] = {'\0'};
            sprintf(buffer, "$%lu", prev_ub + 1);

            terminal_type term(next_terminal_id);
            --next_terminal_id;

            const char *name(trait::Alphabet<const char *>::copy(buffer));
            terminal_map.append(std::make_pair(
                mpl::Static<alphabet_type>::VALUE,
                name
            ));

            auto_symbol_upper_bound = name;

            return term;
        }

        /// add or return an existing variable by name. this assumes a cstring
        /// is being passed. If the variable doesn't exist then a new cstring
        /// is allocated to represent it.
        const variable_type get_variable(const char *name) throw() {

            assert(is_valid_symbol_name(name));

            typename named_variable_map_type::iterator loc(
                named_variable_map.find(name)
            );

            // no variable with this name
            if(named_variable_map.end() == loc) {
                variable_type var(add_variable());

                const char *name_copy(trait::Alphabet<
                    const char *
                >::copy(name));

                get_variable(var)->name = name_copy;
                named_variable_map[name_copy] = var;

                // check if it's an upper bound
                if('$' == *name) {
                    if(0 < strcmp(name, auto_symbol_upper_bound)) {
                        auto_symbol_upper_bound = name_copy;
                    }
                }

                return var;
            } else {
                return (*loc).second;
            }
        }

        /// add a new variable to the grammar
        const variable_type add_variable(void) throw() {

            // get an allocated variable
            cfg::Variable<AlphaT> *var(unused_variables);
            cfg::internal_sym_type var_id(1);

            if(0 == var) {
                var = variable_allocator->allocate();
                var->name = 0;
                var_id = next_variable_id;
                ++next_variable_id;
                variable_map.append(var);
            } else {
                unused_variables = var->next;
                var_id = var->id;
                variable_map.set(static_cast<unsigned>(var_id), var);
            }

            cfg::Variable<AlphaT> *prev(find_variable(var_id, -1));
            cfg::Variable<AlphaT> *next(find_variable(var_id, 1));

            // initialize
            var->prev = prev;
            var->next = next;

            var->id = var_id;
            var->num_productions = 0;

            if(0 != prev) {
                prev->next = var;
            }

            if(0 != next) {
                next->prev = var;
            }

            if(0 == start_variable) {
                start_variable = var;
            }

            ++num_variables_;

            variable_type ret(var_id);
            return ret;
        }

        /// remove a variable from a grammar. this marks all of the variables
        /// productions as deleted. this, however, does NOT ensure that the
        /// variable is used anywhere else in the grammar.
        ///
        /// !!! this modifies any living productions so that generators
        ///     pointing at productions of this variable can recover.
        void unsafe_remove_variable(const variable_type _var) throw() {

            cfg::Variable<AlphaT> *var(get_variable(_var));

            // update the first production
            if(0 != first_production && first_production->var == var) {
                set_next_production(var->id + 1);
            }

            if(0 != var->prev) {
                assert(0 != var->prev->next);
                var->prev->next = var->next;
            }

            if(0 != var->next) {
                assert(0 != var->next->prev);
                var->next->prev = var->prev;
            }

            // release the productions
            cfg::Production<AlphaT> *prod(var->first_production);
            cfg::Production<AlphaT> *next_prod(0);

            // mark the related productions as deleted
            for(; 0 != prod; prod = next_prod) {

                next_prod = prod->next;
                prod->var = 0;
                prod->next = 0;

                // !!! this is to allow generators to recover from variable
                //     deletions
                prod->prev = helper::unsafe_cast<
                    cfg::Production<AlphaT> *
                >(var);

                if(prod->is_deleted) {
                    continue;
                }

                prod->is_deleted = true;
                cfg::Production<AlphaT>::release(prod);
                --num_productions_;
            }

            // clear out this var's info
            var->first_production = 0;
            var->num_productions = 0;
            var->prev = 0;

            // add it to the unused variable list
            var->next = unused_variables;
            unused_variables = var;

            variable_map.set(static_cast<unsigned>(var->id), 0);

            --num_variables_;
        }

        /// remove a variable and all productions in the grammar that
        /// relate to this variable. this can have the effect of removing
        /// productions all over the place.
        void remove_variable(const variable_type _var) throw() {

            production_type P;
            variable_type V;
            variable_type R;

            generator_type prods_on_var(search(~P, R --->* __));
            generator_type prods_using_var(
                search(~P, (~V) --->* __ + R + __)
            );

            std::list<variable_type> vars_to_remove;
            vars_to_remove.push_back(_var);

            while(!vars_to_remove.empty()) {

                R = vars_to_remove.back();

                vars_to_remove.pop_back();

                // remove productions on this variable
                for(prods_on_var.rewind();
                    prods_on_var.match_next(); ) {
                    remove_production(P);
                }

                // remove productions using this variable
                for(prods_using_var.rewind();
                    prods_using_var.match_next(); ) {

                    remove_production(P);

                    // this variable only generates the variable we are deleting;
                    // remove the variable
                    if(0 == num_productions(V)) {
                        vars_to_remove.push_back(V);
                    }
                }
            }

            unsafe_remove_variable(_var);
        }

        /// does this grammar have this particular terminal?
        bool has_terminal(const alphabet_type term) const throw() {
            return 0 != terminal_map_inv.count(term);
        }

        /// get the terminal reference for a particular terminal.
        const terminal_type get_terminal(const alphabet_type term) throw() {
            typename terminal_map_inv_type::iterator pos(
                terminal_map_inv.find(term)
            );
            cfg::internal_sym_type term_id;

            // add in the terminal
            if(terminal_map_inv.end() == pos) {
                term_id = next_terminal_id;
                --next_terminal_id;
                alphabet_type copy(traits_type::copy(term));
                terminal_map.append(std::make_pair<alphabet_type,const char *>(
                    copy, 0
                ));
                terminal_map_inv[copy] = term_id;

            // return the terminal
            } else {
                term_id = (*pos).second;
            }

            return terminal_type(term_id);
        }

        inline bool has_start_variable(void) const throw() {
            return 0 != start_variable;
        }

        inline bool is_variable_terminal(const terminal_type term) const throw() {
            assert(0 != term.value);
            const unsigned id(static_cast<unsigned>(term.value * -1));
            assert(id < terminal_map.size());
            return 0 != terminal_map.get(id).second;
        }

        /// get the alphabetic value for a terminal
        inline const alphabet_type &get_alpha(const terminal_type term) const throw() {
            assert(0 != term.value);
            const unsigned id(static_cast<unsigned>(term.value * -1));
            assert(id < terminal_map.size());
            assert(0 == terminal_map.get(id).second);
            return terminal_map.get(id).first;
        }

        /// get the name for a variable terminal
        inline const char *get_name(const terminal_type term) const throw() {
            assert(0 != term.value);
            const unsigned id(static_cast<unsigned>(term.value * -1));
            assert(id < terminal_map.size());
            assert(0 != terminal_map.get(id).second);
            return terminal_map.get(id).second;
        }

        /// get the name for a variable. if the variable doesn't have a name
        /// then one is generated automatically.
        inline const char *get_name(const variable_type _var) const throw() {
            cfg::Variable<AlphaT> *var(get_variable(_var));
            if(0 != var->name) {
                return var->name;
            }

            // figure out a name for this variable
            unsigned long prev_ub(strtoul(
                &(auto_symbol_upper_bound[1]),
                0,
                10
            ));

            // make the new name
            char buffer[1024] = {'\0'};
            sprintf(buffer, "$%lu", prev_ub + 1);
            const char *name(trait::Alphabet<const char *>::copy(buffer));

            var->name = name;
            auto_symbol_upper_bound = name;

            return name;
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

            ++num_productions_;
            ++(var->num_productions);

            // if we have a no productions then add this production in
            if(0 == var->first_production) {

                prod->next = 0;
                var->first_production = prod;
                cfg::Production<AlphaT>::hold(prod);

            // no null production; go look for equivalent productions to
            // make sure we don't add a duplicate
            } else {

                cfg::Production<AlphaT> *related_prod(var->first_production);
                cfg::Production<AlphaT> *prev_prod(0);

                // make sure the production is unique
                for(; 0 != related_prod;
                    prev_prod = related_prod,
                    related_prod = related_prod->next) {

                    // we've found our insertion point
                    if(prod->is_less_than(*related_prod)) {

                        prod->next = related_prod;
                        prod->prev = related_prod->prev;
                        related_prod->prev = prod;

                        if(0 == prod->prev) {
                            var->first_production = prod;
                        } else {
                            prod->prev->next = prod;
                        }

                        cfg::Production<AlphaT>::hold(prod);
                        goto done;

                    // insertion sort by hash value
                    } else if(prod->is_greater_than(*related_prod)) {
                        continue;

                    // possible duplicate found
                    } else if(related_prod->is_equivalent_to(*prod)) {

                        if(related_prod->is_deleted) {
                            related_prod->is_deleted = false;
                            cfg::Production<AlphaT>::hold(related_prod);
                        } else {
                            --num_productions_;
                            --(var->num_productions);
                        }

                        production_allocator->deallocate(prod);
                        prod = related_prod;
                        goto done;
                    }
                }

                // add to the end of the list

                assert(0 != prev_prod);
                prev_prod->next = prod;
                prod->prev = prev_prod;
                prod->next = 0;
                cfg::Production<AlphaT>::hold(prod);
            }

        done:

            if(0 == first_production
            || first_production->var->id > var->id
            || (first_production->var->id == var->id
                && prod->is_less_than(*first_production))) {
                first_production = prod;
            }

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
            symbol_buffer_type &builder
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
                (0 != var->first_production) &&
                "The variable is in an invalid state."
            );

            prod->is_deleted = true;

            // go find the next production
            if(first_production == prod) {
                set_next_production(var->id);
            }

            --num_productions_;
            --(var->num_productions);

            cfg::Production<AlphaT>::release(prod);
        }


        /// get the variable representing the empty string, epsilon
        inline const symbol_string_type &epsilon(void) const throw() {
            return mpl::Static<symbol_string_type>::VALUE;
        }

        /// get the number of variables in this CFG
        inline unsigned num_variables(void) const throw() {
            return num_variables_;
        }

        /// the capacity of this CFG
        inline unsigned num_variables_capacity(void) const throw() {
            return static_cast<unsigned>(next_variable_id);
        }

        /// get the number of productions in the CFG
        inline unsigned num_productions(void) const throw() {
            return num_productions_;
        }

        /// get the number of productions related to a variable
        unsigned num_productions(variable_type _var) throw() {
            return get_variable(_var)->num_productions;
        }

        /// get the number of terminals in the CFG; note: not all terminals
        /// are necessarily reachable
        inline unsigned num_terminals(void) const throw() {
            return terminal_map.size() - 1U;
        }

        /// get the number of variable terminals; these are terminals that
        /// are left undefined in the grammar.
        inline unsigned num_variable_terminals(void) const throw() {
            return variable_terminal_map.size();
        }

        /// create a variable generator
        inline generator_type
        search(cfg::Unbound<AlphaT, cfg::variable_tag> sym) const throw() {
            generator_type gen(
                const_cast<self_type *>(this),
                reinterpret_cast<void *>(sym.symbol), // binder
                reinterpret_cast<cfg::detail::PatternData<AlphaT> *>(0), // pattern
                &(cfg::detail::SimpleGenerator<AlphaT>::bind_next_variable),
                &(cfg::detail::SimpleGenerator<AlphaT>::reset_next_variable),
                &(cfg::OpaquePattern<AlphaT>::default_gen_reset)
            );
            return gen;
        }

        /// create a terminal generator
        inline generator_type
        search(cfg::Unbound<AlphaT, cfg::terminal_tag> sym) const throw() {
            generator_type gen(
                const_cast<self_type *>(this),
                reinterpret_cast<void *>(sym.symbol), // binder
                reinterpret_cast<cfg::detail::PatternData<AlphaT> *>(0), // pattern
                &(cfg::detail::SimpleGenerator<AlphaT>::bind_next_terminal),
                &(cfg::detail::SimpleGenerator<AlphaT>::reset_next_terminal),
                &(cfg::OpaquePattern<AlphaT>::default_gen_reset)
            );
            return gen;
        }

        /// create a production generator
        inline generator_type
        search(cfg::Unbound<AlphaT, cfg::production_tag> uprod) const throw() {
            generator_type gen(
                const_cast<self_type *>(this),
                reinterpret_cast<void *>(uprod.prod), // binder
                reinterpret_cast<cfg::detail::PatternData<AlphaT> *>(0), // pattern
                &(cfg::detail::SimpleGenerator<AlphaT>::bind_next_production),
                &(cfg::detail::SimpleGenerator<AlphaT>::reset_next_production),
                &(cfg::detail::SimpleGenerator<AlphaT>::free_next_production)
            );
            return gen;
        }

        /// go find all productions that fit a certain pattern. bind each
        /// production to uprod, and also destructure each production
        /// according to the pattern.
        template <typename PatternT, typename StringT, const unsigned state>
        inline generator_type
        search(
            cfg::Unbound<AlphaT, cfg::production_tag> uprod,
            cfg::detail::PatternBuilder<AlphaT,PatternT,StringT,state> pattern_builder
        ) const throw() {

            typedef cfg::detail::PatternBuilder<AlphaT,PatternT,StringT,state>
                    builder_type;

            generator_type gen(
                const_cast<self_type *>(this),
                reinterpret_cast<void *>(uprod.prod), // binder
                pattern_builder.pattern, // pattern
                &(cfg::detail::PatternGenerator<
                    AlphaT,
                    builder_type
                >::bind_next_pattern),
                &(cfg::detail::PatternGenerator<
                    AlphaT,
                    builder_type
                >::reset_next_pattern),
                &(cfg::detail::SimpleGenerator<AlphaT>::free_next_production)
            );

            return gen;
        }

        /// go find all productions that fit a certain pattern. destructure
        /// each production according to the pattern.
        template <typename PatternT, typename StringT, const unsigned state>
        inline generator_type
        search(
            cfg::detail::PatternBuilder<AlphaT,PatternT,StringT,state> pattern_builder
        ) const throw() {

            typedef cfg::detail::PatternBuilder<AlphaT,PatternT,StringT,state>
                    builder_type;

            generator_type gen(
                const_cast<self_type *>(this),
                reinterpret_cast<void *>(0), // binder
                pattern_builder.pattern, // pattern
                &(cfg::detail::PatternGenerator<
                    AlphaT,
                    builder_type
                >::bind_next_pattern),
                &(cfg::detail::PatternGenerator<
                    AlphaT,
                    builder_type
                >::reset_next_pattern),
                &(cfg::detail::SimpleGenerator<AlphaT>::free_next_production)
            );

            return gen;
        }

        /// go find all productions that fit a certain pattern. destructure
        /// each production according to the pattern.
        inline generator_type search(pattern_type &pattern) const throw() {
            generator_type gen(
                const_cast<self_type *>(this),
                reinterpret_cast<void *>(0), // binder
                pattern.pattern, // pattern
                pattern.gen_next,
                pattern.gen_reset,
                pattern.gen_free
            );
            return gen;
        }

        /// go find all productions that fit a certain pattern. bind each
        /// production to uprod, and also destructure each production
        /// according to the pattern.
        inline generator_type search(
            cfg::Unbound<AlphaT, cfg::production_tag> uprod,
            pattern_type &pattern
        ) const throw() {
            generator_type gen(
                const_cast<self_type *>(this),
                reinterpret_cast<void *>(uprod.prod), // binder
                pattern.pattern, // pattern
                pattern.gen_next,
                pattern.gen_reset,
                pattern.gen_free
            );
            return gen;
        }

    private:

        /// go find the next variable in some direction
        cfg::Variable<AlphaT> *find_variable(
            const cfg::internal_sym_type id,
            const cfg::internal_sym_type increment
        ) throw() {
            cfg::Variable<AlphaT> *next(0);
            for(cfg::internal_sym_type i(id + increment);
                i < next_variable_id && 0 < i;
                i += increment) {

                next = variable_map.get(static_cast<unsigned>(i));
                if(0 != next) {
                    break;
                }
            }
            return next;
        }

        /// go find and set the next production
        void set_next_production(const cfg::internal_sym_type id) throw() {
            cfg::Production<AlphaT> *next(0);
            cfg::Variable<AlphaT> *next_var(0);

            for(cfg::internal_sym_type i(id);
                i < next_variable_id;
                ++i) {

                next_var = variable_map.get(static_cast<unsigned>(i));

                if(0 == next_var) {
                    continue;
                }

                for(next = next_var->first_production;
                    0 != next;
                    next = next->next) {

                    if(next->is_deleted) {
                        continue;
                    }

                    goto found_next;
                }
            }

        found_next:

            first_production = next;
        }

        inline cfg::Variable<AlphaT> *
        get_variable(const variable_type _var) const throw() {
            assert(
                0 < _var.value &&
                _var.value < next_variable_id &&
                "Invalid variable passed to get_variable()."
            );

            cfg::Variable<AlphaT> *var(variable_map.get(
                static_cast<unsigned>(_var.value)
            ));

            assert(
                0 != var &&
                "Invalid variable passed to get_variable()."
            );

            return var;
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
}

#include "fltl/include/cfg/ProductionBuilder.hpp"
#include "fltl/include/cfg/SymbolString.hpp"
#include "fltl/include/cfg/OpaqueProduction.hpp"
#include "fltl/include/cfg/Any.hpp"
#include "fltl/include/cfg/Unbound.hpp"
#include "fltl/include/cfg/Generator.hpp"
#include "fltl/include/cfg/Pattern.hpp"
#include "fltl/include/cfg/OpaquePattern.hpp"

#endif /* FLTL_LIB_CONTEXTFREEGRAMMAR_HPP_ */
