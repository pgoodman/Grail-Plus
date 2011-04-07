/*
 * NFA_REMOVE_EPSILON.hpp
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

#ifndef FLTL_NFA_REMOVE_EPSILON_HPP_
#define FLTL_NFA_REMOVE_EPSILON_HPP_

#include <set>

#include "fltl/include/NFA.hpp"

namespace grail { namespace algorithm {

    template <typename AlphaT>
    class NFA_REMOVE_EPSILON {
    private:

        typedef fltl::NFA<AlphaT> NFA;

        FLTL_NFA_USE_TYPES(NFA);

    public:

        static void run(NFA &nfa) throw() {

            std::set<state_type> reached_states;

            symbol_type read;
            const symbol_type epsilon(nfa.epsilon());

            state_type A;
            generator_type states(nfa.search(~A));

            // epsilon transitions on A
            transition_type epsilon_transition;
            state_type B;
            generator_type epsilon_transitions(nfa.search(
                ~epsilon_transition,
                A,
                epsilon,
                ~B
            ));

            // all transitions on B
            transition_type reachable_transition;
            generator_type reachable_transitions(nfa.search(
                ~reachable_transition,
                B,
                nfa._,
                nfa._
            ));

            for(bool updated(false); states.match_next(); ) {
                reached_states.clear();
                reached_states.insert(A);

                for(updated = true; updated; ) {

                    updated = false;

                    // follow along an epsilon transition
                    for(epsilon_transitions.rewind();
                        epsilon_transitions.match_next(); ) {

                        // remove the transition
                        nfa.remove_transition(epsilon_transition);

                        // we've already reached this state by following
                        // an epsilon transition
                        if(0 != reached_states.count(B)) {
                            continue;
                        }

                        reached_states.insert(B);
                        for(reachable_transitions.rewind();
                            reachable_transitions.match_next(); ) {

                            read = reachable_transition.read();
                            nfa.add_transition(
                                A,
                                read,
                                reachable_transition.sink()
                            );

                            if(epsilon == read) {
                                updated = true;
                            }
                        }
                    }
                }
            }
        }
    };

}}

#endif /* FLTL_NFA_REMOVE_EPSILON_HPP_ */
