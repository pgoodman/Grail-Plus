/*
 * TDOP_MINIMIZE_WEAK.hpp
 *
 *  Created on: May 22, 2012
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef Grail_Plus_TDOP_MINIMIZE_WEAK_HPP_
#define Grail_Plus_TDOP_MINIMIZE_WEAK_HPP_

#include <cassert>

namespace grail { namespace algorithm {

    /// attempt to merge categories together using a weak form of minimization.
    ///
    /// two categories are weakly equivalent iff
    ///     - they share the same initial rules
    ///     - they share the same operator strings for their extensions rules,
    ///       and:
    ///       - similar rules share the same upper bound; or:
    ///       - the different upper bounds on similar rules provably do not
    ///         change the language accepted by the machine.
    ///
    /// note: this tool will remove all symbol predicates.
    /// note: this tool assumes that there all categories are bound.
    /// note: this tool assumes all categories are reachable.
    template <typename AlphaT>
    class TDOP_MINIMIZE_WEAK {

        FLTL_TDOP_USE_TYPES(fltl::TDOP<AlphaT>);

    private:

        /// returns true iff the tdop machine has any rule that contains an
        /// unbound category.
        bool has_unbound_category(tdop_type &tdop) throw() {
            category_type C;
            return tdop.search(tdop._ --->* tdop.__ + C + tdop.__).match_next()
                || tdop.search(tdop.__ --->* tdop.__ + C + tdop.__).match_next();
        }

    public:

        static void run(tdop_type &tdop) throw() {
            assert(!has_unbound_category(tdop));
        }
    };
}}


#endif /* Grail_Plus_TDOP_MINIMIZE_WEAK_HPP_ */
