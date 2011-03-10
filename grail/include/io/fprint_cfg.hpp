/*
 * CFGPrinter.hpp
 *
 *  Created on: Feb 19, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_CFG_PRINTER_HPP_
#define FLTL_CFG_PRINTER_HPP_

#include <cstdio>
#include <map>

#include "fltl/include/CFG.hpp"

#include "grail/include/io/fprint.hpp"

#include "grail/include/algorithm/CFG_REMOVE_USELESS.hpp"

namespace grail { namespace io {

    template <typename AlphaT>
    static int fprint_production(
        FILE *ff,
        const fltl::CFG<AlphaT> &cfg,
        typename fltl::CFG<AlphaT>::sym_str_t str,
        const char *prefix
    ) throw() {
        int num(0);

        num += fprintf(ff, "  %s", prefix);
        typename fltl::CFG<AlphaT>::sym_t s;

        unsigned len(str.length());

        if(0 == len) {
            num += fprintf(ff, " epsilon");
        } else {
            for(unsigned i(0); i < len; ++i) {
                s = str.at(i);

                if(s.is_variable()) {
                    typename fltl::CFG<AlphaT>::var_t s_as_var(s);
                    num += fprintf(ff, " %s", cfg.get_name(s_as_var));
                } else {
                    typename fltl::CFG<AlphaT>::term_t s_as_term(s);
                    if(!cfg.is_variable_terminal(s_as_term)) {
                        num += fprintf(ff, " \"");
                        num += fprint(ff, cfg.get_alpha(s_as_term));
                        num += fprintf(ff, "\"");
                    } else {
                        num += fprintf(ff," %s", cfg.get_name(s_as_term));
                    }
                }
            }
        }

        num += fprintf(ff, "\n");
        return num;
    }

    /// print out a context-free grammar
    template <typename AlphaT>
    int fprint(FILE *ff, fltl::CFG<AlphaT> &cfg) throw() {
        int num(0);

        using fltl::CFG;

        algorithm::CFG_REMOVE_USELESS<AlphaT>::run(cfg);

        // nothing to print
        if(0 == cfg.num_productions() || !cfg.has_start_variable()) {
            return num;
        }

        typename CFG<AlphaT>::var_t SV(cfg.get_start_variable());
        typename CFG<AlphaT>::var_t V(SV);
        typename CFG<AlphaT>::prod_t P;
        typename CFG<AlphaT>::sym_str_t S;

        // print the start variable first
        typename CFG<AlphaT>::generator_t productions(cfg.search(
            V --->* ~S
        ));

        const char sep[] = {':', '\0', '|', '\0'};

        num += fprintf(ff, "%s\n", cfg.get_name(SV));
        for(unsigned sep_offset(0); productions.match_next(); sep_offset = 2) {
            num += fprint_production(
                ff,
                cfg,
                S,
                &(sep[sep_offset])
            );
        }
        num += fprintf(ff, "  ;\n");

        // print the start variable first
        typename CFG<AlphaT>::generator_t all_vars(cfg.search(~V));

        for(; all_vars.match_next(); ) {
            if(V == SV) {
                continue;
            }

            num += fprintf(ff, "%s\n", cfg.get_name(V));
            productions.rewind();
            for(unsigned sep_offset(0); productions.match_next(); sep_offset = 2) {
                num += fprint_production(
                    ff,
                    cfg,
                    S,
                    &(sep[sep_offset])
                );
            }
            num += fprintf(ff, "  ;\n");
        }
        num += fprintf(ff, "\n");

        return num;
    }
}}

#endif /* FLTL_CFG_PRINTER_HPP_ */
