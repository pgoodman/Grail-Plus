/*
 * Uncopyable.hpp
 *
 *  Created on: Sep 17, 2010
 *      Author: Peter Goodman
 *     Version: $Id$
 */

#ifndef FLTL_TRAIT_UNCOPYABLE_HPP_
#define FLTL_TRAIT_UNCOPYABLE_HPP_

namespace fltl { namespace trait {

    /// Don't allow for subtypes of this type to be copied
    class Uncopyable {
    protected:

        Uncopyable(void) throw() { }

        virtual ~Uncopyable(void) throw() { }

    private:

        Uncopyable(const Uncopyable &) throw() { }

        Uncopyable &operator=(const Uncopyable &) throw() {
            return *this;
        }
    };

}}

#endif /* FLTL_TRAIT_UNCOPYABLE_HPP_ */
