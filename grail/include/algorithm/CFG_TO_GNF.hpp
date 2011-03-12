/*
 * CFG_TO_GNF.hpp
 *
 *  Created on: Mar 4, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
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
        typedef typename CFG::variable_type variable_type;
        typedef typename CFG::production_type production_type;
        typedef typename CFG::symbol_string_type symbol_string_type;
        typedef typename CFG::generator_type generator_type;

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
