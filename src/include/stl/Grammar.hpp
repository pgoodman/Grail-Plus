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
    ///
    /// The base grammar template type imposes the following restrictions
    /// on its parameterized types:
    ///
    ///     - Parameter types must have default constructors, copy
    ///       constructors, and copy assignment operators.
    ///     - The default construction of AcceptNonTermT must be the epsilon
    ///       non-terminal that accepts an empty word. By default,
    ///       AcceptNonTermT is NonTermT.
    ///
    template <typename TermT, // terminal type
              typename NonTermT, // non-terminal type
              typename AcceptNonTermT=NonTermT, // empty non-terminal
              typename AllocatorNameT // name of allocator template type
             >
    class Grammar {
    private:
    public:
    };
}}

#endif /* CFTL_STL_GRAMMAR_HPP_ */
