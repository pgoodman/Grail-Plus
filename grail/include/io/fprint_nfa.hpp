/*
 * fprint_nfa.hpp
 *
 *  Created on: Mar 2, 2012
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

#ifndef FLTL_FPRINT_NFA_HPP_
#define FLTL_FPRINT_NFA_HPP_

#include <cstdio>

#include "fltl/include/NFA.hpp"

#include "grail/include/io/fprint.hpp"

namespace grail { namespace io {

    template <typename AlphaT>
    static int fprint_symbol(
        FILE *ff,
        const fltl::NFA<AlphaT> &nfa,
        typename fltl::NFA<AlphaT>::symbol_type sym
    ) throw() {
        int num(0);
        if(nfa.is_in_input_alphabet(sym)) {

            num += fprintf(ff, "\"");
            num += io::fprint(ff, nfa.get_alpha(sym));
            num += fprintf(ff, "\"");
        } else {
            num += fprintf(ff, "%s", nfa.get_name(sym));
        }
        return num;
    }

    /// print out a context-free grammar
    template <typename AlphaT>
    int fprint(FILE *ff, const fltl::NFA<AlphaT> &nfa) throw() {

        typedef fltl::NFA<AlphaT> NFA;

        int num(0);
        if(0 == nfa.num_accept_states()) {
            return num;
        }

        // start state
        num += fprintf(ff, "(START) |- %u\n", nfa.get_start_state().number());

        // accept states
        typename NFA::state_type state;
        typename NFA::generator_type accept_states(nfa.search(~state));
        for(; accept_states.match_next(); ) {
            if(nfa.is_accept_state(state)) {
                num += fprintf(ff, "%u -| (FINAL)\n", state.number());
            }
        }

        // transitions
        typename NFA::transition_type trans;
        typename NFA::generator_type transitions(nfa.search(~trans));
        for(; transitions.match_next(); ) {
            num += fprintf(ff, "%u ", trans.source().number());
            num += fprint_symbol(ff, nfa, trans.read());
            num += fprintf(ff, " %u ", trans.sink().number());
            num += fprintf(ff, "\n");
        }

        num += fprintf(ff, "\n");
        return num;
    }

}}

#endif /* FLTL_FPRINT_NFA_HPP_ */
