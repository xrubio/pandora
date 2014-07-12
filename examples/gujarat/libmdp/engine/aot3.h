/*
 *  Copyright (C) 2011 Universidad Simon Bolivar
 * 
 *  Permission is hereby granted to distribute this software for
 *  non-commercial research purposes, provided that this copyright
 *  notice is included with any such distribution.
 *  
 *  THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 *  EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE
 *  SOFTWARE IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU
 *  ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.
 *  
 *  Blai Bonet, bonet@ldc.usb.ve
 *
 */

#ifndef AOT_H
#define AOT_H

#include "policy.h"
#include "bdd_priority_queue.h"

#include <iostream>
#include <cassert>
#include <limits>
#include <vector>
#include <queue>

//#define DEBUG
#define USE_BDD_PQ

namespace Policy {

namespace AOT {

// Forward references
template<typename T> class aot_t;
template<typename T> class max_depth_priority_queue_t;

////////////////////////////////////////////////
//
// AND/OR Tree
//

template<typename T> struct node_t {
    float value_;
    float delta_;
    unsigned depth_;
    unsigned nsamples_;
    bool in_best_policy_;
    bool in_queue_;
    bool in_pq_;

    node_t(float value = 0, float delta = 0, unsigned depth = 0)
      : value_(value), delta_(delta), depth_(depth), nsamples_(0),
        in_best_policy_(false), in_queue_(false), in_pq_(false) { }
    virtual ~node_t() { }

    virtual void print(std::ostream &os, bool indent = true) const = 0;
    virtual void expand(const aot_t<T> *policy,
                        max_depth_priority_queue_t<T> &nodes_for_propagation) = 0;
    virtual void propagate(const aot_t<T> *policy) = 0;
};

template<typename T> struct state_node_t;
template<typename T> struct action_node_t : public node_t<T> {
    Problem::action_t action_;
    float action_cost_;
    state_node_t<T> *parent_;
    std::vector<std::pair<float, state_node_t<T>*> > children_;

    action_node_t(Problem::action_t action, unsigned depth = 0)
      : node_t<T>(0, 0, depth), action_(action), action_cost_(0), parent_(0) { }
    virtual ~action_node_t() { }

    bool is_leaf() const { return children_.empty(); }
    void update_value(float discount) {
        node_t<T>::value_ = 0;
        for( unsigned i = 0, isz = children_.size(); i < isz; ++i ) {
            node_t<T>::value_ +=
              children_[i].first * children_[i].second->value_;
        }
        node_t<T>::value_ =
          action_cost_ + discount * node_t<T>::value_;
    }

    virtual void print(std::ostream &os, bool indent = true) const {
        if( indent ) os << std::setw(2 * node_t<T>::depth_) << "";
        os << "[action=" << action_
           << ",value=" << node_t<T>::value_
           << ",delta=" << node_t<T>::delta_
           << "]";
    }
    virtual void expand(const aot_t<T> *policy,
                        max_depth_priority_queue_t<T> &nodes_for_propagation) {
        policy->expand(this, nodes_for_propagation);
    }
    virtual void propagate(const aot_t<T> *policy) {
        policy->propagate(this);
    }
};

template<typename T> struct state_node_t : public node_t<T> {
    const T state_;
    bool is_goal_;
    bool is_dead_end_;
    int best_action_;
    std::vector<std::pair<int, action_node_t<T>*> > parents_;
    std::vector<action_node_t<T>*> children_;

    state_node_t(const T &state, unsigned depth = 0)
      : node_t<T>(0, 0, depth), state_(state), is_goal_(false),
        is_dead_end_(false), best_action_(Problem::noop) { }
    virtual ~state_node_t() { }

    Problem::action_t best_action() const {
        return best_action_ == Problem::noop ?
          Problem::noop : children_[best_action_]->action_;
    }

    bool is_leaf() const {
        //return is_dead_end_ || (!is_goal_ && children_.empty());
        return is_dead_end_ || is_goal_ || children_.empty();
    }
    void update_value() {
        assert(!is_goal_);
        if( !is_dead_end_ ) {
            node_t<T>::value_ = std::numeric_limits<float>::max();
            for( unsigned i = 0, isz = children_.size(); i < isz; ++i ) {
                float child_value = children_[i]->value_;
                if( child_value < node_t<T>::value_ ) {
                    node_t<T>::value_ = child_value;
                    best_action_ = i;
                }
            }
        }
    }

    virtual void print(std::ostream &os, bool indent = true) const {
        if( indent ) os << std::setw(2 * node_t<T>::depth_) << "";
        os << "[state=" << state_
           << ",depth=" << node_t<T>::depth_
           << ",best_action=" << best_action()
           << ",#pa=" << parents_.size()
           << ",#chld=" << children_.size()
           << ",value=" << node_t<T>::value_
           << ",delta=" << node_t<T>::delta_
           << "]";
    }
    virtual void expand(const aot_t<T> *policy,
                        max_depth_priority_queue_t<T> &nodes_for_propagation) {
        policy->expand(this, nodes_for_propagation);
    }
    virtual void propagate(const aot_t<T> *policy) {
        policy->propagate(this);
    }
};

////////////////////////////////////////////////
//
// Hash Table
//

template<typename T> struct map_functions_t {
    bool operator()(const std::pair<const T*, unsigned> &p1,
                    const std::pair<const T*, unsigned> &p2) const {
        return (p1.second == p2.second) && (*p1.first == *p2.first);
    }
    size_t operator()(const std::pair<const T*, unsigned> &p) const {
        return p.first->hash();
    }
};

template<typename T> class hash_t :
  public std::tr1::unordered_map<std::pair<const T*, unsigned>,
                                 state_node_t<T>*,
                                 map_functions_t<T>,
                                 map_functions_t<T> > {
  public:
    typedef typename std::tr1::unordered_map<std::pair<const T*, unsigned>,
                                             state_node_t<T>*,
                                             map_functions_t<T>,
                                             map_functions_t<T> >
            base_type;
    typedef typename base_type::const_iterator const_iterator;
    const_iterator begin() const { return base_type::begin(); }
    const_iterator end() const { return base_type::end(); }

  public:
    hash_t() { }
    virtual ~hash_t() { }
    void print(std::ostream &os) const {
        for( const_iterator it = begin(); it != end(); ++it ) {
            os << "(" << it->first.first << "," << it->first.second << ")" << std::endl;
        }
    }
    void clear() {
        for( const_iterator it = begin(); it != end(); ++it ) {
            state_node_t<T> *s_node = it->second;
            for( int i = 0, isz = s_node->children_.size(); i < isz; ++i ) {
                delete s_node->children_[i];
            }
            delete it->second;
        }
        base_type::clear();
    }
};

////////////////////////////////////////////////
//
// Priority Queues
//

template<typename T> struct min_priority_t {
    bool operator()(const node_t<T> *n1, const node_t<T> *n2) const {
        float v1 = fabs(n1->delta_), v2 = fabs(n2->delta_);
        return v1 > v2;
    }
};

template<typename T> struct max_priority_t {
    bool operator()(const node_t<T> *n1, const node_t<T> *n2) const {
        float v1 = fabs(n1->delta_), v2 = fabs(n2->delta_);
        return v2 > v1;
    }
};

template<typename T> struct max_depth_t {
    bool operator()(const node_t<T> *n1, const node_t<T> *n2) const {
        return n2->depth_ > n1->depth_;
    }
};

template<typename T> class priority_queue_t :
  public std::priority_queue<node_t<T>*,
                             std::vector<node_t<T>*>,
                             min_priority_t<T> > {
};

template<typename T> class bdd_priority_queue_t :
  public std::bdd_priority_queue<node_t<T>*,
                                 min_priority_t<T>,
                                 max_priority_t<T> > {
  public:
    bdd_priority_queue_t(unsigned capacity)
      : std::bdd_priority_queue<node_t<T>*,
                                min_priority_t<T>,
                                max_priority_t<T> >(capacity) { }
};

template<typename T> class max_depth_priority_queue_t :
  public std::priority_queue<node_t<T>*,
                             std::vector<node_t<T>*>,
                             max_depth_t<T> > {
};

////////////////////////////////////////////////
//
// Policy
//

template<typename T> class aot_t : public improvement_t<T> {
  protected:
    unsigned width_;
    unsigned depth_bound_;
    float parameter_;
    bool delayed_evaluation_;
    unsigned expansions_per_iteration_;
    unsigned leaf_nsamples_;
    unsigned delayed_evaluation_nsamples_;
    int leaf_selection_strategy_;
    mutable unsigned num_nodes_;
    mutable hash_t<T> table_;

#ifdef USE_BDD_PQ
    mutable bdd_priority_queue_t<T> inside_bdd_priority_queue_;
    mutable bdd_priority_queue_t<T> outside_bdd_priority_queue_;
#else
    mutable priority_queue_t<T> inside_priority_queue_;
    mutable priority_queue_t<T> outside_priority_queue_;
#endif
    mutable float from_inside_;
    mutable float from_outside_;

    mutable node_t<T> *random_leaf_; // only used for random leaf selection strategy

    mutable unsigned total_number_expansions_;
    mutable unsigned total_evaluations_;

    // abstraction of selection strategy
    void (aot_t::*setup_expansion_loop_ptr_)(state_node_t<T>*) const;
    void (aot_t::*prepare_next_expansion_iteration_ptr_)(state_node_t<T>*) const;
    bool (aot_t::*exist_nodes_to_expand_ptr_)() const;
    node_t<T>* (aot_t::*select_node_for_expansion_ptr_)(state_node_t<T>*) const;
    void (aot_t::*clear_internal_state_ptr_)() const;

  public:
    aot_t(const policy_t<T> &base_policy,
          unsigned width,
          unsigned depth_bound,
          float parameter,
          bool delayed_evaluation,
          unsigned expansions_per_iteration,
          unsigned leaf_nsamples,
          unsigned delayed_evaluation_nsamples,
          int leaf_selection_strategy)
      : improvement_t<T>(base_policy),
        width_(width),
        depth_bound_(depth_bound),
        parameter_(parameter),
        delayed_evaluation_(delayed_evaluation),
        expansions_per_iteration_(expansions_per_iteration),
        leaf_nsamples_(leaf_nsamples),
        delayed_evaluation_nsamples_(delayed_evaluation_nsamples),
        leaf_selection_strategy_(leaf_selection_strategy),
        num_nodes_(0),
#ifdef USE_BDD_PQ
        inside_bdd_priority_queue_(expansions_per_iteration),
        outside_bdd_priority_queue_(expansions_per_iteration),
#endif
        from_inside_(0),
        from_outside_(0),
        random_leaf_(0),
        total_number_expansions_(0),
        total_evaluations_(0) {
        clear_leaf_selection_strategy();
        set_leaf_selection_strategy(leaf_selection_strategy_);
    }
    virtual ~aot_t() { }

    virtual Problem::action_t operator()(const T &s) const {
        // initialize tree and setup expansion loop for selection strategy
        ++policy_t<T>::decisions_;
        clear();
        state_node_t<T> *root = fetch_node(s, 0).first;
        (this->*setup_expansion_loop_ptr_)(root);

        // expand leaves and propagate values
        unsigned expanded = 0;
        max_depth_priority_queue_t<T> nodes_for_propagation;
        for( unsigned i = 0; (i < width_) && (this->*exist_nodes_to_expand_ptr_)(); ) {
            assert(nodes_for_propagation.empty());

            unsigned expanded_in_iteration = 0;
            while( (i < width_) &&
                   (expanded_in_iteration < expansions_per_iteration_) &&
                   (this->*exist_nodes_to_expand_ptr_)() ) {
                select_and_expand(root, nodes_for_propagation);
                ++expanded_in_iteration;
                ++i;
            }

            // propagate values up the tree
            while( !nodes_for_propagation.empty() ) {
                node_t<T> *node = nodes_for_propagation.top();
                nodes_for_propagation.pop();
                //std::cout << "pop: " << node->depth_ << std::endl;
                propagate_values(node);
            }

            expanded += expanded_in_iteration;
            (this->*prepare_next_expansion_iteration_ptr_)(root);
        }
        (this->*clear_internal_state_ptr_)();

        assert((width_ == 0) ||
               ((root != 0) &&
                policy_t<T>::problem().applicable(s, root->best_action())));
        assert(expanded <= width_);

        // select best action
        return width_ == 0 ?
          improvement_t<T>::base_policy_(s) : root->best_action();
    }

    virtual const policy_t<T>* clone() const {
        return new aot_t(improvement_t<T>::base_policy_,
                         width_,
                         depth_bound_,
                         parameter_,
                         delayed_evaluation_,
                         expansions_per_iteration_,
                         leaf_nsamples_,
                         delayed_evaluation_nsamples_,
                         leaf_selection_strategy_);
    }

    virtual void print_stats(std::ostream &os) const {
        os << "stats: policy-type=aot(width="
           << width_ << ",depth="
           << depth_bound_ << ",par="
           << parameter_ << ")" << std::endl;
        os << "stats: decisions=" << policy_t<T>::decisions_ << std::endl;
        os << "stats: %in=" << from_inside_ / (from_inside_ + from_outside_)
           << ", %out=" << from_outside_ / (from_inside_ + from_outside_)
           << std::endl;
        os << "stats: #expansions=" << total_number_expansions_
           << ", #evaluations=" << total_evaluations_
           << std::endl;
        improvement_t<T>::base_policy_.print_stats(os);
    }

    void print_tree(std::ostream &os) const {
        std::cout << "error: not yet implemented" << std::endl;
        assert(0);
    }

    // clear data structures
    void clear_table() const {
        table_.clear();
    }
    void clear() const {
        clear_table();
        num_nodes_ = 0;
    }

    // lookup a node in hash table; if not found, create a new entry.
    std::pair<state_node_t<T>*, bool> fetch_node(const T &state, unsigned depth) const {
        typename hash_t<T>::iterator it =
          table_.find(std::make_pair(&state, depth));
        if( it == table_.end() ) {
            ++num_nodes_;
            state_node_t<T> *node = new state_node_t<T>(state, depth);
            table_.insert(std::make_pair(std::make_pair(&node->state_, depth), node));
            if( policy_t<T>::problem().terminal(state) ) {
                node->value_ = 0;
                node->is_goal_ = true;
            } else if( policy_t<T>::problem().dead_end(state) ) {
                node->value_ = policy_t<T>::problem().dead_end_value();
                node->is_dead_end_ = true;
            } else {
                node->value_ = evaluate(state, depth);
                node->nsamples_ = leaf_nsamples_;
            }
            return std::make_pair(node, false);
        } else {
            bool re_evaluated = false;
            if( it->second->is_leaf() && !it->second->is_dead_end_ && !it->second->is_goal_ ) {
                // resample: throw other rollouts to get better estimation
                float oval = it->second->value_;
                float nval = oval * it->second->nsamples_ + evaluate(state, depth);
                it->second->nsamples_ += leaf_nsamples_;
                it->second->value_ = nval / it->second->nsamples_;
                re_evaluated = true;
            }
            return std::make_pair(it->second, re_evaluated);
        }
    }

    // expansion of state and action nodes. The binding of appropriate method
    // is done at run-time with virtual methods
    void select_and_expand(state_node_t<T> *root,
                           max_depth_priority_queue_t<T> &nodes_for_propagation) const {
        ++total_number_expansions_;
        node_t<T> *node = (this->*select_node_for_expansion_ptr_)(root);
        node->expand(this, nodes_for_propagation);
    }
    void expand(action_node_t<T> *a_node,
                max_depth_priority_queue_t<T> &nodes_for_propagation,
                bool picked_from_queue = true) const {
        assert(a_node->is_leaf());
        assert(!a_node->parent_->is_goal_);
        assert(!a_node->parent_->is_dead_end_);
        std::vector<std::pair<T, float> > outcomes;
        policy_t<T>::problem().next(a_node->parent_->state_,
                                    a_node->action_,
                                    outcomes);
        a_node->children_.reserve(outcomes.size());
        for( int i = 0, isz = outcomes.size(); i < isz; ++i ) {
            const T &state = outcomes[i].first;
            float prob = outcomes[i].second;
            std::pair<state_node_t<T>*, bool> p =
              fetch_node(state, 1 + a_node->depth_);
            if( p.second ) {
                assert(p.first->is_leaf());
                nodes_for_propagation.push(p.first);
                //std::cout << "push: " << p.first->depth_ << std::endl;
            }
            p.first->parents_.push_back(std::make_pair(i, a_node));
            a_node->children_.push_back(std::make_pair(prob, p.first));
            a_node->value_ += prob * p.first->value_;
        }
        a_node->value_ = a_node->action_cost_ +
          policy_t<T>::problem().discount() * a_node->value_;
        nodes_for_propagation.push(a_node);
        //std::cout << "push: " << a_node->depth_ << std::endl;

        // re-sample sibling action nodes that are still leaves
        if( picked_from_queue ) {
            state_node_t<T> *parent = a_node->parent_;
            assert(a_node->depth_ == parent->depth_);
            const T &state = parent->state_;
            for( int i = 0, isz = parent->children_.size(); i < isz; ++i ) {
                action_node_t<T> *sibling = parent->children_[i];
                if( sibling->is_leaf() ) {
                    assert(sibling != a_node);
                    float oval = (sibling->value_ - sibling->action_cost_) /
                      policy_t<T>::problem().discount();
                    float eval = evaluate(state, sibling->action_, 1 + parent->depth_);
                    float nval = oval * sibling->nsamples_ + eval;
                    sibling->nsamples_ +=
                      delayed_evaluation_nsamples_ * leaf_nsamples_;
                    sibling->value_ = sibling->action_cost_ +
                      policy_t<T>::problem().discount() * nval / sibling->nsamples_;

#ifdef DEBUG
                    std::cout << "sibling re-sampled: "
                              << "num=" << sibling->nsamples_
                              << std::endl;
#endif
                }
            }
        }
    }
    void expand(state_node_t<T> *s_node,
                max_depth_priority_queue_t<T> &nodes_for_propagation) const {
        assert(s_node->is_leaf());
        assert(!s_node->is_goal_);
        assert(!s_node->is_dead_end_);
        s_node->children_.reserve(policy_t<T>::problem().number_actions(s_node->state_));
        for( Problem::action_t a = 0; a < policy_t<T>::problem().number_actions(s_node->state_); ++a ) {
            if( policy_t<T>::problem().applicable(s_node->state_, a) ) {
                // create node for this action
                ++num_nodes_;
                action_node_t<T> *a_node = new action_node_t<T>(a, s_node->depth_);
                a_node->action_cost_ = policy_t<T>::problem().cost(s_node->state_, a);
                a_node->parent_ = s_node;
                s_node->children_.push_back(a_node);

                // expand node
                if( !delayed_evaluation_ ) {
                    expand(a_node, nodes_for_propagation, false);
                } else {
                    // instead of full-width expansion to calculate value,
                    // estimate it by sampling states and applying rollouts
                    // of base policy
                    float eval = evaluate(s_node->state_, a, 1 + s_node->depth_);
                    a_node->value_ = a_node->action_cost_ +
                      policy_t<T>::problem().discount() * eval;
                    a_node->nsamples_ =
                      delayed_evaluation_nsamples_ * leaf_nsamples_;
                }
            }
        }
        nodes_for_propagation.push(s_node);
        //std::cout << "push: " << s_node->depth_ << std::endl;
    }

    // propagate new values bottom-up using BFS and stopping when values changes no further
    void propagate_values(node_t<T> *node) const {
        node->propagate(this);
    }
    void propagate(action_node_t<T> *a_node) const {
        assert(a_node->parent_ != 0);
        propagate(a_node->parent_);
    }
    void propagate(state_node_t<T> *s_node) const {
        std::deque<state_node_t<T>*> queue;
        queue.push_back(s_node);
        s_node->in_queue_ = true;
        while( !queue.empty() ) {
            state_node_t<T> *s_node = queue.front();
            queue.pop_front();
            s_node->in_queue_ = false;
            float old_value = s_node->value_;
            if( !s_node->is_leaf() ) s_node->update_value();
            if( s_node->is_leaf() || (old_value != s_node->value_) ) {
                for( int i = 0, isz = s_node->parents_.size(); i < isz; ++i ) {
                    action_node_t<T> *a_node = s_node->parents_[i].second;
                    float old_value = a_node->value_;
                    a_node->update_value(policy_t<T>::problem().discount());
                    assert(a_node->parent_ != 0);
                    if( !a_node->parent_->in_queue_ && (a_node->value_ != old_value) ) {
                        queue.push_back(a_node->parent_);
                        a_node->parent_->in_queue_ = true;
                    }
                }
            }
        }
    }

    // evaluate a state with base policy, and evaluate an action node by
    // sampling states
#ifdef EXPERIMENT
    float evaluate(const T &s, unsigned depth) const {
        extern const Heuristic::heuristic_t<T> *global_heuristic;
        total_evaluations_ += leaf_nsamples_;
        if( depth >= depth_bound_ ) {
            return 0;
        } else if( global_heuristic != 0 ) {
            return global_heuristic->value(s);
        } else {
            return Evaluation::evaluation(improvement_t<T>::base_policy_, s,
                                          leaf_nsamples_, depth_bound_ - depth);
        }
    }
#else
    float evaluate(const T &s, unsigned depth) const {
        total_evaluations_ += leaf_nsamples_;
        return depth < depth_bound_ ?
          Evaluation::evaluation(improvement_t<T>::base_policy_, s,
                                 leaf_nsamples_, depth_bound_ - depth) : 0;
    }
#endif
    float evaluate(const T &state, Problem::action_t action, unsigned depth) const {
        float value = 0;
        for( unsigned i = 0; i < delayed_evaluation_nsamples_; ++i ) {
            std::pair<T, bool> sample =
              policy_t<T>::problem().sample(state, action);
            value += evaluate(sample.first, depth);
        }
        return value / delayed_evaluation_nsamples_;
    }

    // abstraction of selection strategy
    void clear_leaf_selection_strategy() {
        setup_expansion_loop_ptr_ = 0;
        prepare_next_expansion_iteration_ptr_ = 0;
        exist_nodes_to_expand_ptr_ = 0;
        select_node_for_expansion_ptr_ = 0;
        clear_internal_state_ptr_ = 0;
    }
    void set_leaf_selection_strategy(int strategy) {
        if( strategy == 1 ) {
            random_setup_selection_strategy();
        } else {
            delta_setup_selection_strategy();
        }
    }

    // selection strategy based on delta values
    void delta_setup_selection_strategy() {
        setup_expansion_loop_ptr_ = &aot_t::delta_setup_expansion_loop;
        prepare_next_expansion_iteration_ptr_ = &aot_t::delta_prepare_next_expansion_iteration;
        exist_nodes_to_expand_ptr_ = &aot_t::delta_exist_nodes_to_expand;
        select_node_for_expansion_ptr_ = &aot_t::delta_select_node_for_expansion;
        clear_internal_state_ptr_ = &aot_t::delta_clear_internal_state;
    }
    void delta_setup_expansion_loop(state_node_t<T> *root) const {
        assert(empty_priority_queues());
        insert_into_priority_queue(root);
    }
    void delta_prepare_next_expansion_iteration(state_node_t<T> *root) const {
        clear_priority_queues();
        recompute_delta(root);
    }
    bool delta_exist_nodes_to_expand() const {
        return !empty_priority_queues();
    }
    node_t<T> *delta_select_node_for_expansion(state_node_t<T>*) const {
        return select_from_priority_queue();
    }
    void delta_clear_internal_state() const {
        clear_priority_queues();
    }

    void recompute_delta(state_node_t<T> *root) const {
        assert(!root->is_goal_);
        assert(!root->is_dead_end_);

        std::deque<state_node_t<T>*> s_queue;
        bool expanding_from_s_queue = true;
        std::deque<action_node_t<T>*> a_queue;
        bool expanding_from_a_queue = false;

        root->delta_ = std::numeric_limits<float>::max();
        root->in_best_policy_ = true;
        s_queue.push_back(root);

        while( !s_queue.empty() || !a_queue.empty() ) {
            // expand from the state queue
            if( expanding_from_s_queue ) {
                while( !s_queue.empty() ) {
                    state_node_t<T> *s_node = s_queue.back();
                    s_queue.pop_back();
                    s_node->in_queue_ = false;
                    recompute_delta(s_node, a_queue);
                }
                expanding_from_s_queue = false;
                expanding_from_a_queue = true;
            }

            // expand from the action queue
            if( expanding_from_a_queue ) {
                while( !a_queue.empty() ) {
                    action_node_t<T> *a_node = a_queue.back();
                    a_queue.pop_back();
                    recompute_delta(a_node, s_queue);
                }
                expanding_from_a_queue = false;
                expanding_from_s_queue = true;
            }
        }
    }
    void recompute_delta(state_node_t<T> *s_node,
                         std::deque<action_node_t<T>*> &a_queue) const {
        assert(!s_node->is_goal_);
        assert(!s_node->is_dead_end_);
        if( s_node->is_leaf() ) {
            // insert tip node into priority queue
            if( !s_node->is_dead_end_ && !s_node->is_goal_ && (s_node->depth_ < depth_bound_) ) {
                insert_into_priority_queue(s_node);
            }
        } else {
            assert(!s_node->children_.empty());
            float best_value = s_node->children_[s_node->best_action_]->value_;
            if( s_node->in_best_policy_ ) {
                assert(s_node->delta_ >= 0);

                // compute Delta
                float Delta = std::numeric_limits<float>::max();
                for( int i = 0, isz = s_node->children_.size(); i < isz; ++i ) {
                    if( i != s_node->best_action_ ) {
                        action_node_t<T> *a_node = s_node->children_[i];
                        float d = a_node->value_ - best_value;
                        Delta = Utils::min(Delta, d);
                    }
                }

                // compute delta
                for( int i = 0, isz = s_node->children_.size(); i < isz; ++i ) {
                    action_node_t<T> *a_node = s_node->children_[i];
                    if( i == s_node->best_action_ ) {
                        a_node->delta_ = Utils::min(s_node->delta_, Delta);
                        a_node->in_best_policy_ = true;
                        assert(a_node->delta_ >= 0);
                    } else {
                        a_node->delta_ = best_value - a_node->value_;
                        a_node->in_best_policy_ = false;
                        assert(a_node->delta_ <= 0);
                    }
                    a_queue.push_back(a_node);
                }
            } else {
                assert(s_node->delta_ <= 0);
                for( int i = 0, isz = s_node->children_.size(); i < isz; ++i ) {
                    action_node_t<T> *a_node = s_node->children_[i];
                    a_node->delta_ = s_node->delta_ + best_value - a_node->value_;
                    a_node->in_best_policy_ = false;
                    assert(a_node->delta_ <= 0);
                    a_queue.push_back(a_node);
                }
            }
        }
    }
    void recompute_delta(action_node_t<T> *a_node,
                         std::deque<state_node_t<T>*> &s_queue) const {
        if( a_node->is_leaf() ) {
            // insert tip node into priority queue
            if( a_node->parent_->depth_ < depth_bound_ ) {
                insert_into_priority_queue(a_node);
            }
        } else {
            for( int i = 0, isz = a_node->children_.size(); i < isz; ++i ) {
                state_node_t<T> *s_node = a_node->children_[i].second;
                if( !s_node->in_queue_ && !s_node->is_goal_ && !s_node->is_dead_end_ ) {
                    float delta = std::numeric_limits<float>::max();
                    bool in_best_policy = false;
                    for( int j = 0, jsz = s_node->parents_.size(); j < jsz; ++j ) {
                        int child_index = s_node->parents_[j].first;
                        action_node_t<T> *parent = s_node->parents_[j].second;
                        assert(parent->children_[child_index].second == s_node);
                        float d = parent->delta_ /
                                  (policy_t<T>::problem().discount() *
                                   parent->children_[child_index].first);
                        delta = Utils::min(delta, fabsf(d));
                        in_best_policy = in_best_policy || parent->in_best_policy_;
                    }
                    s_node->delta_ = in_best_policy ? delta : -delta;
                    s_node->in_best_policy_ = in_best_policy;
                    s_queue.push_back(s_node);
                    s_node->in_queue_ = true;
                }
            }
        }
    }

    // implementation of priority queue for storing the deltas
    unsigned size_priority_queues() const {
#ifdef USE_BDD_PQ
        return inside_bdd_priority_queue_.size() + outside_bdd_priority_queue_.size();
#else
        return inside_priority_queue_.size() + outside_priority_queue_.size();
#endif
    }
    bool empty_inside_priority_queue() const {
#ifdef USE_BDD_PQ
        return inside_bdd_priority_queue_.empty();
#else
        return inside_priority_queue_.empty();
#endif
    }
    bool empty_outside_priority_queue() const {
#ifdef USE_BDD_PQ
        return outside_bdd_priority_queue_.empty();
#else
        return outside_priority_queue_.empty();
#endif
    }
    bool empty_priority_queues() const {
        return empty_inside_priority_queue() && empty_outside_priority_queue();
    }
    void clear(priority_queue_t<T> &pq) const {
        while( !pq.empty() ) {
            node_t<T> *node = pq.top();
            pq.pop();
            assert(node->in_pq_);
            node->in_pq_ = false;
        }
    }
    void clear(bdd_priority_queue_t<T> &pq) const {
        while( !pq.empty() ) {
            node_t<T> *node = pq.top();
            pq.pop();
            assert(node->in_pq_);
            node->in_pq_ = false;
        }
    }
    void clear_priority_queues() const {
#ifdef USE_BDD_PQ
        clear(inside_bdd_priority_queue_);
        clear(outside_bdd_priority_queue_);
#else
        clear(inside_priority_queue_);
        clear(outside_priority_queue_);
#endif
    }
    void insert_into_inside_priority_queue(node_t<T> *node) const {
#ifdef USE_BDD_PQ
        std::pair<bool, bool> p = inside_bdd_priority_queue_.push(node);
        node->in_pq_ = p.first;
        if( p.second ) {
            node_t<T> *removed = inside_bdd_priority_queue_.removed_element();
            assert(removed != 0);
            assert(removed->in_pq_);
            removed->in_pq_ = false;
        }
#else
        inside_priority_queue_.push(node);
        node->in_pq_ = true;
#endif
    }
    void insert_into_outside_priority_queue(node_t<T> *node) const {
#ifdef USE_BDD_PQ
        std::pair<bool, bool> p = outside_bdd_priority_queue_.push(node);
        node->in_pq_ = p.first;
        if( p.second ) {
            node_t<T> *removed = outside_bdd_priority_queue_.removed_element();
            assert(removed != 0);
            assert(removed->in_pq_);
            removed->in_pq_ = false;
        }
#else
        outside_priority_queue_.push(node);
        node->in_pq_ = true;
#endif
    }
    void insert_into_priority_queue(node_t<T> *node) const {
        if( !node->in_pq_ ) {
            float sign = copysignf(1, node->delta_);
            if( sign == 1 ) {
#ifdef DEBUG
                std::cout << "push:in  " << node << "=";
                node->print(std::cout, false);
                std::cout << std::endl;
#endif
                insert_into_inside_priority_queue(node);
            } else {
#ifdef DEBUG
                std::cout << "push:out " << node << "=";
                node->print(std::cout, false);
                std::cout << std::endl;
#endif
                insert_into_outside_priority_queue(node);
            }
        }
    }
    node_t<T>* select_from_inside() const {
        node_t<T> *node = 0;
#ifdef USE_BDD_PQ
        node = inside_bdd_priority_queue_.top();
        inside_bdd_priority_queue_.pop();
#else
        node = inside_priority_queue_.top();
        inside_priority_queue_.pop();
#endif
        assert(node->in_pq_);
        node->in_pq_ = false;
        ++from_inside_;
        return node;
    }
    node_t<T>* select_from_outside() const {
       node_t<T> *node = 0;
#ifdef USE_BDD_PQ
        node = outside_bdd_priority_queue_.top();
        outside_bdd_priority_queue_.pop();
#else
        node = outside_priority_queue_.top();
        outside_priority_queue_.pop();
#endif
        assert(node->in_pq_);
        node->in_pq_ = false;
        ++from_outside_;
        return node;
    }
    node_t<T>* select_from_priority_queue() const {
        node_t<T> *node = 0;
        if( empty_inside_priority_queue() ) {
            node = select_from_outside();
        } else if( empty_outside_priority_queue() ) {
            node = select_from_inside();
        } else {
            if( Random::real() < parameter_ )
                node = select_from_inside();
            else
                node = select_from_outside();
        }

#ifdef DEBUG
        std::cout << "pop ";
        node->print(std::cout, false);
        std::cout << std::endl;
#endif

        return node;
    }

    // selection strategy based on random selection
    void random_setup_selection_strategy() {
        setup_expansion_loop_ptr_ = &aot_t::random_setup_expansion_loop;
        prepare_next_expansion_iteration_ptr_ = &aot_t::random_prepare_next_expansion_iteration;
        exist_nodes_to_expand_ptr_ = &aot_t::random_exist_nodes_to_expand;
        select_node_for_expansion_ptr_ = &aot_t::random_select_node_for_expansion;
        clear_internal_state_ptr_ = &aot_t::random_clear_internal_state;
    }
    void random_setup_expansion_loop(state_node_t<T> *root) const {
        random_leaf_ = root;
    }
    void random_prepare_next_expansion_iteration(state_node_t<T> *node) const {
        if( node->is_leaf() ) {
            if( !node->is_goal_ && !node->is_dead_end_ && (node->depth_ < depth_bound_) ) {
                if( (random_leaf_ == 0) || (Random::real() < 0.5) )
                    random_leaf_ = node;
            }
        } else {
            assert(!node->children_.empty());
            for( int i = 0, isz = node->children_.size(); i < isz; ++i ) {
                action_node_t<T> *a_node = node->children_[i];
                assert(!a_node->children_.empty());
                for( int j = 0, jsz = a_node->children_.size(); j < jsz; ++j ) {
                    state_node_t<T> *s_node = a_node->children_[j].second;
                    random_prepare_next_expansion_iteration(s_node);
                }
            }
        }
    }
    bool random_exist_nodes_to_expand() const {
        return random_leaf_ != 0;
    }
    node_t<T> *random_select_node_for_expansion(state_node_t<T> *node) const {
        node_t<T> *leaf = random_leaf_;
        random_leaf_ = 0;
        return leaf;
    }
    void random_clear_internal_state() const {
        random_leaf_ = 0;
    }

};

}; // namespace AOT

template<typename T>
inline const policy_t<T>* make_aot(const policy_t<T> &base_policy,
                                   unsigned width,
                                   unsigned depth_bound,
                                   float parameter,
                                   bool delayed_evaluation = true,
                                   unsigned expansions_per_iteration = 100,
                                   unsigned leaf_nsamples = 1,
                                   unsigned delayed_evaluation_nsamples = 1,
                                   int leaf_selection_strategy = 0) {
    return new AOT::aot_t<T>(base_policy,
                             width,
                             depth_bound,
                             parameter,
                             delayed_evaluation,
                             expansions_per_iteration,
                             leaf_nsamples,
                             delayed_evaluation_nsamples,
                             leaf_selection_strategy);
}

}; // namespace Policy

#undef DEBUG

#endif

