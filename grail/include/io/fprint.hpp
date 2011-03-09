/*
 * fprint.hpp
 *
 *  Created on: Feb 19, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
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
