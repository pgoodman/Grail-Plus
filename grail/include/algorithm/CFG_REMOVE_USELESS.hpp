/*
 * CFG_REMOVE_USELESS.hpp
 *
 *  Created on: Mar 10, 2011
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

#ifndef FLTL_CFG_REMOVE_USELESS_HPP_
#define FLTL_CFG_REMOVE_USELESS_HPP_

#include <vector>

namespace grail { namespace algorithm {

    /// remove all non-generating variables and all unreachable variables
    template <typename AlphaT>
    class CFG_REMOVE_USELESS {

        typedef fltl::CFG<AlphaT> CFG;

        FLTL_CFG_USE_TYPES(CFG);

    private:

        static void reach_variable(
            const CFG &cfg,
            const variable_type var,
            std::vector<bool> &reachable
        ) throw() {
            if(reachable[var.number()]) {
                return;
            }

            reachable[var.number()] = true;

            symbol_string_type str;
            generator_type reached_productions(cfg.search(var --->* ~str));

            for(; reached_productions.match_next(); ) {
                for(unsigned i(0), len(str.length()); i < len; ++i) {
                    if(str.at(i).is_variable()) {
                        reach_variable(
                            cfg,
                            variable_type(str.at(i)),
                            reachable
                        );
                    }
                }
            }
        }

    public:

        static void run(CFG &cfg) throw() {
            variable_type V;
            generator_type variables(cfg.search(~V));

            // no start variable, nothing is reachable
            if(!cfg.has_start_variable()) {
                for(; variables.match_next(); ) {
                    cfg.unsafe_remove_variable(V);
                }
                return;
            }

            // find all reachable variables
            std::vector<bool> reachable(cfg.num_variables_capacity() + 2, false);
            reach_variable(cfg, cfg.get_start_variable(), reachable);

            // get rid of unreachable variables
            for(variables.rewind();
                variables.match_next(); ) {

                if(!(reachable[V.number()])) {
                    cfg.remove_variable(V);
                }
            }

            // get rid of non-generating variables
            for(variables.rewind();
                variables.match_next(); ) {

                if(0 == cfg.num_productions(V)) {
                    cfg.remove_variable(V);
                }
            }
        }
    };
}}

#endif /* FLTL_CFG_REMOVE_USELESS_HPP_ */
