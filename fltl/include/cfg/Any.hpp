/*
 * Any.hpp
 *
 *  Created on: Mar 10, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_ANY_HPP_
#define FLTL_ANY_HPP_

namespace fltl { namespace cfg {

    template <typename AlphaT>
    class AnySymbol {
    public:
        typedef AnySymbol self_type;
        FLTL_CFG_PRODUCTION_PATTERN(any_symbol_tag)
    };

    template <typename AlphaT>
    class AnySymbolString {
    public:

        AnySymbolStringOfLength<AlphaT>
        operator()(unsigned &len) const throw() {
            return AnySymbolStringOfLength<AlphaT>(len);
        }
    };

    template <typename AlphaT>
    class AnySymbolStringOfLength {
    public:

        typedef AnySymbolStringOfLength<AlphaT> self_type;

        unsigned *length;

        AnySymbolStringOfLength(unsigned &len) throw()
            : length(&len)
        { }

        AnySymbolStringOfLength(const self_type &that) throw()
            : length(that.length)
        { }
    };

}}

#endif /* FLTL_ANY_HPP_ */
