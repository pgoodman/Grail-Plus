/*
 * fprint.cpp
 *
 *  Created on: Feb 19, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_FPRINT_CPP_
#define FLTL_FPRINT_CPP_

#include "grail/include/io/fprint.hpp"

namespace grail { namespace io {

    void fprint(FILE *ff, char obj) throw() {
        fprintf(ff, "%c", obj);
    }
    void fprint(FILE *ff, unsigned char obj) throw() {
        fprintf(ff, "%u", static_cast<unsigned>(obj));
    }
    void fprint(FILE *ff, short obj) throw() {
        fprintf(ff, "%d", static_cast<int>(obj));
    }
    void fprint(FILE *ff, unsigned short obj) throw() {
        fprintf(ff, "%u", static_cast<unsigned>(obj));
    }
    void fprint(FILE *ff, int obj) throw() {
        fprintf(ff, "%d", obj);
    }
    void fprint(FILE *ff, unsigned obj) throw() {
        fprintf(ff, "%u", obj);
    }
    void fprint(FILE *ff, long obj) throw() {
        fprintf(ff, "%ld", obj);
    }
    void fprint(FILE *ff, unsigned long obj) throw() {
        fprintf(ff, "%lu", obj);
    }
    void fprint(FILE *ff, float obj) throw() {
        fprintf(ff, "%f", obj);
    }
    void fprint(FILE *ff, double obj) throw() {
        fprintf(ff, "%f", obj);
    }

    void fprint(FILE *ff, char *obj) throw() {
        fprintf(ff, "%s", obj);
    }
}}

#endif /* FLTL_FPRINT_CPP_ */
