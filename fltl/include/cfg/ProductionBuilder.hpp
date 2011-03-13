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

namespace fltl { namespace cfg {

    /// buffer for building productions
    template <typename AlphaT>
    class ProductionBuilder : protected trait::Uncopyable {
    private:

        friend class CFG<AlphaT>;

        typedef Symbol<AlphaT> symbol_type;
        typedef SymbolString<AlphaT> symbol_string_type;
        typedef ProductionBuilder<AlphaT> self_type;

        helper::Array<symbol_type> buffer;

    public:

        ProductionBuilder(void) throw()
            : buffer()
        {
            buffer.reserve(32U);
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

        inline self_type &operator<<(const symbol_string_type &str) throw() {
            append(str);
            return *this;
        }

        inline void append(const symbol_type &sym) throw() {
            if(0 != sym.value) {
                buffer.append(sym);
            }
        }

        inline void append(const symbol_string_type &str) throw() {
            if(0 != str.symbols) {
                const unsigned str_len(str.length());
                buffer.reserve(buffer.size() + str_len);

                for(unsigned i(0); i < str_len; ++i) {
                    buffer.append(str.at(i));
                }
            }
        }

        SymbolString<AlphaT> symbols(void) throw() {
            return SymbolString<AlphaT>(
                &(buffer.get(0)),
                buffer.size()
            );
        }

        inline unsigned size(void) const throw() {
            return buffer.size();
        }

        inline const symbol_type &symbol_at(const unsigned i) throw() {
            return buffer.get(i);
        }
    };
}}

#endif /* FLTL_CFGPRODUCTIONBUILDER_HPP_ */
