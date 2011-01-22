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

#include "fltl/include/helper/Array.hpp"
#include "fltl/include/helper/ListAllocator.hpp"

#include "fltl/include/mpl/If.hpp"
#include "fltl/include/mpl/Query.hpp"
#include "fltl/include/mpl/Static.hpp"

#include "fltl/include/preprocessor/NO_INLINE.hpp"
#include "fltl/include/preprocessor/REPEAT_LEFT.hpp"

#include "fltl/include/trait/Uncopyable.hpp"

/// the class name of a query let'ed variable
#define FLTL_CFG_LET_VAR_CLASS_NAME(var_name) \
    cfg_var__ ## var_name ## __

#define FLTL_CFG_LET_VAR_OPAQUE_CLASS_NAME(var_name) \
    cfg_var__ ## var_name ## __opaque

/// macro for building query variables to be used specifically for
/// CFG queries.
#define FLTL_CFG_LET_(type,var_name) \
    typedef struct FLTL_CFG_LET_VAR_OPAQUE_CLASS_NAME(var_name) *FLTL_CFG_LET_VAR_CLASS_NAME(var_name); \
    fltl::mpl::Expr< \
        type, \
        FLTL_CFG_LET_VAR_CLASS_NAME(var_name) \
    > var_name

/// macros for building (non-)terminal query variables
#define FLTL_CFG_LET_NON_TERM(var_name) FLTL_CFG_LET_(fltl::lib::cfg::query_non_terminal_tag, var_name)
#define FLTL_CFG_LET_TERM(var_name) FLTL_CFG_LET_(fltl::lib::cfg::query_terminal_tag, var_name)

namespace fltl { namespace lib {

    // forward declaration
    template <typename AlphaT>
    class CFG;

    namespace cfg {

        /// type used to represent symbols (terminals, non-terminals)
        /// of a grammar.
        typedef int32_t internal_sym_type;

        /// tag classes for meta-programming
        class query_builder_tag { };
        class query_non_terminal_tag { };
        class query_terminal_tag { };

        // forward declarations
        template <typename> class ProductionBuilder;
        template <typename> class Variable;
        template <typename> class Production;
        template <typename, const unsigned short> class StaticProduction;
        template <typename> class DynamicProduction;
        template <typename> class Symbol;
        template <typename> class OpaqueProduction;
        template <typename> class SymbolString;
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

        /// allocator for variables
        helper::ListAllocator<
            cfg::Variable<AlphaT>,
            &cfg::Variable<AlphaT>::get_next_pointer
        > variable_allocator;

    public:

        /// type tag for specializing
        typedef cfg::query_builder_tag query_builder_tag;

        /// arbitrary symbol (terminal, non-terminal) of a grammar
        typedef cfg::Symbol<AlphaT> symbol_type;

        /// type of a production builder
        typedef cfg::ProductionBuilder<AlphaT> production_builder_type;

        /// represents a terminal of a grammar
        class terminal_type : public cfg::Symbol<AlphaT> {
            friend class CFG<AlphaT>;
        };

        /// represents a non-terminal of a grammar
        class variable_type : public cfg::Symbol<AlphaT> {
        public:
            friend class CFG<AlphaT>;
            variable_type(void) throw()
                : cfg::Symbol<AlphaT>(0)
            { }
        };

        /// represents a production
        typedef cfg::OpaqueProduction<AlphaT> production_type;

        /// string of symbols
        typedef cfg::SymbolString<AlphaT> symbol_string_type;

        /// short forms
        typedef symbol_type sym_t;
        typedef production_builder_type prod_builder_t;
        typedef terminal_type term_t;
        typedef variable_type var_t;
        typedef production_type prod_t;
        typedef symbol_string_type sym_str_t;

        /// constructor
        CFG(void) throw()
            : trait::Uncopyable()
            , next_variable_id(1)
            , next_terminal_id(-1)
            , terminal_map()
            , terminal_map_inv()
            , variable_map()
            , variable_allocator()
        {
            terminal_map.reserve(256U);
            variable_map.reserve(256U);

            terminal_map.append(mpl::Static<AlphaT>::VALUE);
            variable_map.append(0);
        }

        /// destructor
        ~CFG(void) throw() {

            // free the variables
            if(0 < variable_map.size()) {

                for(cfg::Variable<AlphaT> *var(variable_map.get(1U)), *next_var(0);
                    0 != var;
                    var = next_var) {

                    next_var = var->next;
                    variable_map.set(var->id, 0);
                    variable_allocator.deallocate(var);
                }
            }
        }

        /// add a new variable to the
        variable_type add_variable(void) throw() {
            cfg::Variable<AlphaT> *var(variable_allocator.allocate());
            var->init(next_variable_id, variable_map.back());

            cfg::internal_sym_type var_id(next_variable_id);

            ++next_variable_id;
            variable_map.append(var);

            return *static_cast<variable_type *>(
                reinterpret_cast<void *>(&var_id)
            );
        }

        /// get the terminal reference for a particular terminal.
        terminal_type get_terminal(const AlphaT term) throw() {
            cfg::internal_sym_type &term_id(terminal_map_inv[term]);

            if(0 == term_id) {
                term_id = next_terminal_id;
                --next_terminal_id;
                terminal_map.append(term);
            }

            return *static_cast<terminal_type *>(
                reinterpret_cast<void *>(&term_id)
            );
        }

        /// add a production to the grammar
        FLTL_NO_INLINE production_type add_production(
            const variable_type var,
            production_builder_type &builder
        ) throw() {

            assert(
                0 < var.value &&
                var.value <= next_variable_id &&
                "Invalid variable passed to add_production()."
            );

            cfg::Variable<AlphaT> *relation(variable_map.get(var.value));

            assert(
                0 != relation &&
                "Invalid variable passed to add_production()."
            );

            cfg::Production<AlphaT> *prod(allocate_production(
                &(builder.buffer.get(0)),
                builder.buffer.size()
            ));

            // set the variable of the production
            prod->var = var;

            // make sure the production is unique
            for(cfg::Production<AlphaT> *related_prod(relation->productions);
                0 != related_prod;
                related_prod = related_prod->next) {

                if(related_prod->is_equivalent_to(*prod)) {
                    delete prod;
                    return production_type(related_prod);
                }
            }

            // add the production in
            relation->add_production(prod);
            return production_type(prod);
        }

        /// remove a production from the grammar
        void remove_production(production_type &_prod) throw() {
            cfg::Production<AlphaT> *prod(_prod.production);
            cfg::Variable<AlphaT> *var(variable_map.get(prod->get(0).value));

            if(0 == prod->prev) {
                var->productions = prod->next;
            } else {
                prod->prev->next = prod->next;
            }

            if(0 != prod->next) {
                prod->next->prev = prod->prev;
            }

            prod->prev = 0;
            prod->next = 0;

            cfg::Production<AlphaT>::release(prod);
            prod = 0;
        }

        /// get the variable representing the empty string, epsilon
        inline variable_type epsilon(void) const throw() {
            return mpl::Static<variable_type>::VALUE;
        }

    private:

        /// allocate a new production by copying the symbols from some array.
        /// if the production is small then the symbols of the production
        /// are allocated along with the production; otherwise the symbols
        /// are separately heap-allocated.
        FLTL_NO_INLINE static cfg::Production<AlphaT> *
        allocate_production(
            const cfg::Symbol<AlphaT> *symbols,
            const unsigned num_symbols
        ) throw() {
            switch(num_symbols) {
            case 0:
                assert(false && "Production builder is in invalid state.");
            case 1: return new cfg::StaticProduction<AlphaT,1>(symbols);
            case 2: return new cfg::StaticProduction<AlphaT,2>(symbols);
            case 3: return new cfg::StaticProduction<AlphaT,3>(symbols);
            case 4: return new cfg::StaticProduction<AlphaT,4>(symbols);
            case 5: return new cfg::StaticProduction<AlphaT,5>(symbols);
            case 6: return new cfg::StaticProduction<AlphaT,6>(symbols);
            case 7: return new cfg::StaticProduction<AlphaT,7>(symbols);
            case 8: return new cfg::StaticProduction<AlphaT,8>(symbols);
            case 9: return new cfg::StaticProduction<AlphaT,9>(symbols);
            case 10: return new cfg::StaticProduction<AlphaT,10>(symbols);
            case 11: return new cfg::StaticProduction<AlphaT,11>(symbols);
            case 12: return new cfg::StaticProduction<AlphaT,12>(symbols);
            case 13: return new cfg::StaticProduction<AlphaT,13>(symbols);
            case 14: return new cfg::StaticProduction<AlphaT,14>(symbols);
            case 15: return new cfg::StaticProduction<AlphaT,15>(symbols);
            case 16: return new cfg::StaticProduction<AlphaT,16>(symbols);
            case 17: return new cfg::StaticProduction<AlphaT,17>(symbols);
            case 18: return new cfg::StaticProduction<AlphaT,18>(symbols);
            case 19: return new cfg::StaticProduction<AlphaT,19>(symbols);
            case 20: return new cfg::StaticProduction<AlphaT,20>(symbols);
            case 21: return new cfg::StaticProduction<AlphaT,21>(symbols);
            case 22: return new cfg::StaticProduction<AlphaT,22>(symbols);
            case 23: return new cfg::StaticProduction<AlphaT,23>(symbols);
            case 24: return new cfg::StaticProduction<AlphaT,24>(symbols);
            case 25: return new cfg::StaticProduction<AlphaT,25>(symbols);
            case 26: return new cfg::StaticProduction<AlphaT,26>(symbols);
            case 27: return new cfg::StaticProduction<AlphaT,27>(symbols);
            case 28: return new cfg::StaticProduction<AlphaT,28>(symbols);
            case 29: return new cfg::StaticProduction<AlphaT,29>(symbols);
            case 30: return new cfg::StaticProduction<AlphaT,30>(symbols);
            case 31: return new cfg::StaticProduction<AlphaT,31>(symbols);
            case 32: return new cfg::StaticProduction<AlphaT,32>(symbols);
            default:
                return new cfg::DynamicProduction<AlphaT>(symbols, num_symbols);
            }
            return 0;
        }
    };
}}

#include "fltl/lib/cfg/ProductionBuilder.hpp"
#include "fltl/lib/cfg/SymbolString.hpp"
#include "fltl/lib/cfg/OpaqueProduction.hpp"

namespace fltl { namespace mpl {

    /// Classes for template variables.
    namespace detail {
        template <typename TagT, typename>
        class CFGQueryVar { };
    }

    template <typename T>
    class QueryBuilder<T,fltl::lib::cfg::query_builder_tag> {
    public:
    };

}}

#endif /* FLTL_LIB_CONTEXTFREEGRAMMAR_HPP_ */