/*
 * OpaqueProduction.hpp
 *
 *  Created on: Jan 21, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_OPAQUEPRODUCTION_HPP_
#define FLTL_OPAQUEPRODUCTION_HPP_

namespace fltl { namespace cfg {

    /// the production type that is exposed to users
    template <typename AlphaT>
    class OpaqueProduction {
    private:

        friend class CFG<AlphaT>;
        friend class detail::SimpleGenerator<AlphaT>;

        template <typename, typename>
        friend class detail::PatternGenerator;

        typedef OpaqueProduction<AlphaT> self_type;
        typedef typename CFG<AlphaT>::variable_type variable_type;
        typedef typename CFG<AlphaT>::symbol_type symbol_type;
        typedef typename CFG<AlphaT>::symbol_string_type symbol_string_type;

        /// the internal production that this opaque type references
        Production<AlphaT> *production;

        /// proper constructor for productions.
        explicit OpaqueProduction(Production<AlphaT> *prod) throw()
            : production(prod)
        {
            if(0 != prod) {
                Production<AlphaT>::hold(production);
            }
        }

        void assign(Production<AlphaT> *prod) throw() {
            if(production != prod) {

                if(0 != production) {
                    Production<AlphaT>::release(production);
                }

                production = prod;

                if(0 != production) {
                    Production<AlphaT>::hold(production);
                }
            }
        }

    public:

        OpaqueProduction(void) throw()
            : production(0)
        { }

        OpaqueProduction(const self_type &that) throw()
            : production(that.production)
        {
            if(0 != production) {
                Production<AlphaT>::hold(production);
            }
        }

        ~OpaqueProduction(void) throw() {
            if(0 != production) {
                Production<AlphaT>::release(production);
                production = 0;
            }
        }

        inline self_type &operator=(const self_type &that) throw() {
            assign(that.production);
            return *this;
        }

        /// is this production valid?
        inline bool is_valid(void) const throw() {
            return 0 != production;
        }

        /// access the variable of this production
        inline variable_type
        variable(void) const throw() {
            assert(
                0 != production &&
                "Unable to access variable of non-existent production."
            );
            return variable_type(production->var->id);
            //return *helper::unsafe_cast<variable_type *>(&(production->var));
        }

        /// return the length of this production
        inline unsigned length(void) const throw() {
            assert(
                0 != production &&
                "Unable to access length of non-existent production."
            );
            return production->length();
        }

        /// was this production automatically added to its variable? this
        /// happens in the event that we remove all productions from a
        /// variable.
        inline bool was_automatically_made(void) const throw() {
            assert(
                0 != production &&
                "Unable to access state of non-existent production."
            );
            return production->var->null_production == production;
        }

        /// equivalence of productions; to check that symbols are equivalent,
        /// use a.symbols() == b.symbols().
        inline bool operator==(const self_type &that) const throw() {
            return production == that.production;
        }
        inline bool operator!=(const self_type &that) const throw() {
            return production != that.production;
        }

        /// access a symbol from the production. uses 0 indexing, i.e. if we
        /// have a production of the form prod = A -> B C D then:
        ///             prod[0] <==> prod.symbol(0) <==> B.
        inline const symbol_type &
        operator[](const unsigned offset) const throw() {
            return symbol_at(offset);
        }

        inline const symbol_type &
        symbol_at(const unsigned offset) const throw() {
            assert(
                0 != production &&
                "Unable to access symbol of non-existent production."
            );
            return production->symbols.at(offset);
        }

        /// get a slice of symbols
        inline const symbol_string_type &
        symbols(void) const throw() {
            return production->symbols;
        }

        Unbound<AlphaT, production_tag> operator~(void) throw() {
            return Unbound<AlphaT, production_tag>(this);
        }
    };

}}

#endif /* FLTL_OPAQUEPRODUCTION_HPP_ */
