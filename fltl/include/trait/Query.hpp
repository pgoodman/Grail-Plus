/*
 * Query.hpp
 *
 *  Created on: Sep 20, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef FLTL_TRAIT_QUERY_HPP_
#define FLTL_TRAIT_QUERY_HPP_

namespace fltl { namespace trait {

    /// the idea of the query trait is that it provides all the machinery
    /// for the query classes to build the query without having to be
    /// "owned" by a specific data structure. i.e. a class has this trait
    /// if there is a specialization of this trait for that class.
    template <typename DataStructurT>
    class Query { };

}}

#endif /* FLTL_TRAIT_QUERY_HPP_ */
