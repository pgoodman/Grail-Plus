/*
 * CFG_REMOVE_LR.hpp
 *
 *  Created on: Mar 5, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_CFG_REMOVE_LR_HPP_
#define FLTL_CFG_REMOVE_LR_HPP_

#include <cassert>
#include <set>
#include <map>

#include "fltl/include/CFG.hpp"

#include "grail/algorithm/CFG_REMOVE_EPSILON.hpp"

namespace grail { namespace algorithm {

    /// remove all left recursion from a context-free grammar.
    /// follows the algorithm of R. C. Moore, "Removing Left Recursion from
    /// Context-Free Grammars"
    template <typename AlphaT>
    class CFG_REMOVE_LR {
    private:

        // take off the templates!
        typedef fltl::CFG<AlphaT> CFG;
        typedef typename CFG::variable_type variable_type;
        typedef typename CFG::terminal_type terminal_type;
        typedef typename CFG::production_type production_type;
        typedef typename CFG::generator_type generator_type;
        typedef typename CFG::symbol_string_type symbol_string_type;
        typedef typename CFG::production_builder_type production_builder_type;

        static void find_left_corners_rec(
            CFG &cfg,
            std::map<variable_type, std::set<variable_type> *> &left_corners,
            std::set<variable_type> &in_progress,
            std::set<variable_type> &updated,
            variable_type var
        ) throw() {

        }

        /// compute all left corners
        static void find_left_corners(
            CFG &cfg,
            std::map<variable_type, std::set<variable_type> *> &left_corners
        ) throw() {
            variable_type A;
            generator_type variables(cfg.search(~A));

            // sets of variables that we are currently computing
            std::set<variable_type> in_progress;

            // sets of variables to which we have added new left corners
            std::set<variable_type> updated[2];
            unsigned which_updated(0);

            // iterator positions
            typename std::set<variable_type>::iterator updated_it;
            typename std::set<variable_type>::iterator updated_end;

            for(; variables.match_next(); ) {
                left_corners[A] = new std::set<variable_type>;
                updated[which_updated].insert(A);
            }

            for(; !updated[which_updated].empty(); in_progress.clear()) {

                for(updated_it = updated[which_updated].begin(),
                    updated_end = updated[which_updated].end();
                    updated_end != updated_it;
                    ++updated_it) {

                    find_left_corners_rec(
                        cfg,
                        left_corners,
                        in_progress,
                        updated[1 - which_updated],
                        *updated_it
                    );
                }

                updated[which_updated].clear();
                which_updated = 1 - which_updated;
            }
        }

    public:

        static void run(CFG &cfg) throw() {

            // remove all epsilon transitions, except in the case that
            // the language generates the empty string
            CFG_REMOVE_EPSILON<AlphaT>::run(cfg);

            std::map<variable_type, std::set<variable_type> *> left_corners;
            find_left_corners(cfg, left_corners);

            variable_type A;
            variable_type B;


        }
    };
}}


#endif /* FLTL_CFG_REMOVE_LR_HPP_ */
