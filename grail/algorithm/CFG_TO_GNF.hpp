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

#include "grail/algorithm/CFG_REMOVE_UNITS.hpp"
#include "grail/algorithm/CFG_REMOVE_EPSILON.hpp"

namespace grail { namespace algorithm {

    /// convert a context-free grammar into Greibach Normal Form.
    template <typename AlphaT>
    class CFG_TO_GNF {

        // take off the templates!
        typedef fltl::CFG<AlphaT> CFG;


    };
}}

#endif /* FLTL_CFG_TO_GNF_HPP_ */
