/*
 * fprint_tdop.hpp
 *
 *  Created on: May 20, 2012
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef Grail_Plus_IO_FPRINT_TDOP_HPP_
#define Grail_Plus_IO_FPRINT_TDOP_HPP_

#include <cstdio>
#include <cassert>

#include "fltl/include/TDOP.hpp"

#include "grail/include/io/fprint.hpp"

namespace grail { namespace io {

    // forward declaration
    template <typename AlphaT, typename ThingT>
    int fprint(FILE *ff, const fltl::TDOP<AlphaT> &cfg, const ThingT thing) throw();

    template <typename AlphaT>
    class fprint_tdop_impl {
    public:

        FLTL_TDOP_USE_TYPES(fltl::TDOP<AlphaT>);

        static int do_print(
            FILE *ff,
            const tdop_type &tdop,
            const symbol_type &sym
        ) throw() {
            int num(0);
            num += fprintf(ff, "\"");
            num += fprint(ff, tdop.get_alpha(sym));
            num += fprintf(ff, "\"");
            return num;
        }

        static int do_print(
            FILE *ff,
            const tdop_type &tdop,
            const category_type cat
        ) throw() {
            return fprintf(ff, "%s", tdop.get_name(cat));
        }

        static int do_print(
            FILE *ff,
            const tdop_type &tdop,
            const term_type term
        ) throw() {
            category_type cat;
            symbol_type sym;
            if(term.match(cat)) {
                return do_print(ff, tdop, cat);
            } else if(term.match(sym)) {
                return do_print(ff, tdop, sym);
            } else {
                return 0;
            }
        }

        static int do_print(
            FILE *ff,
            const tdop_type &tdop,
            const operator_type op
        ) throw() {
            if(op.is_category()) {
                category_type cat;
                unsigned lower_bound(0);

                if(op.match(cat, lower_bound)) {
                    int num(do_print(ff, tdop, cat));
                    num += fprintf(ff, "(%u)", lower_bound);
                    return num;

                } else if(op.match(cat)) {
                    return do_print(ff, tdop, cat);

                } else {
                    assert(false);
                }

            } else if(op.is_symbol()) {
                int num(0);
                if(op.is_symbol_predicate()) {
                    num += fprintf(ff, "&");
                }

                symbol_type sym;
                op.match(sym);

                return num + do_print(ff, tdop, sym);
            }

            return 0;
        }

        /// print out an operator string
        static int do_print(
            FILE *ff,
            const tdop_type &tdop,
            const operator_string_type ops
        ) throw() {
            int num(0);
            unsigned i(0);
            for(; i < ops.length(); ++i) {
                if(0 < i) {
                    num += fprintf(ff, " ");
                }
                num += do_print(ff, tdop, ops[i]);
            }

            return num;
        }

        /// print out an operator string
        static int do_print(
            FILE *ff,
            const tdop_type &tdop,
            const rule_type rule
        ) throw() {
            int num(0);
            if(!rule.is_valid()) {
                return num;
            }
            num += do_print(ff, tdop, rule.category());
            num += fprintf(ff, " :: ");
            operator_string_type ops;
            unsigned upper_bound(0U);

            if(rule.match(upper_bound, ops)) {
                num += fprintf(ff, "%u ", upper_bound);
            } else {
                rule.match(ops);
            }

            return num + do_print(ff, tdop, ops);
        }
    };

    /// print something out of a TDOP machine
    template <typename AlphaT, typename ThingT>
    int fprint(FILE *ff, const fltl::TDOP<AlphaT> &tdop, const ThingT thing) throw() {
        return fprint_tdop_impl<AlphaT>::do_print(ff, tdop, thing);
    }

    /// print out a TDOP machine
    template <typename AlphaT>
    int fprint(FILE *ff, fltl::TDOP<AlphaT> &tdop) throw() {
        FLTL_TDOP_USE_TYPES(fltl::TDOP<AlphaT>);

        int num(0);

        category_type cat;
        unsigned upper_bound(0);
        operator_string_type ops;

        generator_type categories(tdop.search(~cat));
        generator_type initial_rules(tdop.search(cat --->* (~ops)));
        generator_type extension_rules(tdop.search(cat[upper_bound] --->* (~ops)));

        for(; categories.match_next(); ) {
            num += fprint(ff, tdop, cat);
            num += fprintf(ff, " {\n");

            // print out initial rules
            for(initial_rules.rewind(); initial_rules.match_next(); ) {
                num += fprintf(ff, "    ");
                num += fprint(ff, tdop, ops);
                num += fprintf(ff, ";\n");
            }

            // print out extension rules
            for(extension_rules.rewind(); extension_rules.match_next(); ) {
                num += fprintf(ff, "    %u ", upper_bound);
                num += fprint(ff, tdop, ops);
                num += fprintf(ff, ";\n");
            }

            num += fprintf(ff, "}\n");
        }

        return num;
    }
}}

#endif /* Grail_Plus_IO_FPRINT_TDOP_HPP_ */
