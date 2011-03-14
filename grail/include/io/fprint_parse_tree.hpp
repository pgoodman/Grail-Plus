/*
 * fprint_parse_tree.hpp
 *
 *  Created on: Mar 14, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
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
