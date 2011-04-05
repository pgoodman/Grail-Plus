/*
 * SymbolString.hpp
 *
 *  Created on: Jan 21, 2011
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

#ifndef FLTL_SYMBOLSTRING_HPP_
#define FLTL_SYMBOLSTRING_HPP_

// number of allocators, should be max string length
#define FLTL_SYMBOL_STRING_NUM_ALLOCATORS 32

// number of symbol arrays to allocate per block in the symbol array list
// allocators
#define FLTL_SYMBOL_STRING_ALLOC_LIST_SIZE 64U

// element in a statically initialized array for a de/allocator
#define FLTL_SYMBOL_STRING_INIT_FUNC(n, func) \
    , &detail::SymbolArray<AlphaT,n>::func

namespace fltl { namespace cfg {

    // forward declaration
    template <typename AlphaT>
    class SymbolString;

    namespace detail {

        template <typename AlphaT, const unsigned num_symbols>
        class SymbolArray {
        public:

            typedef SymbolArray<AlphaT, num_symbols> self_type;

            // make sure that there's enough storage to hold a pointer; this
            // should be captured by the +3 that accounts for the ref count,
            // hash, and string lengths.
            enum {
                LEN = num_symbols + str::FIRST_SYMBOL,
                NUM_SLOTS = (
                    ((LEN * sizeof(unsigned)) < sizeof(self_type *))
                        ? (sizeof(self_type *) / sizeof(unsigned))
                        : LEN
                )
            };

            Symbol<AlphaT> symbols[NUM_SLOTS];

            static Symbol<AlphaT> *allocate(const unsigned) throw() {
                return SymbolStringAllocator<
                    AlphaT,num_symbols
                >::allocator->allocate()->symbols;
            }

            static void deallocate(Symbol<AlphaT> *ptr) throw() {
                SymbolStringAllocator<
                    AlphaT,num_symbols
                >::allocator->deallocate(
                    helper::unsafe_cast<SymbolArray<AlphaT, num_symbols> *>(
                        ptr
                    )
                );
            }

            SymbolArray(void)
                : symbols()
            { }
        };

        /// static allocator for symbol arrays
        template <typename AlphaT, const unsigned num_symbols>
        class SymbolStringAllocator {
        public:
            static helper::StorageChain<helper::BlockAllocator<
                SymbolArray<AlphaT, num_symbols>,
                FLTL_SYMBOL_STRING_ALLOC_LIST_SIZE
            > > allocator;
        };

        /// static initialize the symbol array's allocator
        template <typename AlphaT, const unsigned num_symbols>
        helper::StorageChain<helper::BlockAllocator<
            SymbolArray<AlphaT, num_symbols>,
            FLTL_SYMBOL_STRING_ALLOC_LIST_SIZE
        > > SymbolStringAllocator<AlphaT, num_symbols>::allocator(
            CFG<AlphaT>::production_allocator
        );

        /// symbol array of size zero, i.e. flexible symbol array
        template <typename AlphaT>
        class SymbolArray<AlphaT, 0U> {
        public:

            static Symbol<AlphaT> *
            allocate(const unsigned num_symbols) throw() {
                return new Symbol<AlphaT>[num_symbols + str::FIRST_SYMBOL];
            }

            static void deallocate(Symbol<AlphaT> *ptr) throw() {
                delete [] ptr;
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
        friend class detail::PatternData<AlphaT>;

        template <typename, typename> friend class Unbound;

        template <typename,const unsigned> friend class detail::SymbolArray;

        template <typename, typename, const unsigned, typename, typename>
        friend class detail::Match2;

        template <typename, typename, typename,const unsigned>
        friend class detail::DecRefCounts;

        template <typename, typename, typename, typename>
        friend class mpl::OpBinaryPlus;

        template <typename, typename, typename, typename>
        friend class mpl::OpBinaryEq;

        template <typename, typename, typename, typename>
        friend class mpl::OpBinaryNotEq;

        typedef SymbolString<AlphaT> self_type;
        typedef Symbol<AlphaT> symbol_type;

        typedef symbol_type *(allocator_func_type)(const unsigned);
        typedef void (deallocator_func_type)(symbol_type *);

        /// allocator jump table
        static allocator_func_type *allocators[];

        /// deallocator jump table
        static deallocator_func_type *deallocators[];

        /// epsilon symbol; pre-compute the hash of the epsilon symbol
        static Symbol<AlphaT> EPSILON;
        static internal_sym_type EPSILON_HASH;

        /// the symbols of this string
        mutable symbol_type *symbols;

        /// allocate a new array of symbols and increase its reference count
        static symbol_type *
        allocate(const unsigned num_symbols) throw() {

            if(0 == num_symbols) {
                return 0;
            }

            // allocate

            symbol_type *syms(0);

            if(num_symbols > FLTL_SYMBOL_STRING_NUM_ALLOCATORS) {
                syms = allocators[0](num_symbols);
            } else {
                syms = allocators[
                    num_symbols % (FLTL_SYMBOL_STRING_NUM_ALLOCATORS + 1)
                ](num_symbols);
            }

            // initialize
            syms[str::REF_COUNT].value = 1;
            syms[str::LENGTH].value = static_cast<internal_sym_type>(
                num_symbols
            );

            return syms;
        }

        /// increase the reference count on a symbol array
        static void incref(symbol_type *syms) throw() {
            if(0 != syms) {
                ++(syms[str::REF_COUNT].value);
            }
        }

        /// decrease the reference count on a symbol array
        static void decref(symbol_type *syms) throw() {
            // referencing into a production
            if(0 == syms) {
                return;
            }

            // don't need to free, symbols being referenced elsewhere.
            if(0 < --(syms[str::REF_COUNT].value)) {
                return;
            }

            // free
            const unsigned len(static_cast<unsigned>(syms[str::LENGTH].value));
            if(len > FLTL_SYMBOL_STRING_NUM_ALLOCATORS) {
                deallocators[0](syms);
            } else {
                deallocators[
                    len % (FLTL_SYMBOL_STRING_NUM_ALLOCATORS + 1)
                ](syms);
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
            ret.symbols[str::FIRST_SYMBOL + len] = *sym;

            if(0 != len) {
                memcpy(
                    &(ret.symbols[str::FIRST_SYMBOL]),
                    &(symbols[str::FIRST_SYMBOL]),
                    sizeof(symbol_type) * len
                );

                ret.symbols[str::HASH].value = hash(
                    symbols[str::HASH].value,
                    sym->hash()
                );
            } else {
                ret.symbols[str::HASH].value = sym->hash();
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
            ret.symbols[str::FIRST_SYMBOL] = *sym;

            if(0 != len) {
                memcpy(
                    &(ret.symbols[str::FIRST_SYMBOL + 1U]),
                    &(symbols[str::FIRST_SYMBOL]),
                    sizeof(symbol_type) * len
                );

                ret.symbols[str::HASH].value = hash(
                    symbols[str::HASH].value,
                    sym->hash()
                );
            } else {
                ret.symbols[str::HASH].value = sym->hash();
            }

            return ret;
        }

        /// very simple associative, commutative hash function
        FLTL_FORCE_INLINE static internal_sym_type hash(
            const internal_sym_type a,
            const internal_sym_type b
        ) throw() {
            return a * b;
            //return symbol_type::mix32(a * b);
            //return (a * b) % 3931;
            /*return symbol_type::mix32(
            //    ((b & 0xFF0000FF) | (a & 0x00FFFF00)) *
                ((a & 0x0FFF000F) | (b & 0xF000FFF0))
            );*/
        }

        /// get the hash of this symbol
        FLTL_FORCE_INLINE internal_sym_type get_hash(void) const throw() {
            if(0 == symbols) {
                return EPSILON_HASH;
            }
            return symbols[str::HASH].value;
        }

        /// hash an array
        inline static internal_sym_type hash_array(
            const symbol_type *syms,
            const unsigned num_syms
        ) throw() {
            internal_sym_type ihash(syms->hash());
            for(const symbol_type *sym(syms + 1), *last(syms + num_syms);
                sym < last;
                ++sym) {

                ihash = hash(ihash, sym->hash());
            }
            return ihash;
        }

        /// works even if symbol_type has a virtual destructor, whereas
        /// memcpy gives memory errors
        inline static bool symbol_memcmp(
            const symbol_type *first_a,
            const symbol_type *first_b,
            const symbol_type *last_a
        ) throw() {
            for(;
                first_a <= last_a && *first_a == *first_b;
                ++first_a, ++first_b) { }
            return first_a > last_a;
        }

        /// create a symbol string from an array of symbols
        SymbolString(const symbol_type *arr, const unsigned num_syms) throw()
            : symbols(0)
        {

            if(0 < num_syms) {
                symbols = allocate(num_syms);
                memcpy(
                    &(symbols[str::FIRST_SYMBOL]),
                    arr,
                    sizeof(symbol_type) * num_syms
                );
                symbols[str::HASH].value = hash_array(arr, num_syms);
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
                symbols[str::FIRST_SYMBOL] = sym;
                symbols[str::HASH].value = sym.hash();
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
        self_type &operator=(const self_type &that) throw() {
            if(symbols == that.symbols) {
                return *this;
            }

            decref(symbols);
            symbols = const_cast<symbol_type *>(that.symbols);
            incref(symbols);

            return *this;
        }

        self_type &operator=(const symbol_type sym) throw() {
            decref(symbols);
            symbols = 0;
            if(0 != sym.value) {
                symbols = allocate(1U);
                symbols[str::FIRST_SYMBOL] = sym;
                symbols[str::HASH].value = sym.hash();
            }
            return *this;
        }

        /// copy a symbol string by value
        void copy(const self_type &that) throw() {
            decref(symbols);

            if(0 == that.symbols) {

                symbols = 0;

            } else if(symbols != that.symbols) {

                const unsigned str_length(static_cast<unsigned>(
                    that.symbols[str::LENGTH].value
                ));

                symbols = allocate(str_length);
                symbols[str::HASH] = that.symbols[str::HASH];
                memcpy(
                    &(symbols[str::FIRST_SYMBOL]),
                    &(that.symbols[str::FIRST_SYMBOL]),
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
                symbols[str::LENGTH].value
            );
        }

        /// concatenation
        inline const self_type
        operator+(const self_type that) const throw() {

            const unsigned len = length();
            const unsigned other_len = that.length();

            self_type ret;
            ret.symbols = allocate(len + other_len);
            if(0 != ret.symbols) {

                internal_sym_type lhash(1);
                internal_sym_type rhash(1);

                if(0 != len) {
                    memcpy(
                        &(ret.symbols[str::FIRST_SYMBOL]),
                        &(symbols[str::FIRST_SYMBOL]),
                        sizeof(symbol_type) * len
                    );
                    lhash = symbols[str::HASH].value;
                }

                if(0 != other_len) {
                    memcpy(
                        &(ret.symbols[str::FIRST_SYMBOL + len]),
                        &(that.symbols[str::FIRST_SYMBOL]),
                        sizeof(symbol_type) * other_len
                    );
                    rhash = that.symbols[str::HASH].value;
                }

                ret.symbols[str::HASH].value = hash(lhash, rhash);
            }

            return ret;
        }

        /// concatenate a symbol onto the end
        inline const self_type
        operator+(const symbol_type sym) const throw() {
            return append_symbol(&sym);
        }

        /// get a slice of symbols. gets a slice of symbols in the range
        /// [from, from+stride[.
        inline const self_type
        substring(const unsigned start, const unsigned stride) const throw() {

            self_type ret;
            const unsigned len = length();

            assert(
                start < len &&
                "Substring of symbol string starts past end of string."
            );

            if(0 == len || 0 == stride) {
                return ret;
            }

            assert(
                (start + stride) <= len &&
                "Substring of symbol string is longer than string."
            );

            if(len == stride) {
                return *this;
            }

            ret.symbols = allocate(stride);
            memcpy(
                &(ret.symbols[str::FIRST_SYMBOL]),
                &(symbols[str::FIRST_SYMBOL + start]),
                stride * sizeof(symbol_type)
            );

            // hash the substring
            ret.symbols[str::HASH].value = hash_array(
                &(ret.symbols[str::FIRST_SYMBOL]),
                stride
            );

            return ret;
        }

        /// check for equality of symbol strings
        inline bool operator==(const self_type &that) const throw() {
            if(symbols == that.symbols) {
                return true;
            }

            const unsigned this_len(length());
            const unsigned that_len(that.length());

            if(this_len != that_len) {
                return false;

            // both are the empty string
            } else if(0 == this_len) {
                return true;
            }

            const symbol_type *this_syms(symbols);
            const symbol_type *that_syms(that.symbols);

            if(symbols[str::HASH].value != that_syms[str::HASH].value) {
                return false;
            }

            if(symbol_memcmp(
                &(this_syms[str::FIRST_SYMBOL]),
                &(that_syms[str::FIRST_SYMBOL]),

                // make sure to compute the address of the last symbol without
                // actually accessing it
                &(this_syms[str::LENGTH + this_len])
            )) {
                // TODO: this might be overkill
                if(this_syms[str::REF_COUNT].value
                 < that_syms[str::REF_COUNT].value) {
                    decref(const_cast<symbol_type *>(this_syms));
                    symbols = const_cast<symbol_type *>(that_syms);
                    incref(const_cast<symbol_type *>(that_syms));
                } else {
                    decref(const_cast<symbol_type *>(that_syms));
                    that.symbols = const_cast<symbol_type *>(this_syms);
                    incref(const_cast<symbol_type *>(this_syms));
                }

                return true;
            }
            return false;
        }

        inline bool operator!=(const self_type &that) const throw() {
            return !(operator==(that));
        }

        inline bool operator==(const symbol_type &that) const throw() {
            return (
                length() == that.length() && that.hash() == get_hash()
            );
        }

        inline bool operator!=(const symbol_type &that) const throw() {
            return (
                length() != that.length() || that.hash() != get_hash()
            );
        }

        /// get a the symbol at a specific index
        inline const symbol_type &
        at(const unsigned offset) const throw() {
            assert(
                0 != symbols &&
                "Cannot access symbol in empty symbol string."
            );

            assert(
                offset < static_cast<unsigned>(symbols[str::LENGTH].value) &&
                "Symbol string index out of bounds."
            );

            return symbols[str::FIRST_SYMBOL + offset];
        }

        FLTL_FORCE_INLINE const symbol_type &
        operator[](const unsigned offset) const throw() {
            return at(offset);
        }

        FLTL_FORCE_INLINE bool is_empty(void) const throw() {
            return 0 == symbols;
        }

        /// return an "unbound" version of this symbol string
        cfg::Unbound<AlphaT,symbol_string_tag> operator~(void) throw() {
            return cfg::Unbound<AlphaT,symbol_string_tag>(this);
        }
    };

    template <typename AlphaT>
    Symbol<AlphaT> SymbolString<AlphaT>::EPSILON;

    template <typename AlphaT>
    internal_sym_type SymbolString<AlphaT>::EPSILON_HASH(EPSILON.hash());

    template <typename AlphaT>
    typename SymbolString<AlphaT>::allocator_func_type *
    SymbolString<AlphaT>::allocators[] = {
        &detail::SymbolArray<AlphaT,0U>::allocate
        FLTL_REPEAT_LEFT(
            FLTL_SYMBOL_STRING_NUM_ALLOCATORS,
            FLTL_SYMBOL_STRING_INIT_FUNC,
            allocate
        )
    };

    template <typename AlphaT>
    typename SymbolString<AlphaT>::deallocator_func_type *
    SymbolString<AlphaT>::deallocators[] = {
        &detail::SymbolArray<AlphaT,0U>::deallocate
        FLTL_REPEAT_LEFT(
            FLTL_SYMBOL_STRING_NUM_ALLOCATORS,
            FLTL_SYMBOL_STRING_INIT_FUNC,
            deallocate
        )
    };

}}

#endif /* FLTL_SYMBOLSTRING_HPP_ */
