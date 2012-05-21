/*
 * CFG_TO_TDOP.hpp
 *
 *  Created on: May 7, 2012
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef Grail_Plus_CFG_TO_TDOP_HPP_
#define Grail_Plus_CFG_TO_TDOP_HPP_


#include <cstdio>
#include <map>
#include <set>
#include <vector>
#include <list>
#include <algorithm>

#include "fltl/include/TDOP.hpp"

#include "grail/include/algorithm/CFG_REMOVE_EPSILON.hpp"
#include "grail/include/algorithm/CFG_REMOVE_UNITS.hpp"
#include "grail/include/algorithm/CFG_REMOVE_USELESS.hpp"

#include "grail/include/io/CommandLineOptions.hpp"
#include "grail/include/io/fread_cfg.hpp"
#include "grail/include/io/fprint_tdop.hpp"

namespace grail { namespace cli {

    template <typename AlphaT>
    class CFG_TO_TDOP {
    public:

        FLTL_CFG_USE_TYPES_PREFIX(fltl::CFG<AlphaT>, cfg);
        FLTL_TDOP_USE_TYPES_PREFIX(fltl::TDOP<AlphaT>, tdop);

        static const char * const TOOL_NAME;

        static void declare(io::CommandLineOptions &opt, bool in_help) throw() {
            io::option_type in(opt.declare("stdin", io::opt::OPTIONAL, io::opt::NO_VAL));
            if(!in_help) {
                if(in.is_valid()) {
                    opt.declare_max_num_positional(0);
                } else {
                    opt.declare_min_num_positional(1);
                    opt.declare_max_num_positional(1);
                }
            }
        }

        static void help(void) throw() {
            //  "  | |                              |                                             |"
            printf(
                "  %s:\n"
                "    Converts a context-free grammar (CFG) into a top-down operator precedence\n"
                "    (TDOP) grammar. This grammar formalism is similar to CGOL, but includes\n"
                "    categories where CFGs include productions.\n\n"
                "  basic use options for %s:\n"
                "    --stdin                        Read a CFG from stdin. Typing a new\n"
                "                                   line followed by Ctrl-D or Ctrl-Z will\n"
                "                                   close stdin.\n"
                "    <file>                         read in a CFG from <file>.\n\n",
                TOOL_NAME, TOOL_NAME
            );
        }

        static int main(io::CommandLineOptions &options) throw() {

            // run the tool
            io::option_type file;
            const char *file_name(0);

            FILE *fp(0);

            if(options["stdin"].is_valid()) {
                file = options["stdin"];
                fp = stdin;
                file_name = "<stdin>";
            } else {
                file = options[0U];
                file_name = file.value();
                fp = fopen(file_name, "r");
            }

            if(0 == fp) {

                options.error(
                    "Unable to open file containing context-free "
                    "grammar for reading."
                );
                options.note("File specified here:", file);

                return 1;
            }

            cfg_cfg_type cfg;
            int ret(0);

            if(io::fread(fp, cfg, file_name)) {
                run(cfg);
            } else {
                ret = 1;
            }

            fclose(fp);

            return ret;
        }

        typedef std::map<cfg_variable_type, std::set<cfg_variable_type> > reachable_lc_var_map;

        static void get_lc_reachable(
            cfg_cfg_type &cfg,
            reachable_lc_var_map &reachable_from_vars
        ) throw() {

            cfg_variable_type V, U, A;
            cfg_generator_type vars(cfg.search(~V));
            cfg_generator_type item_left_corners(cfg.search(U --->* (~A) + cfg.__));

            for(; vars.match_next(); ) {
                std::set<cfg_variable_type> &reachable(reachable_from_vars[V]);
                std::list<cfg_variable_type> work_list;

                // enqueue next initial left corners
                work_list.push_back(V);

                // follow through
                while(!work_list.empty()) {

                    U = work_list.back();
                    work_list.pop_back();

                    if(reachable.count(U)) {
                        continue;
                    }

                    reachable.insert(U);

                    // enqueue next ones
                    for(item_left_corners.rewind(); item_left_corners.match_next(); ) {
                        if(A == U) {
                            continue;
                        }

                        work_list.push_back(A);
                    }
                }
            }
        }

        /// ordering of variables, based on their reachability as left corners
        /// from some root variable, ordered by breadth-first depth.
        typedef std::map<cfg_symbol_type, unsigned> variable_ordering;
        typedef std::map<cfg_variable_type, variable_ordering> rooted_var_order;

        /// compute the immediate dominator of a state
        static cfg_variable_type immediate_dominator(
            std::map<cfg_variable_type, std::set<cfg_variable_type> > &dominators,
            const cfg_variable_type V
        ) throw() {
            std::set<cfg_variable_type> doms(dominators[V]);

            doms.erase(V);

            assert(!doms.empty());

            while(1U < doms.size()) {
                typename std::set<cfg_variable_type>::iterator it(doms.begin());
                cfg_variable_type first(*it);
                cfg_variable_type second(*++it);

                std::set<cfg_variable_type> &first_doms(dominators[first]);

                // second does not dominate first, i.e. first dominates second
                if(0U == first_doms.count(second)) {
                    doms.erase(first);
                } else {
                    doms.erase(second);
                }
            }

            return *(doms.begin());
        }

        struct dom_depth {
            cfg_variable_type var;
            unsigned depth;

            dom_depth(cfg_variable_type v, unsigned d) throw()
                : var(v)
                , depth(d)
            { }
        };

        /// compute the left corner dominators, rooted at a variable
        static void get_lc_dominator_numbering(
            cfg_cfg_type &cfg,
            variable_ordering &order,
            const std::set<cfg_variable_type> &reachable,
            const cfg_variable_type V
        ) throw() {

            std::map<cfg_variable_type, std::set<cfg_variable_type> > dominators;
            dominators[V].insert(V);

            cfg_variable_type U;
            cfg_variable_type W;
            cfg_variable_type X;
            cfg_generator_type vars(cfg.search(~U));
            cfg_generator_type predecessors(cfg.search((~U) --->* W + cfg.__));
            cfg_generator_type successors(cfg.search(W --->* (~X) + cfg.__));

            std::list<cfg_variable_type> work_list;

            // all variables (useful to dominators)
            std::set<cfg_variable_type> all_vars(reachable);
            all_vars.insert(V);

            // push the successors of the root (V) into the work list
            W = V;
            for(successors.rewind(); successors.match_next(); ) {
                work_list.push_back(X);
            }

            std::vector<cfg_variable_type> outgoing(all_vars.size());
            typename std::vector<cfg_variable_type>::iterator outgoing_end;

            // go through the work list
            for(; !work_list.empty(); ) {
                W = work_list.back();
                work_list.pop_back();

                std::set<cfg_variable_type> incoming(all_vars);

                bool has_predecessors = false;

                // for each predecessor U of W
                for(predecessors.rewind(); predecessors.match_next(); ) {

                    // make sure the predecessor is reachable from V, else we
                    // will deal with a graph with multiple roots, which would
                    // overly restrict the dom sets
                    if(!all_vars.count(U)) {
                        continue;
                    }

                    has_predecessors = true;

                    // initialize the dominator set of a predecessor
                    if(!dominators.count(U)) {
                        dominators[U] = all_vars;
                    }

                    const std::set<cfg_variable_type> &U_doms(dominators[U]);

                    // intersect U's dominators with the incoming set, storing
                    // the result in the outgoing list
                    outgoing_end = std::set_intersection(
                        U_doms.begin(), U_doms.end(),
                        incoming.begin(), incoming.end(),
                        outgoing.begin()
                    );

                    // update the incoming set
                    incoming.clear();
                    incoming.insert(outgoing.begin(), outgoing_end);
                }

                if(!has_predecessors) {
                    assert(V == W);
                    continue;
                }

                std::set<cfg_variable_type> &W_doms(dominators[W]);
                const size_t old_size(W_doms.size());

                // transfer
                W_doms.swap(incoming);
                W_doms.insert(W);

                // no change
                if(old_size == W_doms.size()) {
                    continue;
                }

                // change, need to add successors to work list
                for(successors.rewind(); successors.match_next(); ) {
                    work_list.push_back(X);
                }
            }

            // we have the dominators; build up an adjacency list for the
            // immediate dominator relation
            std::map<cfg_variable_type, std::list<cfg_variable_type> > adj_list;
            typename std::set<cfg_variable_type>::iterator reach_it(reachable.begin());

            for(; reach_it != reachable.end(); ++reach_it) {
                U = *reach_it;
                if(V != U) {
                    W = immediate_dominator(dominators, U);
                    adj_list[W].push_back(U);
                }
            }

            // number the nodes in the adjacency list based on their depth
            std::list<dom_depth> depth_list;
            depth_list.push_back(dom_depth(V, 0));

            while(!depth_list.empty()) {
                dom_depth entry = depth_list.back();
                depth_list.pop_back();

                order[entry.var] = entry.depth;

                std::list<cfg_variable_type> &succs(adj_list[entry.var]);
                typename std::list<cfg_variable_type>::iterator succs_it(succs.begin());

                for(; succs_it != succs.end(); ++succs_it) {
                    depth_list.push_back(dom_depth(*succs_it, entry.depth + 1));
                }
            }
        }


        struct bfs_list_item {
        public:
            cfg_variable_type var;
            unsigned depth;
            bool can_increase;

            bfs_list_item(void) throw() { }
            bfs_list_item(cfg_variable_type v, unsigned d, bool ci) throw()
                : var(v)
                , depth(d)
                , can_increase(ci)
            { }
        };

        static void get_lc_bfs_numbering(
            cfg_cfg_type &cfg,
            variable_ordering &bfs_order,
            cfg_variable_type V
        ) throw() {

            //variable_type V;
            cfg_variable_type A;
            cfg_symbol_string_type suffix;
            cfg_generator_type vars(cfg.search(~V));
            cfg_generator_type initial_left_corners(cfg.search(V --->* (~A) + (~suffix)));

            bfs_list_item item;
            cfg_generator_type item_left_corners(cfg.search((item.var) --->* (~A) + (~suffix)));

            //for(; vars.match_next(); ) {
            //std::map<symbol_type, unsigned> &bfs_order(rooted_var_order[V]);
            std::list<bfs_list_item> work_list;

            // enqueue next initial left corners
            for(initial_left_corners.rewind(); initial_left_corners.match_next(); ) {
                work_list.push_back(bfs_list_item(A, 0, suffix.is_empty()));
            }

            // follow through
            while(!work_list.empty()) {

                item = work_list.front();
                work_list.pop_front();

                if(bfs_order.count(item.var)) {
                    if(item.can_increase && bfs_order[item.var] < item.depth) {
                        bfs_order[item.var] = item.depth;
                    }
                    continue;
                }

                bfs_order[item.var] = item.depth;

                // enqueue next ones
                for(item_left_corners.rewind(); item_left_corners.match_next(); ) {

                    // units are considered at the same depth as they face
                    // the same rightward constraints as the variable
                    // generating them.
                    unsigned next_depth(item.depth + 1);
                    /*unsigned next_depth(item.depth);
                    if(!suffix.is_empty()) {
                        ++next_depth;
                    }*/

                    if(A == item.var) {
                        continue;
                    }

                    work_list.push_back(bfs_list_item(A, next_depth, suffix.is_empty()));
                }
            }
            //}
        }

        // look for productions P of the form V --> __ U W __, and convert
        // them to V --> __ U c W __ where c is a terminal representing a
        // followed-by constraint, i.e. c is a terminal left corner of W.
        static void remove_non_terminal_pairs(
            cfg_cfg_type &cfg,
            std::map<cfg_terminal_type, cfg_terminal_type> &followed_by_constraint,
            std::map<cfg_terminal_type, cfg_terminal_type> &reverse_followed_by_constraint
        ) throw() {

            cfg_variable_type V, U, W;
            cfg_terminal_type c;
            cfg_production_type P;
            cfg_symbol_string_type prefix, suffix;

            cfg_generator_type non_term_pair_finder(cfg.search((~V) --->* cfg.__ + (~U) + (~W) + cfg.__));
            cfg_generator_type non_term_pairs(cfg.search(~P, V --->* (~prefix) + (~U) + (~W) + (~suffix)));
            cfg_generator_type W_term_left_corners(cfg.search(W --->* (~c) + cfg.__));

            for(; non_term_pair_finder.match_next(); ) {

                for(bool changed(true); changed; ) {
                    changed = false;

                    for(non_term_pairs.rewind(); non_term_pairs.match_next(); ) {
                        cfg.remove_production(P);

                        for(W_term_left_corners.rewind(); W_term_left_corners.match_next(); ) {

                            if(!followed_by_constraint.count(c)) {
                                cfg_terminal_type c_constraint(cfg.add_variable_terminal());
                                followed_by_constraint[c] = c_constraint;
                                reverse_followed_by_constraint[c_constraint] = c;
                            }

                            changed = true;
                            cfg.add_production(V, prefix + U + followed_by_constraint[c] + W + suffix);
                        }
                    }
                }
            }
        }

        /// move productions that are transitively reachable through left corners
        /// into each variable
        static void raise_reachable_left_corners(
            cfg_cfg_type &cfg,
            reachable_lc_var_map &rooted_reachable
        ) throw() {
            cfg_variable_type V, U;
            cfg_symbol_string_type symbols;
            cfg_production_type P;

            cfg_generator_type vars(cfg.search(~V));
            cfg_generator_type reachable_prods(cfg.search(U --->* (~symbols)));
            cfg_generator_type units(cfg.search(~P, V --->* (~U)));

            for(; vars.match_next(); ) {

                std::set<cfg_variable_type> &reachable_from_V(rooted_reachable[V]);
                typename std::set<cfg_variable_type>::iterator reachable_it(reachable_from_V.begin());

                // add in all reachable productions
                for(; reachable_it != reachable_from_V.end(); ++reachable_it) {
                    U = *reachable_it;

                    if(V == U) {
                        continue;
                    }

                    for(reachable_prods.rewind(); reachable_prods.match_next(); ) {
                        cfg.add_production(V, symbols);
                    }
                }

                // remove all units
                for(units.rewind(); units.match_next(); ) {
                    cfg.remove_production(P);
                }
            }
        }

        /// compute the TDOP grammar of a CFG
        static void run(cfg_cfg_type &cfg) throw() {

            FILE *ff(stdout);
            tdop_tdop_type tdop;

            // for each variable V, find a breadth-first ordering of all nodes
            // reachable through immediate variable left corners of V.
            //rooted_var_order rooted_var_order;

            io::fprint(stdout, cfg);
            fprintf(stdout, "----------------------------------------\n");

            // replace every production that can be a left corner of some
            // variable to that variable
            cfg_variable_type V;
            cfg_variable_type U;
            cfg_production_type P;
            cfg_symbol_string_type symbols;
            cfg_generator_type vars(cfg.search(~V));

            // for each variable, add a category to the TDOP machine
            std::map<cfg_variable_type, tdop_category_type> categories;
            for(; vars.match_next(); ) {
                categories[V] = tdop.get_category(cfg.get_name(V));
            }

            variable_ordering order;
            reachable_lc_var_map rooted_reachable;

            // figure out every variable reachable through every other variable
            // by following along left corner variables
            get_lc_reachable(cfg, rooted_reachable);

            // number the variables
            std::set<cfg_variable_type> &reachable_root(rooted_reachable[cfg.get_start_variable()]);
            get_lc_dominator_numbering(cfg, order, reachable_root, cfg.get_start_variable());
            //get_lc_bfs_numbering(cfg, order, cfg.get_start_variable());

            // raise reachable left corner productions to be related to all
            // variables from which they can be reached
            raise_reachable_left_corners(cfg, rooted_reachable);

            cfg_generator_type lc_vars(cfg.search(~P, (~V) --->* (~U) + cfg.__));

            // eliminate useless productions, i.e. ones that can never be
            // executed because of the binding powers. This makes a special case
            // of direct left recursion.
            for(lc_vars.rewind(); lc_vars.match_next(); ) {
                if(order[V] >= order[U] && V != U) {
                    cfg.remove_production(P);
                }
            }

            /*

            // look for productions P of the form V --> __ U W __, and convert
            // them to V --> __ U c W __ where c is a terminal representing a
            // followed-by constraint, i.e. c is a terminal left corner of W.
            //
            // without adding a constraint (right now, lookahead is limited to
            // 1 terminal), this type of rule could never be matched as there
            // is nothing (i.e. no terminal) to use as an anchoring point for
            // the
            std::map<cfg_terminal_type, cfg_terminal_type> followed_by_constraint;
            std::map<cfg_terminal_type, cfg_terminal_type> reverse_followed_by_constraint;
            remove_non_terminal_pairs(cfg,
                followed_by_constraint,
                reverse_followed_by_constraint
            );

            io::fprint(stdout, cfg);
            fprintf(stdout, "----------------------------------------\n");

            // for each variable, generate a variable terminal. then replace the
            // left corners of each production which have the variable with the
            // variable terminal.
            std::map<unsigned, cfg_terminal_type> order_reps;
            std::map<cfg_variable_type, bool> is_left_recursive;

            for(lc_vars.rewind(); lc_vars.match_next(); ) {

                cfg_terminal_type U_rep;

                if(V == U) {
                    is_left_recursive[V] = true;
                }

                const unsigned ord(order[U]);
                if(order_reps.count(ord)) {
                    U_rep = order_reps[ord];
                } else {
                    U_rep = cfg.add_variable_terminal();
                    order[U_rep] = ord;
                    order_reps[ord] = U_rep;
                }

                cfg.remove_production(P);
                cfg.add_production(V, U_rep + P.symbols().substring(1));
            }

            // remove now unreachable variables
            algorithm::CFG_REMOVE_USELESS<AlphaT>::run(cfg);

            cfg_generator_type prods(cfg.search(V --->* (~symbols)));
            */
            /*
            // print out the TDOP machine
            for(vars.rewind(); vars.match_next(); ) {
                io::fprint(ff, cfg, V);
                fprintf(ff, "(n) {\n");

                const unsigned increment(is_left_recursive[V] ? 1U : 0U);

                for(prods.rewind(); prods.match_next(); ) {
                    fprintf(ff, "    ");

                    for(unsigned i(0); i < symbols.length(); ++i) {

                        if(0 < i) {
                            fprintf(ff, " ");

                        // left denotation
                        } else if(order.count(symbols[0])) {
                            fprintf(ff, "%u", order[symbols[i]] + increment);
                            continue;
                        }

                        // variable
                        if(symbols[i].is_variable()) {
                            io::fprint(ff, cfg, symbols[i]);

                            // this variable is a left corner of V
                            if(order.count(symbols[i])) {
                                fprintf(ff, "(%u)", order[symbols[i]]);

                            // not a left corner, thus no LC info.
                            } else {
                                fprintf(ff, "(n)");
                            }

                        // terminal, variable terminal. might be a followed-by
                        // constraint
                        } else {
                            cfg_terminal_type c(symbols[i]);
                            if(reverse_followed_by_constraint.count(c)) {
                                fprintf(ff, "?");
                                io::fprint(ff, cfg, reverse_followed_by_constraint[c]);
                            } else {
                                io::fprint(ff, cfg, c);
                            }
                        }
                    }
                    fprintf(ff, ";\n");
                }

                fprintf(ff, "}\n");
            }
            */

            // find left recursive variables; this affects the ordering
            std::map<cfg_variable_type, bool> is_left_recursive;
            for(lc_vars.rewind(); lc_vars.match_next(); ) {
                if(V == U) {
                    is_left_recursive[V] = true;
                }
            }

            cfg_generator_type all_prods(cfg.search((~V) --->* (~symbols)));
            for(; all_prods.match_next(); ) {


                // add an initial rule
                if(symbols[0].is_terminal()) {
                    tdop.add_initial_rule(
                        categories[V],
                        symbols_to_ops(cfg, tdop, order, symbols, 0U)
                    );

                // add an extension rule
                } else {
                    unsigned increment(0U);
                    if(symbols[0] == V && is_left_recursive[V]) {
                        increment = 1U;
                    }

                    tdop.add_extension_rule(
                        categories[V],
                        order[symbols[0]] + increment,
                        symbols_to_ops(cfg, tdop, order, symbols, 1U)
                    );
                }
            }

            io::fprint(stdout, cfg);
            fprintf(stdout, "----------------------------------------\n");

            determinize(tdop);

            io::fprint(ff, tdop);
        }

        /// convert a CFG symbol string into a TDOP operator string
        static tdop_operator_string_type symbols_to_ops(
            cfg_cfg_type &cfg,
            tdop_tdop_type &tdop,
            variable_ordering &order,
            const cfg_symbol_string_type &str,
            unsigned start
        ) throw() {
            tdop_operator_string_type ops;
            for(; start < str.length(); ++start) {

                // terminals, variable terminals
                if(str[start].is_terminal()) {
                    // TODO: handle variable terminals
                    cfg_terminal_type term(str[start]);
                    ops = ops + tdop.get_symbol(cfg.get_alpha(term));

                // variable with known ordering
                } else if(order.count(str[start])) {

                    cfg_variable_type var(str[start]);
                    tdop_operator_type op(
                        tdop.get_category(cfg.get_name(var)),
                        order[var]
                    );
                    ops = ops + op;

                // variable with an unknown ordering
                } else {

                    cfg_variable_type var(str[start]);
                    ops = ops + tdop.get_category(cfg.get_name(var));
                }
            }
            return ops;
        }

        /// remove unnecessary symbol predicates and rules containing symbol
        /// predicates that could never be matched.
        static bool remove_bad_symbol_predicates(tdop_tdop_type &tdop) throw() {
            tdop_symbol_type pred;
            tdop_symbol_type sym;
            tdop_operator_string_type prefix;
            tdop_operator_string_type suffix;
            tdop_rule_type R;
            tdop_category_type C;

            unsigned upper_bound(0);
            bool changed(false);

            // update bad initial rules
            tdop_generator_type initial_rules(tdop.search(
                ~R,
                (~C) --->* (~prefix) + (*(~pred)) + (~sym) + (~suffix)
            ));
            for(; initial_rules.match_next(); ) {
                tdop.remove_rule(R);
                if(sym == pred) {
                    changed = true;
                    tdop.add_initial_rule(C, prefix + sym + suffix);
                }
            }

            // update bad extension rules
            tdop_generator_type extension_rules(tdop.search(
                ~R,
                (~C)[upper_bound] --->* (~prefix) + (*(~pred)) + (~sym) + (~suffix)
            ));
            for(; extension_rules.match_next(); ) {
                tdop.remove_rule(R);
                if(sym == pred) {
                    changed = true;
                    tdop.add_extension_rule(C, upper_bound, prefix + sym + suffix);
                }
            }

            return changed;
        }

        /// given an operator string, return the maximal prefix operator string
        /// of symbol predicates.
        static tdop_operator_string_type
        longest_predicate_prefix(tdop_operator_string_type ops) throw() {
            tdop_operator_string_type prefix;

            for(unsigned i(0); i < ops.length(); ++i) {
                if(ops[i].is_category()) {
                    break;
                }

                prefix = prefix + ops[i];
            }

            return prefix;
        }

        /// go look for every extension rule C[upper_bound] --> D ..., where
        /// D is an (un)bound category. Such rules are non-deterministic, as
        /// we have no way of knowing what symbol we should pivot on
        static bool determinize_initial_pivots(tdop_tdop_type &tdop) throw() {
            bool changed(false);

            tdop_category_type C;
            tdop_category_type D;

            tdop_operator_type first;

            tdop_operator_string_type suffix;
            tdop_operator_string_type prefix;

            tdop_rule_type R;
            unsigned upper_bound(0);

            tdop_generator_type extension_rules(tdop.search(
                ~R,
                (~C)[upper_bound] --->* (~first) + (~suffix)
            ));

            tdop_generator_type related_initial_rules(tdop.search(
                D --->* (~prefix)
            ));

            for(extension_rules.rewind(); extension_rules.match_next(); ) {
                if(!first.match(D)) {
                    continue;
                }

                changed = true;
                tdop.remove_rule(R);

                for(related_initial_rules.rewind(); related_initial_rules.match_next(); ) {
                    tdop.add_initial_rule(C, longest_predicate_prefix(prefix) + suffix);
                }
            }

            return changed;
        }

        static void determinize(tdop_tdop_type &tdop) throw() {
            for(bool changed(true); changed; ) {
                changed = false;
                changed = determinize_initial_pivots(tdop) || changed;

                while(remove_bad_symbol_predicates(tdop)) {
                    changed = true;
                }
            }
        }
    };

    template <typename AlphaT>
    const char * const CFG_TO_TDOP<AlphaT>::TOOL_NAME("cfg-to-tdop");
}}



#endif /* Grail_Plus_CFG_TO_TDOP_HPP_ */
