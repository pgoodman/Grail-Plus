/*
 * fprint.hpp
 *
 *  Created on: Feb 19, 2011
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

#ifndef FLTL_FPRINT_HPP_
#define FLTL_FPRINT_HPP_

#include <cstdio>

namespace grail { namespace io {

    int fprint(FILE *ff, char obj) throw();
    int fprint(FILE *ff, unsigned char obj) throw();
    int fprint(FILE *ff, short obj) throw();
    int fprint(FILE *ff, unsigned short obj) throw();
    int fprint(FILE *ff, int obj) throw();
    int fprint(FILE *ff, unsigned obj) throw();
    int fprint(FILE *ff, long obj) throw();
    int fprint(FILE *ff, unsigned long obj) throw();
    int fprint(FILE *ff, float obj) throw();
    int fprint(FILE *ff, double obj) throw();
    int fprint(FILE *ff, const char *obj) throw();

}}

#endif /* FLTL_FPRINT_HPP_ */
