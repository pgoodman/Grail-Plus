/*
 * OperatorString.hpp
 *
 *  Created on: May 11, 2012
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef Grail_Plus_TDOP_OPERATORSTRING_HPP_
#define Grail_Plus_TDOP_OPERATORSTRING_HPP_

// number of allocators, should be max string length
#define FLTL_OPERATOR_STRING_NUM_ALLOCATORS 32

// number of symbol arrays to allocate per block in the symbol array list
// allocators
#define FLTL_OPERATOR_STRING_ALLOC_LIST_SIZE 64U

// element in a statically initialized array for a de/allocator
#define FLTL_OPERATOR_STRING_INIT_FUNC(n, func) \
    , &detail::OperatorArrayImpl<AlphaT,n>::func

namespace fltl { namespace tdop {

    namespace detail {

        template <typename AlphaT>
        class OperatorArray {
        public:
            uint32_t ref_count;
            uint32_t length;
            Operator<AlphaT> *operators;
            // ignored :-P
        };

        /// used for short operator arrays that will be managed by the block
        /// allocator
        template <typename AlphaT, const unsigned num_ops>
        class OperatorArrayImpl {
        public:

            typedef OperatorArrayImpl<AlphaT, num_ops> self_type;
            typedef OperatorArray<AlphaT> dest_type;

            static helper::BlockAllocator<
                self_type,
                FLTL_OPERATOR_STRING_ALLOC_LIST_SIZE
            > allocator;

            // roughly equivalent to operator array; we get to use the same
            // 'operators' pointer interface, but it will point to the array
            // that immediately follows ('operator_').
            uint32_t ref_count;
            uint32_t length;
            Operator<AlphaT> *operators;
            Operator<AlphaT> operators_[num_ops];

            OperatorArrayImpl(void) throw()
                : ref_count(1U)
                , length(num_ops)
                , operators(&(operators_[0]))
            { }

            static dest_type *allocate(const unsigned len) throw() {
                return helper::unsafe_cast<dest_type *>(allocator.allocate());
            }

            static void deallocate(OperatorArray<AlphaT> *ptr) throw() {
                assert(0 != ptr);
                allocator.deallocate(helper::unsafe_cast<self_type>(ptr));
            }
        };

        // static initialization of block allocator for short operator arrays
        template <typename AlphaT, const unsigned num_ops>
        helper::BlockAllocator<
            OperatorArrayImpl<AlphaT, num_ops>,
            FLTL_OPERATOR_STRING_ALLOC_LIST_SIZE
        > OperatorArrayImpl<AlphaT,num_ops>::allocator;

        /// used for long operator arrays; operator's array is heap allocated
        template <typename AlphaT>
        class OperatorArrayImpl<AlphaT, 0> {
        public:

            typedef OperatorArrayImpl<AlphaT, 0> self_type;
            typedef OperatorArray<AlphaT> dest_type;

            uint32_t ref_count;
            uint32_t length;
            Operator<AlphaT> *operators;

            OperatorArrayImpl(unsigned len, Operator<AlphaT> *ops) throw()
                : ref_count(1U)
                , length(len)
                , operators(ops)
            { }

            static dest_type *allocate(const unsigned len) throw() {
                return helper::unsafe_cast<OperatorArray<AlphaT> *>(
                    new self_type(len, new Operator<AlphaT>[len])
                );
            }

            static void deallocate(dest_type *ptr_) throw() {
                self_type *ptr(helper::unsafe_cast<self_type *>(ptr_));

                assert(0U != ptr);
                assert(0U == ptr->ref_count);

                delete [] ptr->operators;
                ptr->operators = 0;
                ptr->length = 0;
                delete ptr;
            }
        };
    }

    /// represents an operator string
    template <typename AlphaT>
    class OperatorString {
    private:

        friend class Term<AlphaT>;
        friend class Symbol<AlphaT>;
        friend class Operator<AlphaT>;


        FLTL_TDOP_USE_TYPES(TDOP<AlphaT>);

        typedef detail::OperatorArray<AlphaT> array_type;
        typedef array_type *(allocator_func_type)(unsigned);
        typedef array_type *(deallocator_func_type)(void);

        array_type *arr;

        static allocator_func_type *allocators[FLTL_OPERATOR_STRING_NUM_ALLOCATORS];
        static deallocator_func_type *deallocators[FLTL_OPERATOR_STRING_NUM_ALLOCATORS];

        /// allocate a new operator string of a certain length
        static array_type *allocate(unsigned len) throw() {
            if(0 == len) {
                return 0;
            }

            if(FLTL_OPERATOR_STRING_NUM_ALLOCATORS <= len) {
                return allocators[0](len);
            } else {
                return allocators[len](len);
            }
        }

        /// de-allocate an operator string
        static void deallocate(array_type *ptr) throw() {
            assert(0 != ptr);

            if(FLTL_OPERATOR_STRING_NUM_ALLOCATORS <= ptr->length) {
                deallocators[0](ptr);
            } else {
                deallocators[ptr->length](ptr);
            }
            ptr = 0;
        }

        static void incref(array_type *ptr) throw() {
            if(0 != ptr) {
                ++(ptr->ref_count);
            }
        }

        static void decref(array_type *ptr) throw() {
            if(0 != ptr) {
                ptr->ref_count -= 1U;
                if(0U == ptr->ref_count) {
                    deallocate(ptr);
                }
            }
        }

        static uint32_t append(array_type *arr, const array_type *other_arr, uint32_t offset) throw() {
            if(0 == other_arr) {
                return offset;
            }

            for(uint32_t i(0); i < other_arr->length; ++offset, ++i) {
                arr->operators[offset] = other_arr->operators[i];
            }

            return offset;
        }

        static uint32_t append(array_type *arr, const operator_type op, uint32_t offset) throw() {
            arr->operators[offset] = op;
            return offset + 1;
        }

    public:

        /// constructors
        OperatorString(void) throw()
            : arr(0)
        { }

        OperatorString(const operator_string_type &that) throw()
            : arr(that.arr)
        {
            incref(arr);
        }

        /// assignment
        operator_string_type &operator=(const operator_string_type &that) throw() {
            if(&that == this) {
                return *this;
            }

            decref(arr);
            arr = const_cast<array_type *>(that.arr);
            incref(arr);

            return *this;
        }

        /// destructor
        ~OperatorString(void) throw() {
            decref(arr);
            arr = 0;
        }

        /// access, properties
        uint32_t length(void) const throw() {
            if(0 == arr) {
                return 0U;
            } else {
                return arr->length;
            }
        }

        operator_type at(const size_t index) const throw() {
            assert(0 != arr);
            assert(index < arr->length);
            return arr->operators[index];
        }

        FLTL_FORCE_INLINE
        operator_type operator[](const size_t index) const throw() {
            return at(index);
        }

        /// concatenation

        operator_string_type operator+(const operator_string_type &that) const throw() {
            const uint32_t new_len(length() + that.length());
            array_type *new_arr(allocate(new_len));
            OperatorString str;
            str.arr = new_arr;
            append(new_arr, that.arr, append(new_arr, arr, 0));
            return str;
        }

        operator_string_type operator+(const operator_type &that) const throw() {
            const uint32_t new_len(length() + 1U);
            array_type *new_arr(allocate(new_len));
            OperatorString str;
            str.arr = new_arr;
            append(new_arr, that, append(new_arr, arr, 0));
            return str;
        }

        /// patterns

        const Unbound<operator_string_type> operator~(void) throw() {
            // TODO
        }
    };

    // static initialization for operator array allocator functions
    template <typename AlphaT>
    typename OperatorString<AlphaT>::allocator_func_type *
    OperatorString<AlphaT>::allocators[] = {
        &detail::OperatorArrayImpl<AlphaT,0U>::allocate
        FLTL_REPEAT_LEFT(
            FLTL_OPERATOR_STRING_NUM_ALLOCATORS,
            FLTL_OPERATOR_STRING_INIT_FUNC,
            allocate
        )
    };

    template <typename AlphaT>
    typename OperatorString<AlphaT>::deallocator_func_type *
    OperatorString<AlphaT>::deallocators[] = {
        &detail::OperatorArrayImpl<AlphaT,0U>::deallocate
        FLTL_REPEAT_LEFT(
            FLTL_OPERATOR_STRING_NUM_ALLOCATORS,
            FLTL_OPERATOR_STRING_INIT_FUNC,
            deallocate
        )
    };

}}


#endif /* Grail_Plus_TDOP_OPERATORSTRING_HPP_ */
