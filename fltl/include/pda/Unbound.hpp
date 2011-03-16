/*
 * Unbound.hpp
 *
 *  Created on: Mar 1, 2011
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

#ifndef FLTL_UNBOUND_HPP_
#define FLTL_UNBOUND_HPP_

namespace fltl { namespace pda {

    template <typename AlphaT>
    class Unbound<AlphaT,symbol_tag> {
    private:

        friend class Symbol<AlphaT>;
        friend class PDA<AlphaT>;
        friend class pattern::Init<AlphaT>;
        template <typename,typename> friend class pattern::Bind;

        Symbol<AlphaT> *symbol;

        Unbound(Symbol<AlphaT> *sym) throw()
            : symbol(sym)
        { }

        Unbound(void) throw() {
            symbol = 0;
        }

    public:

        typedef unbound_symbol_tag tag_type;
    };

    template <typename AlphaT>
    class Unbound<AlphaT,state_tag> {
    private:

        friend class OpaqueState<AlphaT>;
        friend class PDA<AlphaT>;
        friend class pattern::Init<AlphaT>;
        template <typename,typename> friend class pattern::Bind;

        OpaqueState<AlphaT> *state;

        Unbound(OpaqueState<AlphaT> *ss) throw()
            : state(ss)
        { }

        Unbound(void) throw() {
            state = 0;
        }

    public:

        typedef unbound_state_tag tag_type;
    };

    template <typename AlphaT>
    class Unbound<AlphaT,transition_tag> {
    private:

        friend class OpaqueTransition<AlphaT>;
        friend class PDA<AlphaT>;
        friend class pattern::Init<AlphaT>;
        template <typename,typename> friend class pattern::Bind;

        OpaqueTransition<AlphaT> *transition;

        Unbound(OpaqueTransition<AlphaT> *trans) throw()
            : transition(trans)
        { }

        Unbound(void) throw() {
            transition = 0;
        }

    public:

        typedef unbound_transition_tag tag_type;
    };
}}

#endif /* FLTL_UNBOUND_HPP_ */
