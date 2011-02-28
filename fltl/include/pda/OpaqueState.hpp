/*
 * OpaqueState.hpp
 *
 *  Created on: Feb 26, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_OPAQUESTATE_HPP_
#define FLTL_OPAQUESTATE_HPP_

namespace fltl { namespace pda {

    template <typename AlphaT>
    class OpaqueState {
    private:

        friend class PDA<AlphaT>;

        typedef OpaqueState<AlphaT> self_type;

        unsigned id;

        OpaqueState(const unsigned _id) throw()
            : id(_id)
        { }

    public:

        OpaqueState(void) throw()
            : id(0)
        { }

        OpaqueState(const self_type &that) throw()
            : id(that.id)
        { }

        ~OpaqueState(void) throw() {
            id = 0U;
        }

        self_type &operator=(const self_type &that) throw() {
            id = that.id;
            return *this;
        }
    };

}}

#endif /* FLTL_OPAQUESTATE_HPP_ */
