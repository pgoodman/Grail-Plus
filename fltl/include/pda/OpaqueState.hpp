/*
 * OpaqueState.hpp
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

#ifndef FLTL_OPAQUESTATE_HPP_
#define FLTL_OPAQUESTATE_HPP_

namespace fltl { namespace pda {

    template <typename AlphaT>
    class OpaqueState {
    private:

        typedef OpaqueState<AlphaT> self_type;

        friend class PDA<AlphaT>;
        friend class StateGenerator<AlphaT>;
        friend class TransitionGenerator<AlphaT>;
        friend class Transition<AlphaT>;
        friend struct std::less<self_type>;

        template <
            typename, typename,
            typename, typename,
            typename, typename
        > friend class PatternGenerator;

        template <typename, typename>
        friend class detail::ResetPatternGenerator;

        template <typename, typename, typename>
        friend class detail::FindNextTransition;

        unsigned id;

        explicit OpaqueState(const unsigned _id) throw()
            : id(_id)
        { }

    public:

        typedef state_tag tag_type;

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

        bool operator==(const self_type &that) const throw() {
            return id == that.id;
        }

        bool operator!=(const self_type &that) const throw() {
            return id != that.id;
        }

        bool operator<(const self_type &that) const throw() {
            return id < that.id;
        }

        bool operator<=(const self_type &that) const throw() {
            return id <= that.id;
        }

        bool operator>(const self_type &that) const throw() {
            return id > that.id;
        }

        bool operator>=(const self_type &that) const throw() {
            return id >= that.id;
        }

        /// note: not const!
        Unbound<AlphaT,state_tag> operator~(void) throw() {
            return Unbound<AlphaT,state_tag>(this);
        }

        unsigned number(void) const throw() {
            return id;
        }
    };

}}

namespace std {

    template <typename AlphaT>
    struct less<fltl::pda::OpaqueState<AlphaT> > : binary_function <fltl::pda::OpaqueState<AlphaT>,fltl::pda::OpaqueState<AlphaT>,bool> {
    public:
        bool operator() (const fltl::pda::OpaqueState<AlphaT> &x, const fltl::pda::OpaqueState<AlphaT> &y) const {
            return x.id < y.id;
        }
    };
}

#endif /* FLTL_OPAQUESTATE_HPP_ */
