/*
 * AddReferemce.hpp
 *
 *  Created on: Sep 15, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef CFTL_ADDREFEREMCE_HPP_
#define CFTL_ADDREFEREMCE_HPP_

namespace cftl {
    namespace mpl {

        template <typename T>
        class AddReference {
        public:
            typedef T &type_t;
        };

        template <typename T>
        class AddReference<T &> {
        public:
            typedef T &type_t;
        };
    }
}

#endif /* CFTL_ADDREFEREMCE_HPP_ */
