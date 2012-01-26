/*
 * CFGPrinter.hpp
 *
 *  Created on: Feb 19, 2011
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

    /// print out a production
    template <typename AlphaT>
    int fprint(FILE *ff, fltl::CFG<AlphaT> &cfg, typename fltl::CFG<AlphaT>::production_type &prod) throw() {
        int num(fprintf(ff, "%s ->", cfg.get_name(prod.variable())));
        const char sep[] = {'\0', '\0'};
        num += fprint_production(
            ff,
            cfg,
            prod.symbols(),
            &(sep[0])
        );
        return num;
    }

    /// print out a context-free grammar
    template <typename AlphaT>
    int fprint(FILE *ff, fltl::CFG<AlphaT> &cfg) throw() {
        int num(0);

        using fltl::CFG;

        algorithm::CFG_REMOVE_USELESS<AlphaT>::run(cfg);

        // nothing to print
        if(0 == cfg.num_productions()
        || !cfg.has_start_variable()) {
            return num;
        }

        typename CFG<AlphaT>::var_t SV(cfg.get_start_variable());
        typename CFG<AlphaT>::var_t V(SV);
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
