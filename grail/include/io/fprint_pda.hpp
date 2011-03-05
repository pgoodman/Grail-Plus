/*
 * fprint_pda.hpp
 *
 *  Created on: Mar 2, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_FPRINT_PDA_HPP_
#define FLTL_FPRINT_PDA_HPP_

#include <cstdio>

#include "fltl/include/PDA.hpp"

#include "grail/include/io/fprint.hpp"

namespace grail { namespace io {

    template <typename AlphaT>
    static void fprint_symbol(
        FILE *ff,
        const fltl::PDA<AlphaT> &pda,
        typename fltl::PDA<AlphaT>::symbol_type sym
    ) throw() {
        if(pda.is_in_input_alphabet(sym)) {

            fprintf(ff, "\"");
            io::fprint(ff, pda.get_alpha(sym));
            fprintf(ff, "\"");
        } else {
            fprintf(ff, "%s", pda.get_name(sym));
        }
    }

    /// print out a context-free grammar
    template <typename AlphaT>
    void fprint(FILE *ff, const fltl::PDA<AlphaT> &pda) throw() {

        typedef fltl::PDA<AlphaT> PDA;

        if(0 == pda.num_accept_states()) {
            return;
        }

        // start state
        fprintf(ff, "(START) |- %u\n", pda.get_start_state().number());

        // accept states
        typename PDA::state_type state;
        typename PDA::generator_type accept_states(pda.search(~state));
        for(; accept_states.match_next(); ) {
            if(pda.is_accept_state(state)) {
                fprintf(ff, "%u -| (FINAL)\n", state.number());
            }
        }

        // transitions
        typename PDA::transition_type trans;
        typename PDA::generator_type transitions(pda.search(~trans));
        for(; transitions.match_next(); ) {
            fprintf(ff, "%u ", trans.source().number());
            fprint_symbol(ff, pda, trans.read());
            fprintf(ff, " %u, ", trans.sink().number());
            fprint_symbol(ff, pda, trans.pop());
            fprintf(ff, " / ");
            fprint_symbol(ff, pda, trans.push());
            fprintf(ff, "\n");
        }

        fprintf(ff, "\n");
    }

}}

#endif /* FLTL_FPRINT_PDA_HPP_ */
