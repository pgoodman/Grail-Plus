/*
 * TaggedUnion.hpp
 *
 *  Created on: May 11, 2012
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef Grail_Plus_MPL_UNION_HPP_
#define Grail_Plus_MPL_UNION_HPP_

#include "fltl/include/helper/Placeholder.hpp"

#define FLTL_MPL_TAGGED_UNION_CONSTRUCT(type) \
    type &construct()

namespace fltl { namespace mpl {

    template <typename T0, typename T1, typename T2=void, typename T3=void, typename T4=void>
    class Union {

    };

}}


#endif /* Grail_Plus_MPL_UNION_HPP_ */
