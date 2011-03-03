/*
 * Unbound.hpp
 *
 *  Created on: Mar 1, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_UNBOUND_HPP_
#define FLTL_UNBOUND_HPP_

namespace fltl { namespace pda {

    template <typename AlphaT>
    class Unbound<AlphaT,symbol_tag> {
    private:

        friend class Symbol<AlphaT>;
        friend class PDA<AlphaT>;

        Symbol<AlphaT> *symbol;

        Unbound(Symbol<AlphaT> *sym) throw()
            : symbol(sym)
        { }

        Unbound(void) throw() {
            symbol = 0;
        }
    };

    template <typename AlphaT>
    class Unbound<AlphaT,state_tag> {
    private:

        friend class OpaqueState<AlphaT>;
        friend class PDA<AlphaT>;

        OpaqueState<AlphaT> *state;

        Unbound(OpaqueState<AlphaT> *ss) throw()
            : state(ss)
        { }

        Unbound(void) throw() {
            state = 0;
        }
    };

    template <typename AlphaT>
    class Unbound<AlphaT,transition_tag> {
    private:

        friend class OpaqueTransition<AlphaT>;
        friend class PDA<AlphaT>;

        OpaqueTransition<AlphaT> *transition;

        Unbound(OpaqueTransition<AlphaT> *trans) throw()
            : transition(trans)
        { }

        Unbound(void) throw() {
            transition = 0;
        }
    };
}}

#endif /* FLTL_UNBOUND_HPP_ */
