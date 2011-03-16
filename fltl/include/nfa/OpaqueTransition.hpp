/*
 * OpaqueTransition.hpp
 *
 *  Created on: Mar 7, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
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
