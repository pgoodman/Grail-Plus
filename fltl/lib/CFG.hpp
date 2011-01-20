/*
 * ContextFreeGrammar.hpp
 *
 *  Created on: Sep 19, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef FLTL_LIB_CONTEXTFREEGRAMMAR_HPP_
#define FLTL_LIB_CONTEXTFREEGRAMMAR_HPP_

#include <map>
#include <utility>

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

        // forward declaration
        template <typename AlphaT>
        class Variable;

        // forward declaration
        template <typename AlphaT>
        class Production;

        /// grammar symbol (non-terminal or terminal)
        template <typename AlphaT>
        class Symbol {
        protected:
            int value;

            friend class CFG<AlphaT>;
            friend class Variable<AlphaT>;
            friend class Production<AlphaT>;

            explicit Symbol(const int v) throw()
                : value(v)
            { }

        public:

            Symbol(void) throw()
                : value(0)
            { }

            Symbol(const Symbol<AlphaT> &that) throw()
                : value(that.value)
            { }

            Symbol<AlphaT> &operator=(const Symbol<AlphaT> &that) throw() {
                value = that.value;
                return *this;
            }
        };

        /// grammar variable
        template <typename AlphaT>
        class Variable {
        private:

            int id;
            Variable<AlphaT> *next;

        public:

            Variable(void)
                : id(0)
                , next(0)
            { }

            /// initialize the variable
            void init(const int _id, Variable<AlphaT> *prev) {
                id = _id;
                if(0 != prev) {
                    next = prev->next;
                    prev->next = this;
                } else {
                    next = 0;
                }
            }

            /// get a pointer to the next pointer of this variable. used by
            /// the list allocator for variables in the grammar.
            inline static Variable<AlphaT> **
            get_next_pointer(Variable<AlphaT> *var) {
                return &(var->next);
            }
        };

        /// production in the grammar
        /// the first element in a production is its variable.
        template <typename AlphaT>
        class Production {
        private:
            friend class CFG<AlphaT>;
            friend class Variable<AlphaT>;
        };
    }

    /// context-free grammar type.
    ///
    /// Assumptions:
    ///     - AlphaT has a strict weak ordering.
    ///     - AlphaT is default constructible
    ///     - AlphaT is copy constructible
    template <typename AlphaT>
    class CFG : protected trait::Uncopyable {
    private:

        template <typename, typename> friend class QueryBuilder;
        friend class cfg::Variable<AlphaT>;

        /// the next variable id that can be assigned, goes toward +inf
        int next_variable_id;

        /// the next terminal id that can be assigned, goes toward -inf
        int next_terminal_id;

        /// injective mapping between non-zero negative integers and pointers
        /// to the parameterized alphabet type. the association between
        /// terminals and their representations needs to be maintained.
        helper::Array<AlphaT> terminal_map;
        std::map<AlphaT, int> terminal_map_inv;

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

        /// types to be used by non-FLTL code for manipulating a CFG
        class variable_type : public cfg::Symbol<AlphaT> { };
        class terminal_type : public cfg::Symbol<AlphaT> { };

        CFG(void) throw()
            : trait::Uncopyable()
            , next_variable_id(0)
            , next_terminal_id(0)
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

        /// add a new variable to the
        variable_type addVariable(void) throw() {
            cfg::Variable<AlphaT> *var(variable_allocator.allocate());
            var->init(++next_variable_id, variable_map.back());
            variable_map.append(var);

            return *static_cast<variable_type *>(
                reinterpret_cast<void *>(&next_variable_id)
            );
        }

        /// get the terminal reference for a particular terminal.
        terminal_type getTerminal(const AlphaT term) throw() {
            int &id(terminal_map_inv[term]);

            if(0 == id) {
                id = --next_terminal_id;
                terminal_map.append(term);
            }

            return *static_cast<terminal_type *>(
                reinterpret_cast<void *>(&id)
            );
        }
    };
}}

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
