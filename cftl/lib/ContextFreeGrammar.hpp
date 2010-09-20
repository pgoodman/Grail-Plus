/*
 * ContextFreeGrammar.hpp
 *
 *  Created on: Sep 19, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef CFTL_LIB_CONTEXTFREEGRAMMAR_HPP_
#define CFTL_LIB_CONTEXTFREEGRAMMAR_HPP_

#include "src/include/trait/PolyadicOperator.hpp"

#include "src/include/mpl/Sequence.hpp"

#include "src/include/stl/Grammar.hpp"

namespace cftl { namespace lib {

    namespace {
        class catenation : public trait::PolyadicOperator<2> { };
        class disjunction : public trait::PolyadicOperator<2> { };
    }

    template <typename TermT, typename NonTermT>
    class ContextFreeGrammar : public stl::Grammar<
        TermT,
        NonTermT,
        disjunction,
        mpl::Sequence<catenation>
    > {
    private:
    public:
    };

}}

#endif /* CFTL_LIB_CONTEXTFREEGRAMMAR_HPP_ */
