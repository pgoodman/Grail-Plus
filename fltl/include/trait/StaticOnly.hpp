/*
 * StaticOnly.hpp
 *
 *  Created on: Sep 17, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef FLTL_TRAIT_STATICONLY_HPP_
#define FLTL_TRAIT_STATICONLY_HPP_

namespace fltl { namespace trait {

    class StaticOnly {
    private:
        StaticOnly(void) throw() { }
        StaticOnly(const StaticOnly &) throw() { }
        ~StaticOnly(void) throw() { }
    };

}}

#endif /* FLTL_TRAIT_STATICONLY_HPP_ */
