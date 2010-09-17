/*
 * Grammar.hpp
 *
 *  Created on: Sep 17, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef CFTL_STL_GRAMMAR_HPP_
#define CFTL_STL_GRAMMAR_HPP_

namespace cftl { namespace stl {

    /// base grammar type.
    template <typename TermT, // terminal type
              typename NonTermT, // non-terminal type
              typename AllocatorNameT // name of allocator template type
             >
    class Grammar {
    private:
    public:
    };
}}

#endif /* CFTL_STL_GRAMMAR_HPP_ */
