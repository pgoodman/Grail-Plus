/*
 * Generator.hpp
 *
 *  Created on: May 18, 2012
 *      Author: petergoodman
 *     Version: $Id$
 */

#ifndef Grail_Plus_TDOP_GENERATOR_HPP_
#define Grail_Plus_TDOP_GENERATOR_HPP_

#define G(x)

namespace fltl { namespace tdop {

    /// handles generating over various TDOP objects. the semantics of copy
    /// construction and assignment are to move data, and nullify the old
    /// generator.
    template <typename AlphaT>
    class Generator {
    private:

        typedef Generator<AlphaT> self_type;

        friend class detail::CategoryGenerator<AlphaT>;
        friend class detail::SymbolGenerator<AlphaT>;
        friend class detail::RuleGenerator<AlphaT>;
        friend class detail::PatternGenerator<AlphaT,true>;
        friend class detail::PatternGenerator<AlphaT,false>;
        friend class TDOP<AlphaT>;

        FLTL_TDOP_USE_TYPES(TDOP<AlphaT>);

        // location/navigation information
        TDOP<AlphaT> *machine;
        tdop::Category<AlphaT> *category;

        union {
            tdop::Rule<AlphaT> *rule;
            unsigned symbol;
        } cursor;

        // object to which we are binding values
        union {
            symbol_type *symbol;
            category_type *category;
            rule_type *rule;
        } binder;

        // in case of a pattern matching generator
        detail::PatternData<AlphaT> *pattern;
        bool (*bind)(detail::PatternData<AlphaT> *, const OpaqueRule<AlphaT> &);

        void (*reset_)(self_type *);
        bool (*match_)(self_type *);
        void (*free_)(self_type *);

        bool has_been_used;

        static void default_reset(self_type *) { }
        static bool default_match(self_type *) { return false; }
        static void default_free(self_type *) { }

        void take(self_type *that) throw() {
            memcpy(this, that, sizeof *that);
            memset(&that, 0, sizeof *that);

            that->reset_ = &default_reset;
            that->match_ = &default_match;
            that->free_ = &default_free;
        }

    public:

        /// copying has the semantics of taking
        Generator(const self_type &that) throw() {
            take(const_cast<self_type *>(&that));
        }

        Generator(void) throw()
            : machine(0)
            , category(0)
            , pattern(0)
            , reset_(&default_reset)
            , match_(&default_match)
            , free_(&default_free)
            , has_been_used(false)
        {
            memset(&cursor, 0, sizeof cursor);
            memset(&binder, 0, sizeof binder);
        }

        ~Generator(void) throw() {
            if(has_been_used) {
                free_(this);
            }

            if(0 != pattern) {
                detail::PatternData<AlphaT>::decref(pattern);
                pattern = 0;
            }
        }

        self_type &operator=(const self_type &that) throw() {
            assert(0 == machine);
            take(const_cast<self_type *>(&that));
            return *this;
        }

        void rewind(void) throw() {
            reset_(this);
        }

        bool match_next(void) throw() {
            if(!has_been_used) {
                reset_(this);
                has_been_used = true;
            }
            return match_(this);
        }
    };

}}

namespace fltl { namespace tdop { namespace detail {

    /// generate over all categories of the TDOP machine
    template <typename AlphaT>
    class CategoryGenerator {
    private:

        friend class Generator<AlphaT>;
        friend class TDOP<AlphaT>;

        static void reset(Generator<AlphaT> *gen) throw() {
            OpaqueCategory<AlphaT> dummy;
            gen->category = 0;
            gen->cursor.symbol = 1;
            *(gen->binder.category) = dummy;
        }

        static bool match(Generator<AlphaT> *gen) throw() {
            if(0 == gen->category) {
                if(gen->cursor.symbol) {
                    gen->category = gen->machine->first_category();
                } else {
                    return false;
                }
            } else {
                Category<AlphaT> *next(gen->category->next);

                // let go of previous iteration's category
                Category<AlphaT>::decref(gen->category);

                gen->category = next;
            }

            // skip over deleted categories
            for(;;) {
                if(0 == gen->category) {
                    gen->cursor.symbol = 0; // signal that we're at the end
                    return false;
                }

                if(gen->category->is_deleted) {
                    gen->category = gen->category->next;
                    continue;
                }

                break;
            }

            OpaqueCategory<AlphaT> cat(gen->category);
            *(gen->binder.category) = cat;

            // hold on for next iteration
            Category<AlphaT>::incref(gen->category);

            return true;
        }

        static void free(Generator<AlphaT> *gen) throw() {
            if(0 != gen->category) {
                Category<AlphaT>::decref(gen->category);
                gen->category = 0;
            }

            gen->machine = 0;
        }
    };

    template <typename AlphaT>
    class SymbolGenerator {
    private:

        friend class TDOP<AlphaT>;

        typedef typename TDOP<AlphaT>::symbol_rep_type symbol_rep_type;

        static void reset(Generator<AlphaT> *gen) throw() {
            gen->cursor.symbol = 0;
        }

        static bool match(Generator<AlphaT> *gen) throw() {
            while(gen->cursor.symbol < gen->machine->num_symbols_) {
                ++(gen->cursor.symbol);

                const symbol_rep_type &rep(
                    gen->machine->symbol_map.get(gen->cursor.symbol)
                );

                if(symbol_rep_type::FREE != rep.kind) {
                     *(gen->binder.symbol) = Symbol<AlphaT>(gen->cursor.symbol);
                     return true;
                }
            }

            return false;
        }

        static void free(Generator<AlphaT> *gen) throw() {
            gen->machine = 0;
        }
    };

    template <typename AlphaT>
    class RuleGenerator {
    private:

        friend class TDOP<AlphaT>;
        friend class PatternGenerator<AlphaT,true>;
        friend class PatternGenerator<AlphaT,false>;

        typedef Category<AlphaT> category_type;
        typedef Rule<AlphaT> rule_type;

        static void reset(Generator<AlphaT> *gen) throw() {
            TDOP<AlphaT> *machine(gen->machine);
            free(gen);
            gen->machine = machine;
        }

        static void signal_end(category_type *&cat, rule_type *&rule) throw() {
            cat = 0;
            rule = helper::unsafe_cast<rule_type *>(1UL);
        }

        static bool at_end(rule_type *rule) throw() {
            const unsigned long rule_(helper::unsafe_cast<unsigned long>(rule));
            return 1UL == rule_;
        }

        static rule_type *first_rule(category_type *cat) throw() {
            if(0 == cat) {
                return 0;
            }

            if(0 != cat->first_initial_rule) {
                return cat->first_initial_rule;
            }

            return cat->first_extension_rule;
        }

        /// get the next rule within the same category
        static rule_type *next_cat_rule(category_type *cat, rule_type *rule) throw() {

            assert(0 != cat);

            if(0 == rule) {
                return 0;
            } else if(0 != rule->next) {
                return rule->next;

            // the rule was an initial rule; go to extension rules
            } else if(0 > rule->upper_bound) {
                return cat->first_extension_rule;
            }

            return 0;
        }

        /// get the current rule for the generator. we might be on a valid rule,
        /// or just before a valid rule.
        ///
        /// ref counts to cat and rule are held coming into this
        static bool curr(category_type *&cat_, rule_type *&rule_) throw() {

            category_type *cat(cat_);
            category_type *next_cat(0);

            rule_type *rule(rule_);
            rule_type *next_rule(0);

            for(; 0 != cat; cat = next_cat) {
                G( printf("curr::cat = %s\n", cat->name); )

                next_cat = cat->next;

                if(!cat->is_deleted) {
                    for(; 0 != rule; rule = next_rule) {
                        next_rule = next_cat_rule(cat, rule);

                        if(rule->is_deleted) {
                            continue;

                        // we found a rule
                        } else {
                            goto found_rule;
                        }
                    }
                }

                rule = first_rule(next_cat);
            }

            rule_type::decref(rule_);
            category_type::decref(cat_);
            signal_end(cat, rule_);
            return false;

        found_rule:

            assert(0 != rule);
            assert(0 != cat);

            if(cat_ != cat) {
                category_type::incref(cat);
                category_type::decref(cat_);
                cat_ = cat;
            }

            if(rule_ != rule) {
                rule_type::incref(rule);
                rule_type::decref(rule_);
                rule_ = rule;
            }

            return true;
        }

        /// move to any next rule; might not be a valid rule. Assumes that cat
        /// is non-null.
        ///
        /// both cat and rule come in with ref counts held
        static void next(category_type *&cat_, rule_type *&rule_) throw() {

            category_type *next_cat(cat_);
            rule_type *next_rule(next_cat_rule(next_cat, rule_));

            for(; 0 != next_cat; ) {
                G( printf("next::cat = %s\n", next_cat->name); )
                if(0 == next_rule) {
                    next_cat = next_cat->next;
                    next_rule = first_rule(next_cat);
                    continue;
                }
                goto found_rule;
            }

            rule_type::decref(rule_);
            category_type::decref(cat_);
            signal_end(cat_, rule_);
            return;

        found_rule:

            assert(0 != next_cat);
            assert(0 != next_rule);

            rule_type::incref(next_rule);
            rule_type::decref(rule_);

            if(next_cat != cat_) {
                category_type::incref(next_cat);
                category_type::decref(cat_);
                cat_ = next_cat;
            }

            rule_ = next_rule;
        }

        static bool match(Generator<AlphaT> *gen) throw() {
            G( printf("rule::match\n"); )

            // check if we're at the end; can only happen if at least one rule
            // has been generated. If there are no rules to generate, then
            // curr() will return false.
            if(at_end(gen->cursor.rule)) {
                return false;

            // initialize; might not be looking at something valid
            } else if(0 == gen->category) {
                G( printf(" | initializing\n"); )
                gen->category = gen->machine->first_category();
                gen->cursor.rule = first_rule(gen->category);

                // nothing to iterate over
                if(0 == gen->category) {
                    signal_end(gen->category, gen->cursor.rule);
                    return false;
                }

                rule_type::incref(gen->cursor.rule);
                category_type::incref(gen->category);
            }

            G( printf(" | looking for curr\n"); )

            if(curr(gen->category, gen->cursor.rule)) {

                tdop::OpaqueRule<AlphaT> bind_rule(gen->cursor.rule);

                // bind
                *(gen->binder.rule) = bind_rule;

                // move forward
                next(gen->category, gen->cursor.rule);
                return true;
            }

            return false;
        }

        static void free(Generator<AlphaT> *gen) throw() {

            // we've already decref'd everything, but rule will be non-null
            // because of signal_end.
            if(at_end(gen->cursor.rule)) {
                gen->cursor.rule = 0;
                gen->category = 0;

            // the generator is holding onto references, and was stopped and
            // destroyed before reaching the end of all rules
            } else {
                if(0 != gen->cursor.rule) {
                    rule_type::decref(gen->cursor.rule);
                    gen->cursor.rule = 0;
                }

                if(0 != gen->category) {
                    category_type::decref(gen->category);
                    gen->category = 0;
                }
            }

            gen->machine = 0;
        }
    };

    template <typename AlphaT, const bool category_is_bound>
    class PatternGenerator {
    private:

        friend class TDOP<AlphaT>;

        typedef Category<AlphaT> category_type;
        typedef Rule<AlphaT> rule_type;
        typedef RuleGenerator<AlphaT> rule_gen_type;

        static void reset(Generator<AlphaT> *gen) throw() {
            G( printf("pattern::reset\n"); )
            rule_gen_type::reset(gen);
        }

        static bool match(Generator<AlphaT> *gen) throw() {
            G( printf("pattern::match\n"); )
            // check if we're at the end; can only happen if at least one rule
            // has been generated. If there are no rules to generate, then
            // curr() will return false.
            if(rule_gen_type::at_end(gen->cursor.rule)) {
                G( printf(" | at end\n"); )
                return false;

            //  initialize; might not be looking at something valid
            } else if(0 == gen->category) {

                // navigate to the specific category of this pattern
                if(category_is_bound) {
                    gen->category = gen->machine->find_category(
                        *(gen->pattern->category)
                    );

                // go to the first category of the mchine
                } else {
                    gen->category = gen->machine->first_category();
                }

                if(0 == gen->category) {
                    rule_gen_type::signal_end(gen->category, gen->cursor.rule);
                    return false;
                }

                gen->cursor.rule = rule_gen_type::first_rule(gen->category);

                G( printf(" | initialized to cat(%p), rule(%p)\n",
                    static_cast<void *>(gen->category),
                    static_cast<void *>(gen->cursor.rule)
                ); )

                rule_type::incref(gen->cursor.rule);
                category_type::incref(gen->category);
            }

            G( printf(" | trying to find curr...\n"); )

            while(!rule_gen_type::at_end(gen->cursor.rule)) {
                G( printf(" |  | not at end, calling curr()\n"); )

                if(!rule_gen_type::curr(gen->category, gen->cursor.rule)) {
                    G( printf(" |  |  | curr() failed\n"); )
                    return false;
                }

                G( printf(" |  | cat(%p), rule(%p), cat-refcount(%p)\n",
                    static_cast<void *>(gen->category),
                    static_cast<void *>(gen->cursor.rule),
                    static_cast<void *>(&(gen->category->ref_count))
                ); )

                assert(0 != gen->cursor.rule);
                assert(0 != gen->cursor.rule->category);

                G( printf(" |  | rule on category %s is %s rule with %u operators\n", gen->cursor.rule->category->name, gen->cursor.rule->upper_bound < 0 ? "initial" : "extension", gen->cursor.rule->str.length()); )
                G( printf(" |  | trying pattern on rule %p\n", static_cast<void *>(gen->cursor.rule)); )

                // if we don't match the pattern, then move on
                if(!gen->bind(gen->pattern, gen->cursor.rule)) {
                    G( printf(" |  |  | pattern failed\n"); )
                    rule_gen_type::next(gen->category, gen->cursor.rule);
                    continue;
                }

                G( printf(" |  |  | pattern succeeded\n"); )

                // bind the rule, if we are doing that
                if(0 != gen->binder.rule) {
                    tdop::OpaqueRule<AlphaT> bind_rule(gen->cursor.rule);
                    *(gen->binder.rule) = bind_rule;
                }

                G( printf(" |  | moving to next and returning.\n"); )

                rule_gen_type::next(gen->category, gen->cursor.rule);

                G( printf(" |  |  | next cat(%p) rule(%p)\n",
                    static_cast<void *>(gen->category),
                    static_cast<void *>(gen->cursor.rule)
                ); )

                return true;
            }

            G( printf(" | failed to find curr.\n"); )

            return false;
        }

        static void free(Generator<AlphaT> *gen) throw() {
            G( printf("pattern::free\n"); )
            rule_gen_type::free(gen);
        }
    };

}}}



#endif /* Grail_Plus_TDOP_GENERATOR_HPP_ */
