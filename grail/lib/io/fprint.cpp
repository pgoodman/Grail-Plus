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
