/*
 * CFG_TO_2CFG.hpp
 *
 *  Created on: Mar 5, 2011
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

#ifndef FLTL_CFG_TO_2CFG_HPP_
#define FLTL_CFG_TO_2CFG_HPP_


#include <cassert>
#include <set>
#include <map>
#include <utility>

#include "fltl/include/CFG.hpp"

#include "grail/include/algorithm/CFG_REMOVE_UNITS.hpp"
#include "grail/include/algorithm/CFG_REMOVE_EPSILON.hpp"

namespace grail { namespace algorithm {

    /// convert the productions of a CFG to be either unit productions or
    /// tuples
    template <typename AlphaT>
    class CFG_TO_2CFG {

        // take off the templates!
        typedef fltl::CFG<AlphaT> CFG;

        FLTL_CFG_USE_TYPES(CFG);

    public:

        /// go look for all productions with three or more symbols on their RHS
        /// and break them into pairs of productions
        static void run(CFG &cfg) throw() {

            variable_type A;
            symbol_string_type str;
            production_type P;

            generator_type long_rules(cfg.search(
                ~P,
                (~A) --->* cfg._ + cfg._ + cfg._ + cfg.__
            ));

            for(; long_rules.match_next(); ) {

                cfg.remove_production(P);

                str = P.symbols();

                unsigned i(str.length() - 2);
                variable_type prev_new_var(cfg.add_variable());
                cfg.add_production(prev_new_var, str.substring(i, 2));

                for(--i; i > 0; --i) {
                    variable_type new_var(cfg.add_variable());
                    cfg.add_production(new_var, str.at(i) + prev_new_var);
                    prev_new_var = new_var;
                }

                cfg.add_production(A, str.at(0) + prev_new_var);
            }
        }
    };

}}

#endif /* FLTL_CFG_TO_2CFG_HPP_ */
