/*
 * fprint.cpp
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

#ifndef FLTL_FPRINT_CPP_
#define FLTL_FPRINT_CPP_

#include "grail/include/io/fprint.hpp"

namespace grail { namespace io {

    int fprint(FILE *ff, char obj) throw() {
        return fprintf(ff, "%c", obj);
    }
    int fprint(FILE *ff, unsigned char obj) throw() {
        return fprintf(ff, "%u", static_cast<unsigned>(obj));
    }
    int fprint(FILE *ff, short obj) throw() {
        return fprintf(ff, "%d", static_cast<int>(obj));
    }
    int fprint(FILE *ff, unsigned short obj) throw() {
        return fprintf(ff, "%u", static_cast<unsigned>(obj));
    }
    int fprint(FILE *ff, int obj) throw() {
        return fprintf(ff, "%d", obj);
    }
    int fprint(FILE *ff, unsigned obj) throw() {
        return fprintf(ff, "%u", obj);
    }
    int fprint(FILE *ff, long obj) throw() {
        return fprintf(ff, "%ld", obj);
    }
    int fprint(FILE *ff, unsigned long obj) throw() {
        return fprintf(ff, "%lu", obj);
    }
    int fprint(FILE *ff, float obj) throw() {
        return fprintf(ff, "%f", obj);
    }
    int fprint(FILE *ff, double obj) throw() {
        return fprintf(ff, "%f", obj);
    }

    int fprint(FILE *ff, const char *obj) throw() {
        return fprintf(ff, "%s", obj);
    }
}}

#endif /* FLTL_FPRINT_CPP_ */
