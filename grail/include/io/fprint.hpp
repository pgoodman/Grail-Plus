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

    void fprint(FILE *ff, char obj) throw();
    void fprint(FILE *ff, unsigned char obj) throw();
    void fprint(FILE *ff, short obj) throw();
    void fprint(FILE *ff, unsigned short obj) throw();
    void fprint(FILE *ff, int obj) throw();
    void fprint(FILE *ff, unsigned obj) throw();
    void fprint(FILE *ff, long obj) throw();
    void fprint(FILE *ff, unsigned long obj) throw();
    void fprint(FILE *ff, float obj) throw();
    void fprint(FILE *ff, double obj) throw();
    void fprint(FILE *ff, const char *obj) throw();

}}

#endif /* FLTL_FPRINT_HPP_ */
