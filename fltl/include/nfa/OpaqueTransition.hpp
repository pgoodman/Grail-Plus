/*
 * OpaqueTransition.hpp
 *
 *  Created on: Mar 7, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_NFA_OPAQUE_TRANSITION_HPP_
#define FLTL_NFA_OPAQUE_TRANSITION_HPP_

namespace fltl { namespace nfa {

    template <typename AlphaT>
    class OpaqueTransition : protected pda::OpaqueTransition<AlphaT> {
    private:

        typedef OpaqueTransition<AlphaT> self_type;
        typedef pda::OpaqueTransition<AlphaT> base_type;

    public:

        OpaqueTransition(void) throw()
            : pda::OpaqueTransition<AlphaT>()
        { }

        OpaqueTransition(const self_type &that) throw()
            : pda::OpaqueTransition<AlphaT>(that)
        { }

        OpaqueTransition(const base_type &that) throw()
            : pda::OpaqueTransition<AlphaT>(that)
        { }

        using pda::OpaqueTransition<AlphaT>::operator=;
        using pda::OpaqueTransition<AlphaT>::source;
        using pda::OpaqueTransition<AlphaT>::sink;
        using pda::OpaqueTransition<AlphaT>::read;
        using pda::OpaqueTransition<AlphaT>::operator~;
    };

}}

#endif /* FLTL_NFA_OPAQUE_TRANSITION_HPP_ */
