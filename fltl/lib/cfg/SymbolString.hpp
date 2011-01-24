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
#define FLTL_SYMBOL_STRING_ALLOC_LIST_SIZE 64U
#define FLTL_SYMBOL_STRING_MAKE_ALLOCATOR(n, _) \
    static helper::StorageChain<helper::ListAllocator< \
        detail::SymbolArray<AlphaT, n>, \
        &detail::SymbolArray< \
            AlphaT, \
            n \
        >::get_next_pointer, \
        FLTL_SYMBOL_STRING_ALLOC_LIST_SIZE \
    > > alloc ## n ;

#define FLTL_SYMBOL_STRING_ALLOCATE(n,_) \
    case n: syms = (alloc ## n)->allocate()->symbols; break;

#define FLTL_SYMBOL_STRING_DEALLOCATE(n,var) \
    case n: \
        (alloc ## n)->deallocate(static_cast<detail::SymbolArray<AlphaT, n> *>( \
            reinterpret_cast<void *>(var) \
        )); \
        break;

#define FLTL_SYMBOL_STRING_INIT_ALLOCATOR(n,alloc_var) \
    template <typename AlphaT> \
    helper::StorageChain<helper::ListAllocator< \
        detail::SymbolArray<AlphaT, n>, \
        &detail::SymbolArray<AlphaT, n >::get_next_pointer, \
        FLTL_SYMBOL_STRING_ALLOC_LIST_SIZE \
    > > SymbolString<AlphaT>::alloc ## n(alloc_var);

namespace fltl { namespace lib { namespace cfg {

    namespace detail {

        template <typename AlphaT, const unsigned num_symbols>
        struct SymbolArray {
        public:

            typedef SymbolArray<AlphaT, num_symbols> self_type;

            // make sure that there's enough storage to hold a pointer; this
            // should be captured by the +3 that accounts for the ref count,
            // hash, and string lengths.
            enum {
                LEN = num_symbols + 3,
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
                return helper::unsafe_cast<self_type **>(
                    helper::align<16>(&(self->symbols[0]))
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
        friend class Production<AlphaT>;
        friend class OpaqueProduction<AlphaT>;
        friend class ProductionBuilder<AlphaT>;

        typedef SymbolString<AlphaT> self_type;
        typedef Symbol<AlphaT> symbol_type;

        enum {
            REF_COUNT = 0,
            HASH = 1,
            STRING_LENGTH = 2,
            FIRST_SYMBOL = 3
        };

        /// static allocators for short symbol strings
        FLTL_REPEAT_LEFT(
            FLTL_SYMBOL_STRING_NUM_ALLOCATORS,
            FLTL_SYMBOL_STRING_MAKE_ALLOCATOR,
            void
        )

        /// the symbols of this string
        symbol_type *symbols;

        /// allocate a new array of symbols and increase its reference count
        static symbol_type *
        allocate(const unsigned num_symbols) throw() {

            if(0 == num_symbols) {
                return 0;
            }

            // allocate
            symbol_type *syms(0);
            switch(num_symbols) {
                FLTL_REPEAT_LEFT(
                    FLTL_SYMBOL_STRING_NUM_ALLOCATORS,
                    FLTL_SYMBOL_STRING_ALLOCATE,
                    void
                )
                default:
                    syms = new symbol_type[num_symbols + FIRST_SYMBOL];
                    break;
            }

            // initialize
            incref(syms);
            syms[STRING_LENGTH].value = static_cast<internal_sym_type>(
                num_symbols
            );

            return syms;
        }

        /// increase the reference count on a symbol array
        static void incref(symbol_type *syms) throw() {
            if(0 != syms) {
                ++(syms[REF_COUNT].value);
            }
        }

        /// decrease the reference count on a symbol array
        static void decref(symbol_type *syms) throw() {
            // referencing into a production
            if(0 == syms) {
                return;
            }

            // don't need to free, symbols being referenced elsewhere.
            if(0 < --(syms[REF_COUNT].value)) {
                return;
            }

            switch(syms[STRING_LENGTH].value) {
                FLTL_REPEAT_LEFT(
                    FLTL_SYMBOL_STRING_NUM_ALLOCATORS,
                    FLTL_SYMBOL_STRING_DEALLOCATE,
                    syms
                )
            default:
                delete [] syms;
            }
        }

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
                    &(symbols[FIRST_SYMBOL]),
                    sizeof(symbol_type) * len
                );

                ret.symbols[HASH].value = hash(
                    symbols[HASH].value,
                    sym->randomize()
                );
            } else {
                ret.symbols[HASH].value = sym->randomize();
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
                    &(symbols[FIRST_SYMBOL]),
                    sizeof(symbol_type) * len
                );

                ret.symbols[HASH].value = hash(
                    symbols[HASH].value,
                    sym->randomize()
                );
            } else {
                ret.symbols[HASH].value = sym->randomize();
            }

            return ret;
        }

        /// very simple commutative hash function in the group Z_3931
        FLTL_FORCE_INLINE static internal_sym_type hash(
            const internal_sym_type a,
            const internal_sym_type b
        ) throw() {
            return (a * b) % 3931;
        }

        FLTL_FORCE_INLINE internal_sym_type get_hash(void) const throw() {
            if(0 == symbols) {
                return static_cast<internal_sym_type>(2180083513U);
            }
            return symbols[HASH].value;
        }

        /// hash an array
        inline static internal_sym_type hash_array(
            symbol_type *syms,
            const unsigned num_syms
        ) throw() {
            internal_sym_type ihash(syms->randomize());
            for(symbol_type *sym(syms + 1), *last(syms + num_syms);
                sym < last;
                ++sym) {

                ihash = hash(ihash, sym->randomize());
            }
            return ihash;
        }

        /// create a symbol string from an array of symbols
        SymbolString(symbol_type *arr, const unsigned num_syms) throw()
            : symbols(0)
        {
            if(0 < num_syms) {
                symbols = allocate(num_syms);
                memcpy(
                    &(symbols[FIRST_SYMBOL]),
                    arr,
                    sizeof(symbol_type) * num_syms
                );
                symbols[HASH].value = hash_array(arr, num_syms);
            }
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
                symbols[HASH].value = sym.randomize();
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

        /// clear out this symbol string
        void clear(void) throw() {
            decref(symbols);
            symbols = 0;
        }

        /// assign by reference contained in value
        void assign(const self_type that) throw() {
            if(symbols != that.symbols) {
                decref(symbols);
                symbols = const_cast<symbol_type *>(that.symbols);
                incref(symbols);
            }
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
                symbols[HASH] = that.symbols[HASH];
                memcpy(
                    &(symbols[FIRST_SYMBOL]),
                    &(that.symbols[FIRST_SYMBOL]),
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
                symbols[STRING_LENGTH].value
            );
        }

        /// concatenation
        inline self_type operator+(const self_type &that) const throw() {

            const unsigned len = length();
            const unsigned other_len = that.length();

            self_type ret;
            ret.symbols = allocate(len + other_len);
            if(0 != ret.symbols) {

                internal_sym_type lhash(1);
                internal_sym_type rhash(1);

                if(0 != len) {
                    memcpy(
                        &(ret.symbols[FIRST_SYMBOL]),
                        &(symbols[FIRST_SYMBOL]),
                        sizeof(symbol_type) * len
                    );
                    lhash = symbols[HASH].value;
                }

                if(0 != other_len) {
                    memcpy(
                        &(ret.symbols[FIRST_SYMBOL + len]),
                        &(that.symbols[FIRST_SYMBOL]),
                        sizeof(symbol_type) * other_len
                    );
                    rhash = that.symbols[HASH].value;
                }

                ret.symbols[HASH].value = hash(lhash, rhash);
            }

            return ret;
        }

        /// concatenate a symbol onto the end
        inline self_type operator+(const symbol_type &sym) const throw() {
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
                &(symbols[FIRST_SYMBOL + start]),
                stride * sizeof(symbol_type)
            );

            // hash the substring
            ret.symbols[HASH].value = hash_array(
                ret.symbols[FIRST_SYMBOL],
                stride
            );

            return ret;
        }

        /// check for equality of symbol strings
        inline bool operator==(const self_type &that) const throw() {
            if(symbols == that.symbols) {
                return true;
            }

            const symbol_type *this_syms(symbols);
            const symbol_type *that_syms(that.symbols);
            const internal_sym_type len(this_syms[STRING_LENGTH].value);

            if(len != that_syms[STRING_LENGTH].value
            || symbols[HASH].value != that_syms[HASH].value) {
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

        /// get a the symbol at a specific index
        inline const symbol_type &at(const unsigned offset) const throw() {
            assert(
                0 != symbols &&
                "Cannot access symbol in empty symbol string."
            );

            assert(
                offset < static_cast<unsigned>(symbols[STRING_LENGTH].value) &&
                "Symbol string index out of bounds."
            );

            return symbols[
                FIRST_SYMBOL + static_cast<unsigned>(
                    symbols[STRING_LENGTH].value
                )
            ];
        }

        FLTL_FORCE_INLINE const symbol_type &
        operator[](const unsigned offset) const throw() {
            return at(offset);
        }

        FLTL_FORCE_INLINE const bool is_empty(void) const throw() {
            return 0 == symbols;
        }
    };

    FLTL_REPEAT_LEFT(
        FLTL_SYMBOL_STRING_NUM_ALLOCATORS,
        FLTL_SYMBOL_STRING_INIT_ALLOCATOR,
        CFG<AlphaT>::production_allocator
    )
}}}

#endif /* FLTL_SYMBOLSTRING_HPP_ */
