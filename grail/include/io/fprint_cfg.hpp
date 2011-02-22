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

#include "fltl/lib/CFG.hpp"

#include "grail/include/io/fprint.hpp"

namespace grail { namespace io {

    template <typename AlphaT>
    static void fprint_production(
        FILE *ff,
        const fltl::lib::CFG<AlphaT> &cfg,
        std::map<typename fltl::lib::CFG<AlphaT>::var_t,unsigned> &var_ids,
        typename fltl::lib::CFG<AlphaT>::sym_str_t str,
        const char *prefix,
        unsigned &next_var_id
    ) throw() {

        fprintf(ff, "  %s", prefix);
        typename fltl::lib::CFG<AlphaT>::sym_t s;

        for(unsigned i(0); i < str.length(); ++i) {
            s = str.at(i);

            if(s.is_variable()) {
                typename fltl::lib::CFG<AlphaT>::var_t s_as_var(s);
                unsigned &id(var_ids[s_as_var]);

                // not set
                if(0 == id) {
                    id = ++next_var_id;
                }

                fprintf(ff, " %u", id);
            } else {
                typename fltl::lib::CFG<AlphaT>::term_t s_as_term(s);
                fprintf(ff, " ");
                fprint(ff, cfg.get_alpha(s_as_term));
            }
        }

        fprintf(ff, "\n");
    }

    /// print out a context-free grammar
    template <typename AlphaT>
    void fprint(FILE *ff, const fltl::lib::CFG<AlphaT> &cfg) throw() {

        using fltl::lib::CFG;

        // nothing to print
        if(0 == cfg.num_productions()) {
            return;
        }

        typename CFG<AlphaT>::var_t SV(cfg.get_start_variable());
        typename CFG<AlphaT>::var_t V(SV);
        typename CFG<AlphaT>::prod_t P;
        typename CFG<AlphaT>::sym_str_t S;

        // the grammar generates the empty language
        if(cfg.has_default_production(SV)) {
            return;
        }

        // print the start variable first
        typename CFG<AlphaT>::generator_t productions(cfg.search(
            V --->* ~S
        ));

        unsigned next_var_id(1);
        std::map<typename CFG<AlphaT>::var_t,unsigned> var_ids;
        var_ids[SV] = 1;

        const char sep[] = {':', '\0', '|', '\0'};

        fprintf(ff, "1\n");
        for(unsigned sep_offset(0); productions.match_next(); sep_offset = 2) {
            fprint_production(
                ff,
                cfg,
                var_ids,
                S,
                &(sep[sep_offset]),
                next_var_id
            );
        }
        fprintf(ff, "  ;\n");

        // print the start variable first
        typename CFG<AlphaT>::generator_t all_vars(cfg.search(~V));

        for(; all_vars.match_next(); ) {
            if(V == SV) {
                continue;
            }

            unsigned &id(var_ids[V]);
            if(0 == id) {
                id = ++next_var_id;
            }

            fprintf(ff, "%u\n", id);
            productions.rewind();
            for(unsigned sep_offset(0); productions.match_next(); sep_offset = 2) {
                fprint_production(
                    ff,
                    cfg,
                    var_ids,
                    S,
                    &(sep[sep_offset]),
                    next_var_id
                );
            }
            fprintf(ff, "  ;\n");
        }
        fprintf(ff, "\n");
    }
}}

#endif /* FLTL_CFG_PRINTER_HPP_ */
