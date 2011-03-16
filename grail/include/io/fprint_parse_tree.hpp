/*
 * fprint_parse_tree.hpp
 *
 *  Created on: Mar 14, 2011
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

#ifndef FLTL_FPRINT_PARSE_TREE_HPP_
#define FLTL_FPRINT_PARSE_TREE_HPP_

#include <cstdio>

#include "fltl/include/CFG.hpp"

#include "grail/include/cfg/ParseTree.hpp"

namespace grail { namespace io {

    class dot_language { };
    class lisp_language { };
    class tree_language { };

    template <typename AlphaT>
    int fprint(
        FILE *ff,
        fltl::CFG<AlphaT> &gram,
        const grail::cfg::ParseTree<AlphaT> *tree,
        const dot_language
    ) throw() {

        (void) ff;
        (void) gram;
        (void) tree;
    }

    template <typename AlphaT>
    int fprint(
        FILE *ff,
        const fltl::CFG<AlphaT> &gram,
        const grail::cfg::ParseTree<AlphaT> *tree,
        const lisp_language
    ) throw() {

        (void) ff;
        (void) gram;
        (void) tree;
    }

    template <typename AlphaT>
    int fprint(
        FILE *ff,
        const fltl::CFG<AlphaT> &gram,
        const grail::cfg::ParseTree<AlphaT> *tree,
        const tree_language
    ) throw() {

        (void) ff;
        (void) gram;
        (void) tree;
    }
}}

#endif /* FLTL_FPRINT_PARSE_TREE_HPP_ */
