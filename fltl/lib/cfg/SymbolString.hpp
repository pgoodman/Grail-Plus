/*
 * SymbolString.hpp
 *
 *  Created on: Jan 21, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_SYMBOLSTRING_HPP_
#define FLTL_SYMBOLSTRING_HPP_

#define FLTL_SYMBOL_STRING_NUM_ALLOCATORS 32
#define FLTL_SYMBOL_STRING_MAKE_ALLOCATOR(n, _) \
    static helper::ListAllocator< \
        detail::SymbolArray<AlphaT, n>, \
        &detail::SymbolArray<AlphaT, \
        n \
    >::get_next_pointer, 32U> alloc ## n ;

namespace fltl { namespace lib { namespace cfg {

    namespace detail {

        template <typename AlphaT, const unsigned num_symbols>
        struct SymbolArray {
        public:

            typedef SymbolArray<AlphaT, num_symbols> self_type;

            // make sure that there's enough storage to hold a pointer; this
            // should be captured by the +2 that accounts for the ref count
            // and length of the string, but maybe in the future when this
            // code is lost and forgotten, pointers will be wider than
            // 64 bits.
            enum {
                LEN = num_symbols + 2,
                NUM_SLOTS = (
                    ((LEN * sizeof(unsigned)) < sizeof(self_type *))
                        ? (sizeof(self_type *) / sizeof(unsigned))
                        : LEN
                )
            };

            Symbol<AlphaT> symbols[NUM_SLOTS];

            // soo evil!
            inline static self_type **
            get_next_pointer(self_type *self) throw() {
                return static_cast<self_type **>(
                    reinterpret_cast<void **>(&(&(self->symbols[0])))
                );
            }
        };
    }

    /// A string of symbols.
    ///
    /// Note: - the first symbol is a reference count
    ///       - the second symbol is the length of the symbol string
    ///       - a symbol string that was constructed using prod.symbols()
    ///         to get all symbols of a production has *undefined* behavior
    ///         if the symbol string is used after the production has been
    ///         removed from its grammar.
    ///       - it is assumed that integers cannot be aligned on 1-byte
    ///         boundaries. we take advantage of this to tag the lowest order
    ///         bit of the pointer to the symbols to know whether or not the
    ///         symbols are owned or whether they belong to a production.
    template <typename AlphaT>
    class SymbolString {
    private:

        static const Symbol<AlphaT> *SYMBOL_PTR_MASK_INV;
        static const Symbol<AlphaT> *SYMBOL_PTR_MASK;

        static const Symbol<AlphaT> *SYMBOL_PTR_MASK_IS_PROD;
        static const Symbol<AlphaT> *SYMBOL_PTR_MASK_IS_ALLOCATOR;

        /// static allocators for symbol strings
        FLTL_REPEAT_LEFT(
            FLTL_SYMBOL_STRING_NUM_ALLOCATORS,
            FLTL_SYMBOL_STRING_MAKE_ALLOCATOR,
            void
        )

        friend class Symbol<AlphaT>;
        friend class CFG<AlphaT>;
        friend class OpaqueProduction<AlphaT>;

        const Symbol<AlphaT> *symbols;

        SymbolString(Production<AlphaT> *prod) throw()
            : symbols(&(prod->get(0)) | SYMBOL_PTR_MASK_INV)
        { }

        SymbolString(const Symbol<AlphaT> *_symbols) throw()
            : symbols(_symbols)
        { }

    public:

        SymbolString(void) throw()
            : symbols(0)
        { }

    };

    template <typename AlphaT>
    const Symbol<AlphaT> *SymbolString<AlphaT>::SYMBOL_PTR_MASK_INV(
        (void *) 0x3
    );

    template <typename AlphaT>
    const Symbol<AlphaT> *SymbolString<AlphaT>::SYMBOL_PTR_MASK(
        ~SymbolString<AlphaT>::SYMBOL_PTR_MASK_INV
    );
}}}

#endif /* FLTL_SYMBOLSTRING_HPP_ */
