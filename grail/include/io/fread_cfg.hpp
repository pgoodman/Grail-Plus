/*
 * fread_cfg.hpp
 *
 *  Created on: Feb 19, 2011
 *      Author: Peter Goodman
 *     Version: $Id$
 *
 * Copyright 2011 Peter Goodman, all rights reserved.
 */

#ifndef FLTL_FREAD_CFG_HPP_
#define FLTL_FREAD_CFG_HPP_

#include "fltl/lib/CFG.hpp"

#include "grail/include/io/fread.hpp"
#include "grail/include/io/UTF8FileBuffer.hpp"

namespace grail { namespace io {

    template <typename AlphaT>
    bool fread(FILE *ff, fltl::lib::CFG<AlphaT> &cfg) throw() {
        if(0 == ff) {
            return false;
        }

        UTF8FileBuffer<2048> buffer(ff);

        (void) cfg;

        const char *ch(0);

        for(unsigned i(0); i < 100; ++i) {
            ch = buffer.read();
            if(0 == ch || '\0' == *ch) {
                break;
            }

            printf("line=%u col=%u %s\n", buffer.line(), buffer.column(), ch);
        }

        return true;
    }

}}

#endif /* FLTL_FREAD_CFG_HPP_ */
