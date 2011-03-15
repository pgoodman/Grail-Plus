/*
 * ParseTree.hpp
 *
 *  Created on: Mar 13, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_PARSETREE_HPP_
#define FLTL_PARSETREE_HPP_

#include <cassert>
#include <cstring>

#include "fltl/include/CFG.hpp"

namespace grail { namespace cfg {

    template <typename AlphaT>
    class ParseTree {
    public:

        typedef typename fltl::CFG<AlphaT> CFG;
        typedef typename CFG::variable_type variable_type;
        typedef typename CFG::terminal_type terminal_type;
        typedef typename CFG::symbol_type symbol_type;
        typedef typename CFG::production_type production_type;
        typedef typename CFG::alphabet_type alphabet_type;

        typedef ParseTree<AlphaT> self_type;

        symbol_type symbol;

        int num_children;

        union {
            self_type **slots;
            const alphabet_type *alpha;
        } data;

        self_type *parent;

    public:

        ParseTree(production_type &prod) throw()
            : symbol(prod.variable())
            , num_children(static_cast<int>(prod.length()))
            , parent(0)
        {
            memset(&data, 0, sizeof data);
            if(0 < num_children) {
                data.slots = new self_type *[num_children];
                memset(
                    data.slots,
                    0,
                    sizeof(self_type *) * static_cast<unsigned>(num_children)
                );
            }
        }

        ParseTree(terminal_type term, const alphabet_type *alpha_)
            : symbol(term)
            , num_children(-1)
            , parent(0)
        {
            data.alpha = alpha_;
        }

        ~ParseTree(void) throw() {
            if(-1 == num_children) {
                data.alpha = 0;
            } else if(0 < num_children) {
                for(int i(0); i < num_children; ++i) {
                    if(0 != data.slots[i]) {
                        delete (data.slots[i]);
                        data.slots[i] = 0;
                    }
                }
                delete [] (data.slots);
                data.slots = 0;
            }
            num_children = 0;
        }

        // adds children from right-to-left
        void add_child(self_type *child) throw() {
            assert(0 < num_children);

            for(int i(num_children); 0 != i--; ) {
                if(0 == data.slots[i]) {
                    data.slots[i] = child;
                    child->parent = this;
                    return;
                }
            }

            assert(false);
        }

        // can a child be added?
        bool can_add_child(void) const throw() {
            if(0 == num_children) {
                return false;
            } else if(-1 == num_children) {
                return 0 != data.alpha;
            } else {
                return 0 != data.slots[0];
            }
        }
    };

}}

#endif /* FLTL_PARSETREE_HPP_ */
