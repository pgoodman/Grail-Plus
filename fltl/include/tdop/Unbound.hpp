/*
 * Unbound.hpp
 *
 *  Created on: May 11, 2012
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef Grail_Plus_TDOP_UNBOUND_HPP_
#define Grail_Plus_TDOP_UNBOUND_HPP_

namespace fltl { namespace tdop {

    template <typename> class Unbound;

    template <typename AlphaT>
    class Unbound<Symbol<AlphaT> > {
    public:

        /// unbound predicate symbol
        const Unbound<Operator<AlphaT> > operator&(void) const throw() {
            // TODO
        }

        // TODO
    };

    template <typename AlphaT>
    class Unbound<Operator<AlphaT> > {
    public:

        // TODO
    };

    template <typename AlphaT>
    class Unbound<OperatorString<AlphaT> > {
    public:

        // TODO
    };

    template <typename AlphaT>
    class Unbound<Term<AlphaT> > {
    public:

        // TODO
    };

    template <typename AlphaT>
    class Unbound<OpaqueCategory<AlphaT> > {
    public:

        // TODO
    };
}}

#endif /* Grail_Plus_TDOP_UNBOUND_HPP_ */
