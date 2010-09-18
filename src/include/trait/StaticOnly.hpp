/*
 * StaticOnly.hpp
 *
 *  Created on: Sep 17, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef CFTL_TRAIT_STATICONLY_HPP_
#define CFTL_TRAIT_STATICONLY_HPP_

namespace cftl { namespace trait {

    class StaticOnly {
    private:
        StaticOnly(void) throw() { }
        StaticOnly(const StaticOnly &) throw() { }
        ~StaticOnly(void) throw() { }
    };

}}

#endif /* CFTL_TRAIT_STATICONLY_HPP_ */
