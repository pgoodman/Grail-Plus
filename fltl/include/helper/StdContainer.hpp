/*
 * StdContainer.hpp
 *
 *  Created on: May 23, 2012
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef Grail_Plus_FLTL_HELPER_STDCONTAINER_HPP_
#define Grail_Plus_FLTL_HELPER_STDCONTAINER_HPP_

#include <set>
#include <list>
#include <vector>

namespace fltl { namespace helper {

    class not_a_container { };

    template <typename T>
    void add_to_container(not_a_container &, T &) throw() { }

    template <typename T>
    void add_to_container(std::set<T> &cont, T &elm) throw() {
        cont.insert(elm);
    }

    template <typename T>
    void add_to_container(std::list<T> &cont, T &elm) throw() {
        cont.push_back(elm);
    }

    template <typename T>
    void add_to_container(std::vector<T> &cont, T &elm) throw() {
        cont.push_back(elm);
    }
}}

#endif /* Grail_Plus_FLTL_HELPER_STDCONTAINER_HPP_ */
