/*
 * CFG_REMOVE_UNITS.hpp
 *
 *  Created on: Feb 15, 2011
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

#ifndef FLTL_CFG_REMOVE_UNITS_HPP_
#define FLTL_CFG_REMOVE_UNITS_HPP_

#include <cassert>
#include <set>
#include <utility>

#include "fltl/include/CFG.hpp"

namespace grail { namespace algorithm {

    /// remove unit productions from a context-free grammar.
    template <typename AlphaT>
    class CFG_REMOVE_UNITS {
    public:

        // take off the templates!
        typedef fltl::CFG<AlphaT> CFG;

        FLTL_CFG_USE_TYPES(CFG);

        static void run(CFG &cfg) throw() {

            std::pair<variable_type,variable_type> elim_key;
            std::set<std::pair<variable_type,variable_type> > eliminated;

            production_type unit_production;
            variable_type A;
            variable_type B;
            generator_type unit_productions(cfg.search(
                ~unit_production,
                (~A) --->* ~B
            ));

            symbol_string_type gen_string;
            generator_type related_productions(cfg.search(
                B --->* ~gen_string
            ));

            bool updated(true);

            for(; updated; ) {
                updated = false;

                for(unit_productions.rewind();
                    unit_productions.match_next(); ) {

                    cfg.remove_production(unit_production);

                    if(A == B) {
                        continue;
                    }

                    elim_key = std::make_pair(A,B);
                    if(0 != eliminated.count(elim_key)) {
                        continue;
                    }

                    eliminated.insert(elim_key);
                    updated = true;

                    for(related_productions.rewind();
                        related_productions.match_next(); ) {

                        // don't introduce self-loops
                        if(1 == gen_string.length()
                        && A == gen_string.at(0)) {
                            continue;
                        }

                        cfg.add_production(A, gen_string);
                    }
                }
            }
        }
    };
}}

#endif /* FLTL_CFG_REMOVE_UNITS_HPP_ */
