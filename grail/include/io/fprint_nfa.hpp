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

    /// print out either a state's symbolic name or its number if no name
    /// exists
    template <typename AlphaT>
    static char *sprint_state(
        char *buffer,
        const fltl::NFA<AlphaT> &nfa,
        typename fltl::NFA<AlphaT>::state_type state
    ) throw() {
        const char *state_name(nfa.get_name(state));
        if('\0' != state_name[0]) {
            sprintf(buffer, "@%s", state_name);
        } else {
            sprintf(buffer, "%u", state.number());
        }
        return buffer;
    }

    /// print out a context-free grammar
    template <typename AlphaT>
    int fprint(FILE *ff, const fltl::NFA<AlphaT> &nfa) throw() {
        char state_buffer[1024] = {'\0'};

        FLTL_NFA_USE_TYPES(fltl::NFA<AlphaT>);

        int num(0);
        if(0 == nfa.num_accept_states()) {
            return num;
        }

        // start state
        state_type state(nfa.get_start_state());
        num += fprintf(ff, "(START) |- %s\n", sprint_state(state_buffer, nfa, state));

        // accept states
        generator_type accept_states(nfa.search(~state));
        for(; accept_states.match_next(); ) {
            if(nfa.is_accept_state(state)) {
                num += fprintf(ff, "%s -| (FINAL)\n", sprint_state(state_buffer, nfa, state));
            }
        }

        // transitions
        transition_type trans;
        generator_type transitions(nfa.search(~trans));
        for(; transitions.match_next(); ) {
            num += fprintf(ff, "%s ", sprint_state(state_buffer, nfa, trans.source()));
            num += fprint_symbol(ff, nfa, trans.read());
            num += fprintf(ff, " %s ", sprint_state(state_buffer, nfa, trans.sink()));
            num += fprintf(ff, "\n");
        }

        num += fprintf(ff, "\n");
        return num;
    }

}}

#endif /* FLTL_FPRINT_NFA_HPP_ */
