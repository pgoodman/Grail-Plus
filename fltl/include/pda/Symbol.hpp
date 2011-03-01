/*
 * Symbol.hpp
 *
 *  Created on: Feb 26, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_PDA_SYMBOL_HPP_
#define FLTL_PDA_SYMBOL_HPP_

namespace fltl { namespace pda {

    /// represents a symbol from the alphabet
    template <typename AlphaT>
    class Symbol {
    private:

        friend class PDA<AlphaT>;
        friend class SymbolBuffer<AlphaT>;

        typedef Symbol<AlphaT> self_type;

        unsigned id;

        Symbol(const unsigned _id) throw()
            : id(_id)
        { }

    public:

        Symbol(void) throw()
            : id(0)
        { }

        Symbol(const self_type &that) throw()
            : id(that.id)
        { }

        ~Symbol(void) throw() {
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
    };

}}

#endif /* FLTL_PDA_SYMBOL_HPP_ */
