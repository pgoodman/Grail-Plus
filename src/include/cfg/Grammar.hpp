/*
 * Grammar.hpp
 *
 *  Created on: Sep 14, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef CFTL_CFG_GRAMMAR_HPP_
#define CFTL_CFG_GRAMMAR_HPP_

#include "src/include/mpl/TotalOrder.hpp"

namespace cftl {
    namespace cfg {

        template <typename A, typename Order=mpl::TotalOrder<A> >
        class Grammar {

        };

    }
}

#endif /* CFTL_CFG_GRAMMAR_HPP_ */
