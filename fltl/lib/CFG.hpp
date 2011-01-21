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
#include <map>
#include <utility>
#include <stdint.h>

#include "fltl/include/helper/Array.hpp"
#include "fltl/include/helper/ListAllocator.hpp"

#include "fltl/include/mpl/If.hpp"
#include "fltl/include/mpl/Query.hpp"
#include "fltl/include/mpl/Static.hpp"

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

        /// tag classes for meta-programming
        class query_builder_tag { };
        class query_non_terminal_tag { };
        class query_terminal_tag { };

        // forward declarations
        template <typename> class ProductionBuilder;
        template <typename> class Variable;
        template <typename> class Production;
        template <typename> class Symbol;
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
        int32_t next_variable_id;

        /// the next terminal id that can be assigned, goes toward -inf
        int32_t next_terminal_id;

        /// injective mapping between non-zero negative integers and pointers
        /// to the parameterized alphabet type. the association between
        /// terminals and their representations needs to be maintained.
        helper::Array<AlphaT> terminal_map;
        std::map<AlphaT, int32_t> terminal_map_inv;

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
        typedef cfg::Symbol<AlphaT> symbol_type;
        typedef cfg::ProductionBuilder<AlphaT> production_builder_type;

        /// types to be used by non-FLTL code for manipulating a CFG
        class terminal_type : public cfg::Symbol<AlphaT> {
            friend class CFG<AlphaT>;
        };
        class variable_type : public cfg::Symbol<AlphaT> {
        public:
            friend class CFG<AlphaT>;
            variable_type(void) throw()
                : cfg::Symbol<AlphaT>(0)
            { }
        };

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

                for(cfg::Variable<AlphaT> *var(variable_map[1]), *next_var(0);
                    0 != var;
                    var = next_var) {

                    next_var = var->next;
                    variable_map[var->id] = 0;
                    variable_allocator.deallocate(var);
                }
            }
        }

        /// add a new variable to the
        variable_type addVariable(void) throw() {
            variable_map.reserve(next_variable_id + 4);

            cfg::Variable<AlphaT> *var(variable_allocator.allocate());
            var->init(next_variable_id, variable_map.back());

            int32_t var_id(next_variable_id);

            ++next_variable_id;
            variable_map.append(var);

            return *static_cast<variable_type *>(
                reinterpret_cast<void *>(&var_id)
            );
        }

        /// get the terminal reference for a particular terminal.
        terminal_type getTerminal(const AlphaT term) throw() {
            int32_t &term_id(terminal_map_inv[term]);

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
        void addProduction(
            const variable_type var,
            production_builder_type &builder
        ) throw() {

            assert(
                0 < var.value &&
                var.value <= next_variable_id &&
                "Invalid variable passed to addProduction."
            );

            cfg::Variable<AlphaT> *relation(variable_map[var.value]);

            assert(
                0 != relation &&
                "Invalid variable passed to addProduction."
            );

            builder.buffer.set(0U, var);
            cfg::Production<AlphaT> *prod(0);

            // allocate the production
            switch(builder.buffer.size()) {
            case 0:
                assert(false && "Production builder is in invalid state.");
            case 1:
                prod = new cfg::StaticProduction<AlphaT,1>(builder.buffer);
                break;
            case 2:
                prod = new cfg::StaticProduction<AlphaT,2>(builder.buffer);
                break;
            case 3:
                prod = new cfg::StaticProduction<AlphaT,3>(builder.buffer);
                break;
            case 4:
                prod = new cfg::StaticProduction<AlphaT,4>(builder.buffer);
                break;
            case 5:
                prod = new cfg::StaticProduction<AlphaT,5>(builder.buffer);
                break;
            case 6:
                prod = new cfg::StaticProduction<AlphaT,6>(builder.buffer);
                break;
            case 7:
                prod = new cfg::StaticProduction<AlphaT,7>(builder.buffer);
                break;
            case 8:
                prod = new cfg::StaticProduction<AlphaT,8>(builder.buffer);
                break;
            case 9:
                prod = new cfg::StaticProduction<AlphaT,9>(builder.buffer);
                break;
            case 10:
                prod = new cfg::StaticProduction<AlphaT,10>(builder.buffer);
                break;
            case 11:
                prod = new cfg::StaticProduction<AlphaT,11>(builder.buffer);
                break;
            case 12:
                prod = new cfg::StaticProduction<AlphaT,12>(builder.buffer);
                break;
            case 13:
                prod = new cfg::StaticProduction<AlphaT,13>(builder.buffer);
                break;
            case 14:
                prod = new cfg::StaticProduction<AlphaT,14>(builder.buffer);
                break;
            case 15:
                prod = new cfg::StaticProduction<AlphaT,15>(builder.buffer);
                break;
            case 16:
                prod = new cfg::StaticProduction<AlphaT,16>(builder.buffer);
                break;
            default:
                prod = new cfg::DynamicProduction<AlphaT>(builder.buffer);
                break;
            }

            // make sure the production is unique
            for(cfg::Production<AlphaT> *related_prod(relation->productions);
                0 != related_prod;
                related_prod = related_prod->next) {

                if(*related_prod == *prod) {
                    delete prod;
                    return;
                }
            }

            // add the production in
            prod->next = relation->productions;
            relation->productions = prod;
        }

        /// get the variable representing the empty string, epsilon
        inline variable_type epsilon(void) const throw() {
            return mpl::Static<variable_type>::VALUE;
        }
    };
}}

#include "fltl/lib/cfg/ProductionBuilder.hpp"

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
