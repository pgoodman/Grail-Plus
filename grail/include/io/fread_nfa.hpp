/*
 * fread_nfa.hpp
 *
 *  Created on: Mar 8, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef FLTL_FREAD_NFA_HPP_
#define FLTL_FREAD_NFA_HPP_

#include <stdint.h>

#include "grail/include/io/fread.hpp"
#include "grail/include/io/fread_pda.hpp"
#include "grail/include/io/verbose.hpp"

#include "fltl/include/NFA.hpp"
#include "fltl/include/PDA.hpp"
#include "fltl/include/helper/UnsafeCast.hpp"

namespace grail { namespace io {

    template <typename AlphaT>
    bool fread(
        FILE *ff,
        fltl::NFA<AlphaT> &NFA,
        const char * const file_name
    ) throw() {
        fltl::PDA<AlphaT> *PDA(fltl::helper::unsafe_cast<fltl::PDA<AlphaT> *>(&NFA));
        return fread(ff, *PDA, file_name);
    }
}}

#endif /* FLTL_FREAD_NFA_HPP_ */
