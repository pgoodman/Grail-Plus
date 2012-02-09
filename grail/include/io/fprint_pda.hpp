/*
 * fprint_pda.hpp
 *
 *  Created on: Mar 2, 2011
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

#ifndef FLTL_FPRINT_PDA_HPP_
#define FLTL_FPRINT_PDA_HPP_

#include <cstdio>

#include "fltl/include/PDA.hpp"

#include "grail/include/io/fprint.hpp"

namespace grail { namespace io {

    template <typename AlphaT>
    static int fprint_symbol(
        FILE *ff,
        const fltl::PDA<AlphaT> &pda,
        typename fltl::PDA<AlphaT>::symbol_type sym
    ) throw() {
        int num(0);
        if(pda.is_in_input_alphabet(sym)) {

            num += fprintf(ff, "\"");
            num += io::fprint(ff, pda.get_alpha(sym));
            num += fprintf(ff, "\"");
        } else {
            num += fprintf(ff, "%s", pda.get_name(sym));
        }
        return num;
    }

    /// print out either a state's symbolic name or its number if no name
    /// exists
    template <typename AlphaT>
    static char *sprint_state(
        char *buffer,
        const fltl::PDA<AlphaT> &pda,
        typename fltl::PDA<AlphaT>::state_type state
    ) throw() {
        const char *state_name(pda.get_name(state));
        if('\0' != state_name[0]) {
            sprintf(buffer, "@%s", state_name);
        } else {
            sprintf(buffer, "%u", state.number());
        }
        return buffer;
    }

    /// print out a context-free grammar
    template <typename AlphaT>
    int fprint(FILE *ff, const fltl::PDA<AlphaT> &pda) throw() {
        char state_buffer[1024] = {'\0'};

        FLTL_PDA_USE_TYPES(fltl::PDA<AlphaT>);

        int num(0);
        if(0 == pda.num_accept_states()) {
            return num;
        }

        // start state
        state_type state(pda.get_start_state());
        num += fprintf(ff, "(START) |- %s\n", sprint_state(state_buffer, pda, state));

        // accept states
        generator_type accept_states(pda.search(~state));
        for(; accept_states.match_next(); ) {
            if(pda.is_accept_state(state)) {
                num += fprintf(ff, "%s -| (FINAL)\n", sprint_state(state_buffer, pda, state));
            }
        }

        // transitions
        transition_type trans;
        generator_type transitions(pda.search(~trans));
        for(; transitions.match_next(); ) {
            num += fprintf(ff, "%s ", sprint_state(state_buffer, pda, trans.source()));
            num += fprint_symbol(ff, pda, trans.read());
            num += fprintf(ff, " %s, ", sprint_state(state_buffer, pda, trans.sink()));
            num += fprint_symbol(ff, pda, trans.pop());
            num += fprintf(ff, " / ");
            num += fprint_symbol(ff, pda, trans.push());
            num += fprintf(ff, "\n");
        }

        num += fprintf(ff, "\n");
        return num;
    }

}}

#endif /* FLTL_FPRINT_PDA_HPP_ */
