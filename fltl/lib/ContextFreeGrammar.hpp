/*
 * ContextFreeGrammar.hpp
 *
 *  Created on: Sep 19, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef FLTL_LIB_CONTEXTFREEGRAMMAR_HPP_
#define FLTL_LIB_CONTEXTFREEGRAMMAR_HPP_

#include "fltl/include/trait/PolyadicOperator.hpp"

#include "fltl/include/mpl/Sequence.hpp"

#include "fltl/include/stl/Grammar.hpp"

namespace fltl { namespace lib {

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

        typedef stl::Grammar<
            TermT,
            NonTermT,
            disjunction,
            mpl::Sequence<catenation>
        > parent_t;

    public:

        /// trait for query building
        typedef typename parent_t::query_discriminator_t
                query_discriminator_t;
    };

}}

#endif /* FLTL_LIB_CONTEXTFREEGRAMMAR_HPP_ */
