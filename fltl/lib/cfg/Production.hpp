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

    namespace detail {
        static const int32_t primes[] = {
            53, 97, 193, 389,
            769, 1543, 3079, 6151,
            12289, 24593, 49157, 98317,
            196613, 393241, 786433, 1572869,
            3145739, 6291469, 12582917, 25165843,
            50331653, 100663319, 201326611, 402653189,
            805306457, 1610612741
        };
    }

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
    protected:

        enum {
            FIRST_SYMBOL_OFFSET = 2U
        };

        friend class CFG<AlphaT>;
        friend class Variable<AlphaT>;
        friend class OpaqueProduction<AlphaT>;

        typedef Production<AlphaT> self_type;

        /// productions chain in to a doubly-linked list
        self_type *prev;
        self_type *next;

        /// hash of this production; makes for quicker comparison
        uint32_t hash;

        /// variable of this production
        typename CFG<AlphaT>::variable_type var;

        /// compute the hash of a production
        /// TODO: should probably replace this with a real hash function
        static uint32_t get_hash(
            const unsigned num_symbols,
            Symbol<AlphaT> *symbols
        ) throw() {

            uint32_t h(0x85ebca6b);
            Symbol<AlphaT> *curr(symbols);
            Symbol<AlphaT> *last_sym(symbols + num_symbols);

            for(int32_t i(1); curr != last_sym; ++curr, ++i) {
                const float val(static_cast<float>(
                    (curr->value * i) * detail::primes[
                        i % (sizeof(detail::primes) / sizeof(int32_t))
                    ]
                ));
                h ^= *static_cast<const uint32_t *>(
                    reinterpret_cast<const void *>(&val)
                );
            }

            return h;
        }

    public:

        Production(void) throw()
            : prev(0)
            , next(0)
            , hash(0)
            , var()
        { }

        virtual ~Production(void) throw() {
            hash = 0U;
            prev = 0;
            next = 0;
        }

        /// extract one of the symbols in the production. note: the first
        /// symbol is actually a reference counter.
        virtual const Symbol<AlphaT> &get(const unsigned) const throw() = 0;

        /// get the number of symbols in this production, less the 1 symbol
        /// that is the variable that this production is related to
        virtual unsigned length(void) const throw() {
            return static_cast<unsigned>(get(1).value);
        }

        /// check equivalence of two productions
        virtual bool is_equivalent_to(const self_type &that) const throw() {
            if(this == &that) {
                return true;
            }

            if(var != that.var || hash != that.hash) {
                return false;
            }

            const unsigned this_len(length());
            const unsigned that_len(that.length());

            if(this_len != that_len) {
                return false;
            }

            for(unsigned i(0); i < this_len; ++i) {
                if(get(i) != that.get(i)) {
                    return false;
                }
            }

            return true;
        }

        inline static void hold(Production<AlphaT> *prod) throw() {
            assert(
                0 != prod &&
                "Cannot hold non-existant production."
            );

            ++(const_cast<Symbol<AlphaT> &>(prod->get(0)).value);
        }

        inline static void release(Production<AlphaT> *prod) throw() {
            assert(
                0 != prod &&
                "Cannot release non-existant production."
            );

            assert(
                0 != prod->get(0).value &&
                "Cannot release invalid production."
            );

            if(0 == --(const_cast<Symbol<AlphaT> &>(prod->get(0)).value)) {
                delete prod;
                prod = 0;
            }
        }
    };

    /// a production where all of the symbols of the production are contained
    /// within the production; this is used for small productions
    template <typename AlphaT, const unsigned short num_symbols>
    class StaticProduction : public Production<AlphaT> {
    private:

        using Production<AlphaT>::FIRST_SYMBOL_OFFSET;

        Symbol<AlphaT> symbols[num_symbols + FIRST_SYMBOL_OFFSET];

    public:

        StaticProduction(const Symbol<AlphaT> *symbol_buffer) throw()
            : Production<AlphaT>()
        {
            memcpy(
                &(symbols[FIRST_SYMBOL_OFFSET]),
                symbol_buffer,
                sizeof(Symbol<AlphaT>) * num_symbols
            );

            symbols[1].value = static_cast<internal_sym_type>(num_symbols);

            this->hash = Production<AlphaT>::get_hash(
                num_symbols,
                &(symbols[FIRST_SYMBOL_OFFSET])
            );
        }

        virtual ~StaticProduction() throw() { }

        virtual const Symbol<AlphaT> &
        get(const unsigned offset) const throw() {
            assert(
                offset < (num_symbols + FIRST_SYMBOL_OFFSET) &&
                "Access to symbol in production is out of bounds."
            );
            return symbols[offset];
        }
    };

    /// a production where all of the symbols of the production are heap
    /// allocated. this is for larger productions.
    template <typename AlphaT>
    class DynamicProduction : public Production<AlphaT> {
    private:

        using Production<AlphaT>::FIRST_SYMBOL_OFFSET;

        Symbol<AlphaT> *symbols;
        const unsigned num_symbols;

    public:

        DynamicProduction(
            const Symbol<AlphaT> *symbol_buffer,
            const unsigned _num_symbols
        ) throw()
            : Production<AlphaT>()
            , symbols(new Symbol<AlphaT>[_num_symbols + FIRST_SYMBOL_OFFSET])
            , num_symbols(_num_symbols)
        {
            memcpy(
                symbols + FIRST_SYMBOL_OFFSET,
                symbol_buffer,
                sizeof(Symbol<AlphaT>) * _num_symbols
            );

            symbols[1].value = static_cast<internal_sym_type>(num_symbols);

            this->hash = Production<AlphaT>::get_hash(
                num_symbols,
                symbols + 2
            );
        }

        virtual ~DynamicProduction(void) throw() {
            delete [] symbols;
            symbols = 0;
        }

        virtual const Symbol<AlphaT> &
        get(const unsigned offset) const throw() {
            assert(
                0 != symbols &&
                "Invalid use of already freed production."
            );
            assert(
                offset < (num_symbols + FIRST_SYMBOL_OFFSET) &&
                "Access to symbol in production is out of bounds."
            );
            return symbols[offset];
        }
    };
}}}

#endif /* FLTL_PRODUCTION_HPP_ */
