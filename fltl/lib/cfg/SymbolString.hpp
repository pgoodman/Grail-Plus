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

#define FLTL_SYMBOL_STRING_ALLOCATE(n,_) \
    case n: syms = (alloc ## n).allocate()->symbols; break;

#define FLTL_SYMBOL_STRING_DEALLOCATE(n,var) \
    case n: \
        (alloc ## n).deallocate(static_cast<detail::SymbolArray<AlphaT, n> *>( \
            reinterpret_cast<void *>(var) \
        )); \
        break;

#define FLTL_SYMBOL_STRING_INIT_ALLOCATOR(n,_) \
    template <typename AlphaT> \
    helper::ListAllocator< \
        detail::SymbolArray<AlphaT, n>, \
        &detail::SymbolArray<AlphaT, n >::get_next_pointer, \
        32U \
    > SymbolString<AlphaT>::alloc ## n;

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
                return reinterpret_cast<self_type **>(
                    reinterpret_cast<void **>(&(self->symbols[0]))
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
    ///         removed from its grammar. I.e. such symbol strings are
    ///         exempted from reference counting.
    ///       - it is assumed that integers cannot be aligned on 1-byte
    ///         boundaries. we take advantage of this to tag the lowest order
    ///         bit of the pointer to the symbols to know whether or not the
    ///         symbols are owned or whether they belong to a production.
    template <typename AlphaT>
    class SymbolString {
    private:

        friend class Symbol<AlphaT>;
        friend class CFG<AlphaT>;
        friend class OpaqueProduction<AlphaT>;

        typedef SymbolString<AlphaT> self_type;
        typedef Symbol<AlphaT> symbol_type;
        typedef typename CFG<AlphaT>::variable_type variable_type;
        typedef typename CFG<AlphaT>::terminal_type terminal_type;

        /// masks used to distinguish between a string from a production and
        /// a normal string
        static const uintptr_t SYMBOL_PTR_MASK_INV;
        static const uintptr_t SYMBOL_PTR_MASK;

        enum {
            REF_COUNT = 0,
            STRING_LENGTH = 1,
            FIRST_SYMBOL = 2
        };

        /// static allocators for short symbol strings
        FLTL_REPEAT_LEFT(
            FLTL_SYMBOL_STRING_NUM_ALLOCATORS,
            FLTL_SYMBOL_STRING_MAKE_ALLOCATOR,
            void
        )

        /// the symbols of this string
        symbol_type *symbols;

        /// unmask a symbol pointer
        inline static symbol_type *unmask(symbol_type *syms) throw() {
            const uintptr_t masked(reinterpret_cast<uintptr_t>(syms));
            return reinterpret_cast<symbol_type *>(
                masked & SYMBOL_PTR_MASK
            );
        }

        /// allocate a new array of symbols and increase its reference count
        static symbol_type *
        allocate(const unsigned num_symbols) throw() {

            if(0 == num_symbols) {
                return 0;
            }

            symbol_type *syms(0);
            switch(num_symbols) {
                FLTL_REPEAT_LEFT(
                    FLTL_SYMBOL_STRING_NUM_ALLOCATORS,
                    FLTL_SYMBOL_STRING_ALLOCATE,
                    void
                )
                default: syms = new symbol_type[num_symbols]; break;
            }

            incref(syms);
            syms[STRING_LENGTH].value = static_cast<internal_sym_type>(
                num_symbols
            );

            return syms;
        }

        /// increase the reference count on a symbol array
        static void incref(symbol_type *syms) throw() {
            symbol_type *unmasked_syms(unmask(syms));
            if(0 != syms && unmasked_syms == syms) {
                ++(unmasked_syms[REF_COUNT].value);
            }
        }

        /// decrease the reference count on a symbol array
        static void decref(symbol_type *syms) throw() {
            symbol_type *unmasked_syms(unmask(syms));

            // referencing into a production
            if(0 == syms || unmasked_syms != syms) {
                return;
            }

            // don't need to free, symbols being referenced elsewhere.
            if(0 < --(unmasked_syms[REF_COUNT].value)) {
                return;
            }

            switch(unmasked_syms[STRING_LENGTH].value) {
                FLTL_REPEAT_LEFT(
                    FLTL_SYMBOL_STRING_NUM_ALLOCATORS,
                    FLTL_SYMBOL_STRING_DEALLOCATE,
                    unmasked_syms
                )
            default:
                delete [] unmasked_syms;
            }
        }

        /// create a symbol string from a production
        SymbolString(Production<AlphaT> *prod) throw()
            : symbols(reinterpret_cast<symbol_type *>(
                reinterpret_cast<uintptr_t>(&(prod->get(0))) | SYMBOL_PTR_MASK_INV
            ))
        { }

        /// append a symbol onto the end of this string
        inline self_type append_symbol(const symbol_type *sym) const throw() {
            if(0 == sym->value) {
                return *this;
            }

            const unsigned len = length();

            self_type ret;
            ret.symbols = allocate(len + 1U);
            ret.symbols[FIRST_SYMBOL + len] = *sym;

            if(0 != len) {
                memcpy(
                    &(ret.symbols[FIRST_SYMBOL]),
                    &(unmask(symbols)[FIRST_SYMBOL]),
                    sizeof(symbol_type) * len
                );
            }

            return ret;
        }

        /// prepend a symbol onto the beginning of this string
        inline self_type prepend_symbol(const symbol_type *sym) const throw() {
            if(0 == sym->value) {
                return *this;
            }

            const unsigned len = length();

            self_type ret;
            ret.symbols = allocate(len + 1U);
            ret.symbols[FIRST_SYMBOL] = *sym;

            if(0 != len) {
                memcpy(
                    &(ret.symbols[FIRST_SYMBOL + 1U]),
                    &(unmask(symbols)[FIRST_SYMBOL]),
                    sizeof(symbol_type) * len
                );
            }

            return ret;
        }

    public:

        SymbolString(void) throw()
            : symbols(0)
        { }

        /// constructor for a generic symbol -> symbol string
        explicit SymbolString(const symbol_type &sym) throw()
            : symbols(0)
        {
            if(0 != sym.value) {
                symbols = allocate(1U);
                symbols[FIRST_SYMBOL] = sym;
            }
        }

        /// constructor for a variable -> symbol string
        explicit SymbolString(const variable_type &sym) throw()
            : symbols(0)
        {
            if(0 != sym.value) {
                symbols = allocate(1U);
                symbols[FIRST_SYMBOL] = sym;
            }
        }

        /// constructor for a terminal -> symbol string
        explicit SymbolString(const terminal_type &sym) throw()
            : symbols(0)
        {
            if(0 != sym.value) {
                symbols = allocate(1U);
                symbols[FIRST_SYMBOL] = sym;
            }
        }

        /// copy constructor
        SymbolString(const self_type &that) throw()
            : symbols(const_cast<symbol_type *>(that.symbols))
        {
            incref(symbols);
        }

        /// destructor
        ~SymbolString(void) throw() {
            decref(symbols);
            symbols = 0;
        }

        /// assign by reference
        self_type &operator=(self_type &that) throw() {
            if(symbols == that.symbols) {
                return *this;
            }

            decref(symbols);
            symbols = that.symbols;
            incref(symbols);

            return *this;
        }

        /// copy a symbol string by value
        void copy(const self_type &that) throw() {
            decref(symbols);

            if(0 == that.symbols) {

                symbols = 0;

            } else if(symbols != that.symbols) {

                const unsigned str_length(static_cast<unsigned>(
                    that.symbols[STRING_LENGTH].value
                ));

                symbols = allocate(str_length);
                memcpy(
                    &(symbols[FIRST_SYMBOL]),
                    &(unmask(that.symbols)[FIRST_SYMBOL]),
                    str_length * sizeof(symbol_type)
                );
            }
        }

        /// length
        inline unsigned length(void) const throw() {
            if(0 == symbols) {
                return 0U;
            }
            return static_cast<unsigned>(
                unmask(symbols)[STRING_LENGTH].value
            );
        }

        /// concatenation
        inline self_type operator+(const self_type &other) const throw() {

            const unsigned len = length();
            const unsigned other_len = other.length();

            self_type ret;
            ret.symbols = allocate(len + other_len);

            if(0 != len) {
                memcpy(
                    &(ret.symbols[FIRST_SYMBOL]),
                    &(unmask(symbols)[FIRST_SYMBOL]),
                    sizeof(symbol_type) * len
                );
            }

            if(0 != other_len) {
                memcpy(
                    &(ret.symbols[FIRST_SYMBOL + len]),
                    &(unmask(other.symbols)[FIRST_SYMBOL]),
                    sizeof(symbol_type) * other_len
                );
            }

            return ret;
        }

        /// concatenate a symbol onto the end
        inline self_type operator+(const symbol_type &sym) const throw() {
            return append_symbol(&sym);
        }

        inline self_type operator+(const variable_type &sym) const throw() {
            return append_symbol(&sym);
        }

        inline self_type operator+(const terminal_type &sym) const throw() {
            return append_symbol(&sym);
        }

        /// get a slice of symbols. gets a slice of symbols in the range
        /// [from, from+stride[.
        inline self_type
        substring(const unsigned start, const unsigned stride) const throw() {

            self_type ret;
            const int len = length();

            if(0 == len || 0 == stride) {
                return ret;
            }

            assert(
                start < len &&
                "Substring of symbol string starts past end of string."
            );

            assert(
                (start + stride) <= len &&
                "Substring of symbol string is longer than string."
            );

            ret.symbols = allocate(stride);
            memcpy(
                ret.symbols,
                &(unmask(symbols)[start]),
                stride * sizeof(symbol_type)
            );

            return ret;
        }

        /// check for equality of symbol strings
        inline bool operator==(const self_type &that) const throw() {
            if(symbols == that.symbols) {
                return true;
            }

            const symbol_type *this_syms = unmask(symbols);
            const symbol_type *that_syms = unmask(that.symbols);
            const internal_sym_type len(this_syms[STRING_LENGTH].value);

            if(len != that_syms[STRING_LENGTH].value) {
                return false;
            }

            return 0 == memcmp(
                &(this_syms[FIRST_SYMBOL]),
                &(that_syms[FIRST_SYMBOL]),
                static_cast<size_t>(len) * sizeof(symbol_type)
            );
        }

        inline bool operator!=(const self_type &that) const throw() {
            return !(operator==(that));
        }
    };

    template <typename AlphaT>
    const uintptr_t SymbolString<AlphaT>::SYMBOL_PTR_MASK_INV(0x1U);

    template <typename AlphaT>
    const uintptr_t SymbolString<AlphaT>::SYMBOL_PTR_MASK(
        ~SymbolString<AlphaT>::SYMBOL_PTR_MASK_INV
    );

    FLTL_REPEAT_LEFT(
        FLTL_SYMBOL_STRING_NUM_ALLOCATORS,
        FLTL_SYMBOL_STRING_INIT_ALLOCATOR,
        void
    )
}}}

#endif /* FLTL_SYMBOLSTRING_HPP_ */
