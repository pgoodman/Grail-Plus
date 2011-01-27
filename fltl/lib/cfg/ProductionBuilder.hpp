/*
 * CFGProductionBuilder.hpp
 *
 *  Created on: Jan 20, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_CFGPRODUCTIONBUILDER_HPP_
#define FLTL_CFGPRODUCTIONBUILDER_HPP_

namespace fltl { namespace lib { namespace cfg {

    /// buffer for building productions
    template <typename AlphaT>
    class ProductionBuilder : protected trait::Uncopyable {
    private:

        friend class CFG<AlphaT>;

        typedef Symbol<AlphaT> symbol_type;
        typedef ProductionBuilder<AlphaT> self_type;

        helper::Array<symbol_type> buffer;

    public:

        ProductionBuilder(void) throw()
            : buffer()
        {
            buffer.reserve(32U);
            buffer.append(mpl::Static<typename CFG<AlphaT>::variable_type>::VALUE);
        }

        inline self_type &clear(void) throw() {
            buffer.set_size(0U);
            return *this;
        }

        inline self_type &operator<<(const symbol_type &sym) throw() {
            if(0 != sym.value) {
                buffer.append(sym);
            }
            return *this;
        }

        SymbolString<AlphaT> symbols(void) throw() {
            return SymbolString<AlphaT>(
                &(buffer.get(0)),
                buffer.size()
            );
        }
    };
}}}

#endif /* FLTL_CFGPRODUCTIONBUILDER_HPP_ */
