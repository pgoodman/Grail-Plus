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

        typedef OpaqueState<AlphaT> self_type;

        friend class PDA<AlphaT>;
        friend class StateGenerator<AlphaT>;
        friend class TransitionGenerator<AlphaT>;
        friend struct std::less<self_type>;

        unsigned id;

        explicit OpaqueState(const unsigned _id) throw()
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

        bool operator==(const self_type &that) const throw() {
            return id == that.id;
        }

        bool operator!=(const self_type &that) const throw() {
            return id != that.id;
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
