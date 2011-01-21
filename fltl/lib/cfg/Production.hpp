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

#include <cassert>
#include <stdint.h>

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
    /// the first element in a production is its variable.
    template <typename AlphaT>
    class Production {
    protected:

        friend class CFG<AlphaT>;
        friend class Variable<AlphaT>;

        typedef Production<AlphaT> self_type;

        /// next production related to the same variable as this one
        self_type *next;

        /// hash of this production; makes for quicker comparison
        uint32_t hash;

        /// compute the hash of a production
        /// TODO: should probably replace this with a real hash function
        static uint32_t get_hash(
            const unsigned num_symbols,
            Symbol<AlphaT> *symbols
        ) throw() {

            uint32_t h(0x85ebca6b);
            Symbol<AlphaT> *curr(symbols);
            Symbol<AlphaT> *last_sym(symbols + num_symbols);

            for(int32_t i(0); curr != last_sym; ++curr, ++i) {
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
            : next(0)
            , hash(0)
        { }

        virtual ~Production(void) throw() {
            hash = 0U;
            next = 0;
        }

        /// get the number of symbols in this production, less the 1 symbol
        /// that is the variable that this production is related to
        virtual unsigned length(void) const throw() = 0;

        virtual const Symbol<AlphaT> &
        get(const unsigned) const throw() = 0;

        /// index one of the symbols.
        inline virtual const Symbol<AlphaT> &
        operator[](const unsigned offset) const throw() {
            return get(offset);
        }

        /// check equivalence of two productions
        inline virtual bool
        operator==(const self_type &that) const throw() {
            if(hash != that.hash) {
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
    };

    /// a production where all of the symbols of the production are contained
    /// within the production; this is used for small productions
    template <typename AlphaT, const unsigned short num_symbols>
    class StaticProduction : public Production<AlphaT> {
    private:

        Symbol<AlphaT> symbols[num_symbols];

    public:

        StaticProduction(helper::Array<Symbol<AlphaT> > &symbol_buffer) throw()
            : Production<AlphaT>()
        {
            for(unsigned i(0); i < num_symbols; ++i) {
                symbols[i] = symbol_buffer[i];
            }

            this->hash = Production<AlphaT>::get_hash(num_symbols, &(symbols[0]));
        }

        virtual ~StaticProduction() throw() { }

        virtual unsigned length(void) const throw() {
            return static_cast<unsigned>(num_symbols) - 1U;
        }

        virtual const Symbol<AlphaT> &
        get(const unsigned offset) const throw() {
            assert(offset < num_symbols && "Access to symbol in production is out of bounds.");
            return symbols[offset];
        }
    };

    /// a production where all of the symbols of the production are heap
    /// allocated. this is for larger productions.
    template <typename AlphaT>
    class DynamicProduction : public Production<AlphaT> {
    private:

        Symbol<AlphaT> *symbols;
        const unsigned num_symbols;

    public:

        DynamicProduction(helper::Array<Symbol<AlphaT> > &symbol_buffer) throw()
            : Production<AlphaT>()
            , symbols(new Symbol<AlphaT>[symbol_buffer.size()])
            , num_symbols(static_cast<unsigned>(symbol_buffer.size()))
        {
            for(unsigned i(0); i < num_symbols; ++i) {
                symbols[i] = symbol_buffer[i];
            }

            this->hash = Production<AlphaT>::get_hash(num_symbols, symbols);
        }

        virtual ~DynamicProduction(void) throw() {
            delete [] symbols;
            symbols = 0;
        }

        virtual unsigned length(void) const throw() {
            return num_symbols - 1U;
        }

        virtual const Symbol<AlphaT> &
        get(const unsigned offset) const throw() {
            assert(0 != symbols && "Invalid use of already freed production.");
            assert(offset < num_symbols && "Access to symbol in production is out of bounds.");
            return symbols[offset];
        }

    };
}}}

#endif /* FLTL_PRODUCTION_HPP_ */
