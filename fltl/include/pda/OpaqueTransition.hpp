/*
 * OpaqueTransition.hpp
 *
 *  Created on: Feb 26, 2011
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

#ifndef FLTL_OPAQUETRANSITION_HPP_
#define FLTL_OPAQUETRANSITION_HPP_

namespace fltl { namespace pda {

    /// represents a transition of the PDA
    template <typename AlphaT>
    class OpaqueTransition {
    protected:

        friend class PDA<AlphaT>;
        friend class TransitionGenerator<AlphaT>;

        template <
            typename, typename,
            typename, typename,
            typename, typename
        > friend class PatternGenerator;

        friend class Pattern<AlphaT>;

        template <typename, typename>
        friend class detail::ResetPatternGenerator;

        template <typename, typename, typename>
        friend class detail::FindNextTransition;

        typedef OpaqueTransition<AlphaT> self_type;
        typedef OpaqueState<AlphaT> state_type;
        typedef Symbol<AlphaT> symbol_type;

        Transition<AlphaT> *transition;

        explicit OpaqueTransition(Transition<AlphaT> *trans) throw()
            : transition(trans)
        {
            if(0 != trans) {
                Transition<AlphaT>::hold(trans);
            }
        }

        void assign(Transition<AlphaT> *trans) throw() {

            if(transition != trans) {
                if(0 != transition) {
                    Transition<AlphaT>::release(transition);
                }

                transition = trans;

                if(0 != transition) {
                    Transition<AlphaT>::hold(transition);
                }
            }
        }

    public:

        typedef transition_tag tag_type;

        OpaqueTransition(void) throw()
            : transition(0)
        { }

        OpaqueTransition(const self_type &that) throw()
            : transition(that.transition)
        {
            if(0 != transition) {
                Transition<AlphaT>::hold(transition);
            }
        }

        ~OpaqueTransition(void) throw() {
            if(0 != transition) {
                Transition<AlphaT>::release(transition);
                transition = 0;
            }
        }

        self_type &operator=(const self_type &that) throw() {
            assign(that.transition);
            return *this;
        }

        const state_type &source(void) const throw() {
            assert(0 != transition);
            return transition->source_state;
        }

        const state_type &sink(void) const throw() {
            assert(0 != transition);
            return transition->sink_state;
        }

        const symbol_type &read(void) const throw() {
            assert(0 != transition);
            return transition->sym_read;
        }

        const symbol_type &pop(void) const throw() {
            assert(0 != transition);
            return transition->sym_pop;
        }

        const symbol_type &push(void) const throw() {
            assert(0 != transition);
            return transition->sym_push;
        }

        /// note: not const!
        Unbound<AlphaT,transition_tag> operator~(void) throw() {
            return Unbound<AlphaT,transition_tag>(this);
        }
    };

}}

#endif /* FLTL_OPAQUETRANSITION_HPP_ */
