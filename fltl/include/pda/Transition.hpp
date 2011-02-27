/*
 * Transition.hpp
 *
 *  Created on: Feb 26, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_TRANSITION_HPP_
#define FLTL_TRANSITION_HPP_

namespace fltl { namespace pda {

    template <typename AlphaT>
    class Transition {
    private:

        typedef Transition<AlphaT> self_type;
        typedef Symbol<AlphaT> symbol_type;

        friend class PDA<AlphaT>;

        /// the symbol read from input
        symbol_type sym_read;

        /// the symbol that must be on the top of the stack for this
        /// transition to be followed. this symbol is popped off of
        /// the stack.
        symbol_type sym_pop;

        /// symbol to push onto the stack after the transition has been
        /// followed
        symbol_type sym_push;

        /// the next transition that shares the same source state.
        self_type *next;

        /// source and sink states, respectively.
        unsigned source_state;
        unsigned sink_state;
    };

}}

#endif /* FLTL_TRANSITION_HPP_ */
