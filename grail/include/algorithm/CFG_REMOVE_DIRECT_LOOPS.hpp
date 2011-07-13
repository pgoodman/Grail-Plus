/*
 * CFG_REMOVE_DIRECT_LOOPS.hpp
 *
 *  Created on: Mar 6, 2011
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

#ifndef FLTL_CFG_REMOVE_DIRECT_LOOPS_HPP_
#define FLTL_CFG_REMOVE_DIRECT_LOOPS_HPP_

#include "fltl/include/CFG.hpp"

namespace grail { namespace algorithm {

    /// remove all direct self-loops from a CFG.
    template <typename AlphaT>
    class CFG_REMOVE_DIRECT_LOOPS {
    public:

        // take off the templates!
        typedef fltl::CFG<AlphaT> CFG;

        FLTL_CFG_USE_TYPES(CFG);

        static void run(CFG &cfg) throw() {
            variable_type A;
            production_type P;

            // remove direct self-loops
            generator_type self_loops(cfg.search(~P, (~A) --->* A));
            for(; self_loops.match_next(); ) {
                cfg.remove_production(P);
            }
        }
    };
}}

#endif /* FLTL_CFG_REMOVE_DIRECT_LOOPS_HPP_ */
