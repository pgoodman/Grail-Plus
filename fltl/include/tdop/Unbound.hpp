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

    template <typename, typename> class Unbound;

    template <typename AlphaT>
    class Unbound<AlphaT, symbol_tag> {
    private:

        friend class detail::PatternData<AlphaT>;

        Symbol<AlphaT> *expr;

    public:

        /// unbound predicate symbol
        const Unbound<AlphaT, operator_tag> operator&(void) const throw() {
            // TODO
        }

        // TODO
    };

    template <typename AlphaT>
    class Unbound<AlphaT, operator_tag> {
    private:

            friend class detail::PatternData<AlphaT>;

            Operator<AlphaT> *expr;

    public:

        // TODO
    };

    template <typename AlphaT>
    class Unbound<AlphaT, operator_string_tag> {
    private:

            friend class detail::PatternData<AlphaT>;

            OperatorString<AlphaT> *expr;

    public:

        // TODO
    };

    template <typename AlphaT>
    class Unbound<AlphaT, term_tag> {
    private:

            friend class detail::PatternData<AlphaT>;

            Term<AlphaT> *expr;

    public:

        // TODO
    };

    template <typename AlphaT>
    class Unbound<AlphaT, category_tag> {
    private:

            friend class detail::PatternData<AlphaT>;

            OpaqueCategory<AlphaT> *expr;

    public:

        // TODO
    };
}}

#endif /* Grail_Plus_TDOP_UNBOUND_HPP_ */
