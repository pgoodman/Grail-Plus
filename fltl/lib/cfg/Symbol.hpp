/*
 * Symbol.hpp
 *
 *  Created on: Jan 20, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_SYMBOL_HPP_
#define FLTL_SYMBOL_HPP_

namespace fltl { namespace lib { namespace cfg {

    /// grammar symbol (non-terminal or terminal)
    template <typename AlphaT>
    class Symbol {
    protected:

        int32_t value;

        friend class CFG<AlphaT>;
        friend class Variable<AlphaT>;
        friend class Production<AlphaT>;
        friend class ProductionBuilder<AlphaT>;

        explicit Symbol(const int32_t v) throw()
            : value(v)
        { }

    public:

        typedef Symbol<AlphaT> self_type;

        Symbol(void) throw()
            : value(0)
        { }

        Symbol(const self_type &that) throw()
            : value(that.value)
        { }

        self_type &operator=(const self_type &that) throw() {
            value = that.value;
            return *this;
        }

        bool operator==(const self_type &that) const throw() {
            return value == that.value;
        }

        bool operator!=(const self_type &that) const throw() {
            return value != that.value;
        }
    };

}}}

#endif /* FLTL_SYMBOL_HPP_ */
