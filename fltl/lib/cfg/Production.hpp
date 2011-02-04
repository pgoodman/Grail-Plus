/*
 * Production.hpp
 *
 *  Created on: Jan 20, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_PRODUCTION_HPP_
#define FLTL_PRODUCTION_HPP_

namespace fltl { namespace lib { namespace cfg {

    // forward declarations
    template <typename, const unsigned short> class StaticProduction;
    template <typename> class DynamicProduction;

    /// production of a grammar
    ///
    /// Note: - the first symbol in a production is a reference counter
    ///         and as such cannot be treated as a normal symbol. this is
    ///         so that, from a string point of view, symbol strings and
    ///         production symbol lists are equivalent.
    ///
    ///       - the second symbol is the length of the production.
    template <typename AlphaT>
    class Production {
    private:

        friend class CFG<AlphaT>;
        friend class Variable<AlphaT>;
        friend class OpaqueProduction<AlphaT>;
        friend class detail::SimpleGenerator<AlphaT>;
        template <typename, typename> friend class detail::PatternGenerator;

        typedef Production<AlphaT> self_type;

        /// productions chain in to a doubly-linked list
        self_type *prev;
        self_type *next;

        /// variable of this production
        Variable<AlphaT> *var;

        /// symbols of this production
        SymbolString<AlphaT> symbols;

        /// reference counter
        uint32_t ref_count;

        /// was this production deleted?
        bool is_deleted;

        /// get the number of symbols in this production
        inline unsigned length(void) const throw() {
            return symbols.length();
        }

        /// check equivalence of two productions
        bool is_equivalent_to(const self_type &that) const throw() {
            if(this == &that) {
                return true;
            }

            return (var == that.var) && (symbols == that.symbols);
        }

        inline static void hold(self_type *prod) throw() {
            assert(
                0 != prod &&
                "Cannot hold non-existant production."
            );

            //printf("HOLD(%p)\n", reinterpret_cast<void *>(prod));

            ++(prod->ref_count);
        }

        inline static void release(self_type *prod) throw() {
            assert(
                0 != prod &&
                "Cannot release non-existant production."
            );

            assert(
                0 != prod->ref_count &&
                "Cannot release invalid production."
            );

            // time to clean up; unchain them
            if(0 == --(prod->ref_count)) {

                //printf("RELEASE!(%p)\n", reinterpret_cast<void *>(prod));

                if(0 != prod->prev) {
                    //printf("... SET-NEXT(%p,%p)\n", reinterpret_cast<void *>(prod->prev), reinterpret_cast<void *>(prod->next));
                    prod->prev->next = prod->next;
                }

                if(0 != prod->next) {
                    //printf("... SET-PREV(%p,%p)\n", reinterpret_cast<void *>(prod->next), reinterpret_cast<void *>(prod->prev));
                    prod->next->prev = prod->prev;
                }

                prod->next = 0;
                prod->prev = 0;
                prod->symbols.clear();
                CFG<AlphaT>::production_allocator->deallocate(prod);
                prod = 0;



            }
            //else {
            //    printf("RELEASE(%p)\n", reinterpret_cast<void *>(prod));
            //}
        }

    public:

        /// constructor
        Production(void) throw()
            : prev(0)
            , next(0)
            , var()
            , symbols()
            , ref_count(0)
            , is_deleted(false)
        { }

        Production(const self_type &) throw()
            : prev(0)
            , next(0)
            , var()
            , symbols()
            , ref_count(0)
            , is_deleted(false)
        {
            assert(false);
        }

        /// destructor
        ~Production(void) throw() {
            prev = 0;
            next = 0;
            is_deleted = true;
        }

        self_type &operator=(const self_type &) throw() {
            assert(false);
            return *this;
        }
    };

}}}

#endif /* FLTL_PRODUCTION_HPP_ */
