/*
 * Pattern.hpp
 *
 *  Created on: Mar 3, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_PDA_PATTERN_HPP_
#define FLTL_PDA_PATTERN_HPP_

namespace fltl { namespace pda {

    namespace pattern {

        template <typename AlphaT>
        class Init {
        public:
            FLTL_FORCE_INLINE static void init(
                const Symbol<AlphaT> *source,
                Symbol<AlphaT> **dest
            ) throw() {
                *dest = const_cast<Symbol<AlphaT> *>(source);
            }

            FLTL_FORCE_INLINE static void init(
                const Unbound<Symbol<AlphaT>,symbol_tag> *source,
                Symbol<AlphaT> **dest
            ) throw() {
                *dest = const_cast<Symbol<AlphaT> *>(source->symbol);
            }

            FLTL_FORCE_INLINE static void init(
                const OpaqueState<AlphaT> *source,
                OpaqueState<AlphaT> **dest
            ) throw() {
                *dest = const_cast<OpaqueState<AlphaT> *>(source);
            }

            FLTL_FORCE_INLINE static void init(
                const Unbound<OpaqueState<AlphaT>,state_tag> *source,
                OpaqueState<AlphaT> **dest
            ) throw() {
                *dest = const_cast<OpaqueState<AlphaT> *>(source->state);
            }

            FLTL_FORCE_INLINE static void init(
                const any_state_or_symbol_tag *,
                Symbol<AlphaT> **dest
            ) throw() {
                *dest = 0;
            }

            FLTL_FORCE_INLINE static void init(
                const any_state_or_symbol_tag *,
                OpaqueState<AlphaT> **dest
            ) throw() {
                *dest = 0;
            }
        };

        template <typename AlphaT, typename TagT>
        class Bind;

        template <typename AlphaT>
        class Bind<AlphaT,symbol_tag> {
        public:
            FLTL_FORCE_INLINE static bool bind(
                Symbol<AlphaT> source,
                Symbol<AlphaT> *dest
            ) throw() {
                return 0 == dest || source == *dest;
            }
        };

        template <typename AlphaT>
        class Bind<AlphaT,unbound_symbol_tag> {
        public:
            FLTL_FORCE_INLINE static bool bind(
                Symbol<AlphaT> source,
                Symbol<AlphaT> *dest
            ) throw() {
                if(0 != dest) {
                    *dest = source;
                }
                return true;
            }
        };

        template <typename AlphaT>
        class Bind<AlphaT,state_tag> {
        public:
            FLTL_FORCE_INLINE static bool bind(
                OpaqueState<AlphaT> source,
                OpaqueState<AlphaT> *dest
            ) throw() {
                return 0 == dest || source == *dest;
            }
        };

        template <typename AlphaT>
        class Bind<AlphaT,unbound_state_tag> {
        public:
            FLTL_FORCE_INLINE static bool bind(
                OpaqueState<AlphaT> source,
                OpaqueState<AlphaT> *dest
            ) throw() {
                if(0 != dest) {
                    *dest = source;
                }
                return true;
            }
        };

        template <typename AlphaT>
        class Bind<AlphaT,any_state_or_symbol_tag> {
        public:
            FLTL_FORCE_INLINE static bool bind(
                Symbol<AlphaT>,
                Symbol<AlphaT> *
            ) throw() {
                return true;
            }

            FLTL_FORCE_INLINE static bool bind(
                OpaqueState<AlphaT>,
                OpaqueState<AlphaT> *
            ) throw() {
                return true;
            }
        };
    }

    template <typename AlphaT>
    class Pattern {
    private:

        typedef Pattern<AlphaT> self_type;

        friend class PDA<AlphaT>;
        friend class Generator<AlphaT>;

        template <
            typename, typename,
            typename, typename,
            typename, typename
        > friend class PatternGenerator;

        template <typename, typename>
        friend class detail::ResetPatternGenerator;

        template <typename, typename, typename>
        friend class detail::FindNextTransition;

        OpaqueState<AlphaT> *source;
        OpaqueState<AlphaT> *sink;

        Symbol<AlphaT> *read;
        Symbol<AlphaT> *pop;
        Symbol<AlphaT> *push;

        unsigned ref_count;

        static void hold(self_type *self) throw() {
            assert(0 != self);
            ++(self->ref_count);
        }

        static void release(self_type *self) throw() {
            assert(0 != self);
            if(0 == --(self->ref_count)) {
                PDA<AlphaT>::pattern_allocator->deallocate(self);
            }
        }

    public:

        Pattern(void)
            : source(0)
            , sink(0)
            , read(0)
            , pop(0)
            , push(0)
            , ref_count(0)
        { }
    };

}}

#endif /* FLTL_PDA_PATTERN_HPP_ */
