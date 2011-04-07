/*
 * CFG_TO_GNF.hpp
 *
 *  Created on: Mar 4, 2011
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

#ifndef FLTL_CFG_TO_GNF_HPP_
#define FLTL_CFG_TO_GNF_HPP_

#include <cassert>
#include <set>
#include <map>
#include <utility>

#include "fltl/include/CFG.hpp"

#include "grail/include/algorithm/CFG_REMOVE_LR.hpp"
#include "grail/include/algorithm/CFG_REMOVE_EPSILON.hpp"
#include "grail/include/algorithm/CFG_TO_CNF.hpp"
#include "grail/include/algorithm/CFG_TO_2CFG.hpp"

#include "grail/include/io/verbose.hpp"

namespace grail { namespace algorithm {

    /// convert a context-free grammar into Greibach Normal Form.
    template <typename AlphaT>
    class CFG_TO_GNF {

        // take off the templates!
        typedef fltl::CFG<AlphaT> CFG;

        FLTL_CFG_USE_TYPES(CFG);

    public:

        static void run(CFG &cfg) throw() {

            CFG_TO_CNF<AlphaT>::run(cfg);
            CFG_REMOVE_LR<AlphaT>::run(cfg);
            CFG_TO_2CFG<AlphaT>::run(cfg);
            CFG_REMOVE_EPSILON<AlphaT>::run(cfg);

            variable_type A;
            variable_type B;

            symbol_string_type A_suffix;
            symbol_string_type B_str;

            production_type non_greibach_prod;
            generator_type non_greibach_prods(cfg.search(
                ~non_greibach_prod,
                (~A) --->* (~B) + (~A_suffix)
            ));

            generator_type sub_productions(cfg.search(B --->* (~B_str)));

            symbol_string_type new_str;

            for(bool updated(true); updated; ) {
                updated = false;
                for(non_greibach_prods.rewind();
                    non_greibach_prods.match_next();) {

                    cfg.remove_production(non_greibach_prod);
                    for(sub_productions.rewind();
                        sub_productions.match_next(); ) {

                        assert(A != B);

                        new_str = cfg.add_production(A, B_str + A_suffix).symbols();

                        if(!new_str.is_empty() && new_str.at(0).is_variable()) {
                            updated = true;
                        }
                    }
                }
            }
        }

    };
}}

#endif /* FLTL_CFG_TO_GNF_HPP_ */
