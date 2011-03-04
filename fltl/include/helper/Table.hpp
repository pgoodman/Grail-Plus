/*
 * Table.hpp
 *
 *  Created on: Mar 4, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_TABLE_HPP_
#define FLTL_TABLE_HPP_

#include <cassert>

namespace fltl { namespace helper {

    template <typename T>
    class Table {
    private:
        const unsigned num_rows;
        const unsigned num_cols;
        T *slots;

    public:

        Table(const unsigned nr, const unsigned nc) throw()
            : num_rows(nr)
            , num_cols(nc)
            , slots(new T[nr * nc])
        { }

        ~Table(void) throw() {
            if(0 != slots) {
                delete [] slots;
                slots = 0;
            }
        }

        T &operator()(const unsigned row, const unsigned col) throw() {
            assert(row < num_rows);
            assert(col < num_cols);
            return slots[(row * num_rows) + col];
        }
    };

}}

#endif /* FLTL_TABLE_HPP_ */
