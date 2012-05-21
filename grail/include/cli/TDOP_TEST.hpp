/*
 * TDOP_TEST.hpp
 *
 *  Created on: May 15, 2012
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef Grail_Plus_TDOP_TEST_HPP_
#define Grail_Plus_TDOP_TEST_HPP_

#include "fltl/include/TDOP.hpp"

#include "grail/include/io/CommandLineOptions.hpp"
#include "grail/include/io/fprint_tdop.hpp"

namespace grail { namespace cli {

    template <typename AlphaT>
    class TDOP_TEST {
    public:

        FLTL_TDOP_USE_TYPES(fltl::TDOP<AlphaT>);

        static const char * const TOOL_NAME;

        static void declare(io::CommandLineOptions &, bool) throw() { }

        static void help(void) throw() { }

        static int main(io::CommandLineOptions &) throw() {

            printf("hi\n");

            tdop_type parser;

            operator_type op;
            operator_string_type op_str;
            term_type term;
            symbol_type sym;
            category_type cat;
            rule_type rule;

            unsigned x(0);

            // formation of basic strings
            op + op;
            op + term;
            op + sym;
            op + cat;

            term + op;
            term + term;
            term + sym;
            term + cat;

            cat + op;
            cat + term;
            cat + sym;
            cat + cat;

            sym + op;
            sym + term;
            sym + sym;
            sym + cat;

            // concatenation to basic strings
            (op + op) + op;
            op + (term + op);

            (term + op) + term;
            term + (term + term);

            (cat + sym) + cat;
            cat + (cat + cat);

            (sym + term) + sym;
            sym + (sym + sym);

            // testing basic pattern constructs
            ~op;
            ~term;
            ~sym;
            ~cat;

            *sym;
            *(~sym);

            cat(10);
            cat(x);

            cat[x];
            (~cat)[x];

            parser.__(x);

            // pattern initializers
            cat--;
            (~cat)--;
            cat[x]--;
            (~cat)[x]--;

            // initial patterns
            cat --->* cat;
            cat --->* (~cat);
            cat --->* cat[x];
            cat --->* (~cat)[x];

            cat --->* op_str;
            cat --->* ~op_str;

            cat --->* sym;
            cat --->* *sym;
            cat --->* (~sym);
            cat --->* *(~sym);

            cat --->* term;
            cat --->* (~term);

            cat --->* op;
            cat --->* (~op);

            cat --->* parser._;
            cat --->* parser.__;
            cat --->* parser.__(x);

            // pattern extensions
            cat --->* cat + cat;
            cat --->* cat + (~cat);
            cat --->* cat + cat[x];
            cat --->* cat + (~cat)[x];

            cat --->* (~cat) + cat;
            cat --->* (~cat) + (~cat);
            cat --->* (~cat) + cat[x];
            cat --->* (~cat) + (~cat)[x];

            cat --->* cat[x] + cat;
            cat --->* cat[x] + (~cat);
            cat --->* cat[x] + cat[x];
            cat --->* cat[x] + (~cat)[x];

            cat --->* (~cat)[x] + cat;
            cat --->* (~cat)[x] + (~cat);
            cat --->* (~cat)[x] + cat[x];
            cat --->* (~cat)[x] + (~cat)[x];

            cat --->* op_str + op_str;
            cat --->* (~op_str) + (op_str) + (~op_str);

            category_type cat_unnamed = parser.add_category();
            category_type cat_foo = parser.get_category("FOO");
            category_type cat_bar = parser.get_category("BAR");
            category_type cat_foo2 = parser.get_category("FOO");

            assert(cat_foo == cat_foo2);
            assert(cat_bar != cat_foo);
            assert(cat_unnamed != cat_foo);
            assert(cat_unnamed != cat_bar);
            assert(cat == cat_unnamed);
            assert(3 == parser.num_categories());
            assert(cat == parser.get_initial_category());

            assert(0 == parser.num_symbols());

            symbol_type a(parser.get_symbol("a"));
            symbol_type ab(parser.get_symbol("ab"));
            symbol_type a2(parser.get_symbol("a"));

            assert(a == a2);
            assert(a != ab);
            assert(a2 != ab);
            assert(2 == parser.num_symbols());

            rule_type r1(parser.add_initial_rule(cat_foo, *a + cat_foo));
            rule_type r1_2(parser.add_initial_rule(cat_foo, *a + cat_foo));

            rule_type r2(parser.add_initial_rule(cat_foo, *a + cat_foo + cat_foo));
            rule_type r2_2(parser.add_initial_rule(cat_foo, *a + cat_foo + cat_foo));

            rule_type r3(parser.add_extension_rule(cat_bar, 0, *a + cat_foo + cat_foo));
            rule_type r3_2(parser.add_extension_rule(cat_bar, 0, *a + cat_foo + cat_foo));

            rule_type r4(parser.add_extension_rule(cat_bar, 1, *a + cat_foo + cat_foo));
            rule_type r4_2(parser.add_extension_rule(cat_bar, 1, *a + cat_foo + cat_foo));

            assert(r1 == r1_2);
            assert(r1 != r2);
            assert(r2 == r2_2);
            assert(r1 != r3);
            assert(r2 != r3);
            assert(r3 == r3_2);
            assert(r1 != r4);
            assert(r2 != r4);
            assert(r3 != r4);
            assert(r4 == r4_2);

            // test out pattern matching
            pattern_type p((~cat) --->* (~op_str));
            unsigned ub(0);

            bool r = p.match(r1);
            assert(r);
            assert(cat == cat_foo);
            assert(op_str == r1.operators());
            assert(op_str == (*a + cat_foo));

            r = p.match(r2);
            assert(r);
            assert(cat == cat_foo);
            assert(op_str == r2.operators());
            assert(op_str == (*a + cat_foo + cat_foo));

            r = p.match(r3);
            assert(!r);

            p = (~cat)[ub] --->* (~op_str);
            r = p.match(r3);
            assert(r);
            assert(cat == cat_bar);
            assert(op_str == r3.operators());

            p = (~cat) --->* (*(~sym)) + (~op_str);
            r = p.match(r1);
            assert(r);
            assert(cat == cat_foo);
            assert(sym == a);
            assert(op_str == cat_foo);

            generator_type cats(parser.search(~cat));
            printf("categories:\n");
            for(; cats.match_next(); ) {
                printf("%s\n", parser.get_name(cat));
            }

            generator_type syms(parser.search(~sym));
            printf("symbols:\n");
            for(; syms.match_next(); ) {
                printf("%s\n", parser.get_alpha(sym));
            }

            generator_type rules(parser.search(~rule));
            printf("rules:\n");
            for(; rules.match_next(); ) {
                printf("%s\n", parser.get_name(rule.category()));
            }

            generator_type rules_by_p(parser.search(~rule, p));
            printf("rules by p:\n");
            for(; rules_by_p.match_next(); ) {
                assert(cat == rule.category());

                printf("cat = %s; ", parser.get_name(cat));
                printf("sym = %s; ", parser.get_alpha(sym));
                printf("op_str = %u\n", op_str.length());
            }

            printf("\n\n\n");

            io::fprint(stdout, parser);


            printf("num_rules = %u\n", parser.num_rules());

            return 0;
        }
    };

    template <typename AlphaT>
    const char * const TDOP_TEST<AlphaT>::TOOL_NAME("tdop-test");
}}



#endif /* Grail_Plus_TDOP_TEST_HPP_ */
