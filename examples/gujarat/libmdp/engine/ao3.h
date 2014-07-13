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

#ifndef AO3_H
#define AO3_H

#include "policy.h"

#include <iostream>
#include <cassert>
#include <limits>
#include <vector>
#include <queue>

//#define DEBUG

namespace Policy {

////////////////////////////////////////////////

template<typename T> struct ao3_node_t {
    float value_;
    float delta_;
    bool in_best_policy_;

    ao3_node_t(float value = 0, float delta = 0)
      : value_(value), delta_(delta), in_best_policy_(false) { }
    virtual ~ao3_node_t() { }
};

template<typename T> struct ao3_state_node_t;
template<typename T> struct ao3_action_node_t : public ao3_node_t<T> {
    Problem::action_t action_;
    float action_cost_;
    ao3_state_node_t<T> *parent_;
    std::vector<std::pair<float, ao3_state_node_t<T>*> > children_;

    ao3_action_node_t(Problem::action_t action) : action_(action) { }
    virtual ~ao3_action_node_t() { }

    void update_value(float discount) {
        ao3_node_t<T>::value_ = 0;
        for( unsigned i = 0, isz = children_.size(); i < isz; ++i ) {
            ao3_node_t<T>::value_ += children_[i].first * children_[i].second->value_;
        }
        ao3_node_t<T>::value_ = action_cost_ + discount * ao3_node_t<T>::value_;
    }

    void print(std::ostream &os, bool indent = true) const {
    }
};

template<typename T> struct ao3_state_node_t : public ao3_node_t<T> {
    const T state_;
    int depth_;
    int best_action_;
    bool in_pq_;
    bool in_queue_;
    std::vector<std::pair<int, ao3_action_node_t<T>*> > parents_;
    std::vector<ao3_action_node_t<T>*> children_;

    ao3_state_node_t(const T &state, int depth = 0)
      : state_(state), depth_(depth), best_action_(-1), in_pq_(false), in_queue_(false) { }
    virtual ~ao3_state_node_t() { }

    Problem::action_t best_action() const {
        return best_action_ == -1 ? Problem::noop : children_[best_action_]->action_;
    }

    void update_value() {
        ao3_node_t<T>::value_ = std::numeric_limits<float>::max();
        for( unsigned i = 0, isz = children_.size(); i < isz; ++i ) {
            float child_value = children_[i]->value_;
            if( child_value < ao3_node_t<T>::value_ ) {
                ao3_node_t<T>::value_ = child_value;
                best_action_ = i;
            }
        }
    }

    void print(std::ostream &os, bool indent = true) const {
        if( indent ) os << std::setw(2 * depth_) << "";
        os << "[state=" << state_
           << ",depth=" << depth_
           << ",best_action=" << best_action()
           << ",#pa=" << parents_.size()
           << ",value=" << ao3_node_t<T>::value_
           << ",delta=" << ao3_node_t<T>::delta_
           << "]";
    }
};

////////////////////////////////////////////////

template<typename T> struct ao3_map_functions_t {
    bool operator()(const std::pair<const T*, unsigned> &p1, const std::pair<const T*, unsigned> &p2) const {
        return (p1.second == p2.second) && (*p1.first == *p2.first);
    }
    size_t operator()(const std::pair<const T*, unsigned> &p) const {
        return p.first->hash();
    }
};

template<typename T> class ao3_table_t : public std::tr1::unordered_map<std::pair<const T*, unsigned>, ao3_state_node_t<T>*, ao3_map_functions_t<T>, ao3_map_functions_t<T> > {
  public:
    typedef typename std::tr1::unordered_map<std::pair<const T*, unsigned>, ao3_state_node_t<T>*, ao3_map_functions_t<T>, ao3_map_functions_t<T> > base_type;
    typedef typename base_type::const_iterator const_iterator;
    const_iterator begin() const { return base_type::begin(); }
    const_iterator end() const { return base_type::end(); }

  public:
    ao3_table_t() { }
    virtual ~ao3_table_t() { }
    void print(std::ostream &os) const {
        for( const_iterator it = begin(); it != end(); ++it ) {
            os << "(" << it->first.first << "," << it->first.second << ")" << std::endl;
        }
    }
    void clear() {
        for( const_iterator it = begin(); it != end(); ++it ) {
            ao3_state_node_t<T> *s_node = it->second;
            for( int i = 0, isz = s_node->children_.size(); i < isz; ++i ) {
                delete s_node->children_[i];
            }
            delete it->second;
        }
        base_type::clear();
    }
};

////////////////////////////////////////////////

template<typename T> struct ao3_min_priority_t {
    bool operator()(const ao3_state_node_t<T> *n1, const ao3_state_node_t<T> *n2) {
        return fabs(n1->delta_) > fabs(n2->delta_);
    }
};

template<typename T> class ao3_priority_queue_t : public std::priority_queue<ao3_state_node_t<T>*, std::vector<ao3_state_node_t<T>*>, ao3_min_priority_t<T> > {
};

////////////////////////////////////////////////

template<typename T> class ao3_t : public improvement_t<T> {
  protected:
    unsigned width_;
    unsigned depth_bound_;
    float ao_parameter_;
    unsigned num_exp_per_iteration_;
    mutable unsigned num_nodes_;
    mutable ao3_state_node_t<T> *root_;
    mutable ao3_priority_queue_t<T> inside_priority_queue_;
    mutable ao3_priority_queue_t<T> outside_priority_queue_;
    mutable ao3_table_t<T> table_;
    mutable float from_inside_;
    mutable float from_outside_;

  public:
    ao3_t(const policy_t<T> &base_policy, unsigned width, unsigned depth_bound, float ao_parameter)
      : improvement_t<T>(base_policy),
        width_(width),
        depth_bound_(depth_bound),
        ao_parameter_(ao_parameter),
        num_exp_per_iteration_(2),
        num_nodes_(0),
        root_(0),
        from_inside_(0),
        from_outside_(0) {
    }
    virtual ~ao3_t() { }

    virtual Problem::action_t operator()(const T &s) const {
        //std::cout << std::endl << "new iteration" << std::endl;
        // initialize tree and priority queue
        clear();
        root_ = fetch_node(s, 0);
        insert_into_priority_queue(root_);

        // expand leaves and propagate values
        for( unsigned i = 0; (i < width_) && !empty_priority_queues(); ++i ) {
            //std::cout << std::endl;
            for( unsigned j = 0; (j < num_exp_per_iteration_) && !empty_priority_queues(); ++j ) {
                ao3_state_node_t<T> *node = expand();
                propagate(node);
                break;
            }
            clear_priority_queues();
            recompute_delta(root_);
            //std::cout << "root->value=" << root_->value_ << std::endl;
        }
        assert((width_ == 0) ||
               ((root_ != 0) && policy_t<T>::problem().applicable(s, root_->best_action())));

        // select best action
        return width_ == 0 ? improvement_t<T>::base_policy_(s) : root_->best_action();
    }

    bool empty_priority_queues() const {
        return inside_priority_queue_.empty() && outside_priority_queue_.empty();
    }
    void clear(ao3_priority_queue_t<T> &pq) const {
        while( !pq.empty() ) {
            ao3_state_node_t<T> *node = pq.top();
            pq.pop();
            node->in_pq_ = false;
        }
    }
    void clear_priority_queues() const {
        clear(inside_priority_queue_);
        clear(outside_priority_queue_);
    }
    void insert_into_priority_queue(ao3_state_node_t<T> *node) const {
        if( !node->in_pq_ ) {
            if( node->delta_ >= 0 ) {
                inside_priority_queue_.push(node);
            } else {
                outside_priority_queue_.push(node);
            }
            node->in_pq_ = true;
            //std::cout << std::setw(2*node->depth_) << "" << "push "; node->print(std::cout, false); std::cout << std::endl;
        }
    }
    ao3_state_node_t<T>* select_from_inside() const {
        ao3_state_node_t<T> *node = inside_priority_queue_.top();
        inside_priority_queue_.pop();
        node->in_pq_ = false;
        ++from_inside_;
        return node;
    }
    ao3_state_node_t<T>* select_from_outside() const {
        ao3_state_node_t<T> *node = outside_priority_queue_.top();
        outside_priority_queue_.pop();
        node->in_pq_ = false;
        ++from_outside_;
        return node;
    }
    ao3_state_node_t<T>* select_from_priority_queue() const {
        ao3_state_node_t<T> *node = 0;
        if( inside_priority_queue_.empty() ) {
            node = select_from_outside();
        } else if( outside_priority_queue_.empty() ) {
            node = select_from_inside();
        } else {
            if( Random::real() < ao_parameter_ ) {
                node = select_from_inside();
            } else {
                node = select_from_outside();
            }
        }
        //std::cout << "pop "; node->print(std::cout, false); std::cout << std::endl;
        return node;
    }

    void clear_table() const {
        table_.clear();
    }
    void clear() const {
        clear_priority_queues();
        clear_table();
        num_nodes_ = 0;
        root_ = 0;
    }

    unsigned size() const { return num_nodes_; }
    void print_tree(std::ostream &os) const {
        if( root_ == 0 ) {
            os << "(empty)" << std::endl;
        } else {
            //root_->print_tree(os);
            //root_->print(os); os << std::endl;
        }
    }

    ao3_state_node_t<T>* fetch_node(const T &state, unsigned depth) const {
        typename ao3_table_t<T>::iterator it = table_.find(std::make_pair(&state, depth));
        if( it == table_.end() ) {
            ++num_nodes_;
            ao3_state_node_t<T> *node = new ao3_state_node_t<T>(state, depth);
            table_.insert(std::make_pair(std::make_pair(&node->state_, depth), node));
            node->value_ = evaluate(state, depth);
            //std::cout << "NEW "; node->print(std::cout, false); std::cout << std::endl;
            return node;
        } else {
            //std::cout << "FOUND "; it->second->print(std::cout, false); std::cout << std::endl;
            return it->second;
        }
    }

    void stats(std::ostream &os) const {
        if( from_inside_ + from_outside_ > 0 ) {
            os << "%in=" << from_inside_ / (from_inside_ + from_outside_)
               << ", %out=" << from_outside_ / (from_inside_ + from_outside_)
               << std::endl;
        }
    }

    ao3_state_node_t<T>* expand() const {
        // get open node
        ao3_state_node_t<T> *node = select_from_priority_queue();
        assert(node->children_.empty());
        node->children_.reserve(policy_t<T>::problem().number_actions(node->state_));

        // expand node
        std::vector<std::pair<T, float> > outcomes;
        for( Problem::action_t a = 0; a < policy_t<T>::problem().number_actions(node->state_); ++a ) {
            if( policy_t<T>::problem().applicable(node->state_, a) ) {
                // create action node for this action
                ++num_nodes_;
                ao3_action_node_t<T> *a_node = new ao3_action_node_t<T>(a);
                a_node->action_cost_ = policy_t<T>::problem().cost(node->state_, a);
                a_node->parent_ = node;
                node->children_.push_back(a_node);

                // generate successor states
                policy_t<T>::problem().next(node->state_, a, outcomes);
                a_node->children_.reserve(outcomes.size());
                assert(a_node->children_.empty());
                for( int i = 0, isz = outcomes.size(); i < isz; ++i ) {
                    const T &state = outcomes[i].first;
                    float prob = outcomes[i].second;
                    ao3_state_node_t<T> *s_node = fetch_node(state, 1 + node->depth_);
                    s_node->parents_.push_back(std::make_pair(i, a_node));
                    a_node->children_.push_back(std::make_pair(prob, s_node));
                    a_node->value_ += prob * s_node->value_;
                }
                a_node->value_ = a_node->action_cost_ + policy_t<T>::problem().discount() * a_node->value_;
            }
        }
        return node;
    }

    // propagate new value up the DAG using BFS and stopping
    // when the value changes no further
    void propagate(ao3_state_node_t<T> *s_node) const {
        std::deque<ao3_state_node_t<T>*> queue;
        queue.push_back(s_node);
        s_node->in_queue_ = true;
        while( !queue.empty() ) {
            ao3_state_node_t<T> *s_node = queue.front();
            queue.pop_front();
            s_node->in_queue_ = false;
            float old_value = s_node->value_;
            s_node->update_value();
            if( old_value != s_node->value_ ) {
                for( int i = 0, isz = s_node->parents_.size(); i < isz; ++i ) {
                    ao3_action_node_t<T> *a_node = s_node->parents_[i].second;
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

    // recompute delta values for nodes in top-down BFS manner
    void recompute_delta(ao3_state_node_t<T> *root) const {
        std::deque<ao3_state_node_t<T>*> s_queue;
        bool expanding_from_s_queue = true;
        std::deque<ao3_action_node_t<T>*> a_queue;
        bool expanding_from_a_queue = false;

        root->delta_ = std::numeric_limits<float>::max();
        root->in_best_policy_ = true;
        s_queue.push_back(root);

        while( !s_queue.empty() || !a_queue.empty() ) {
            // expand from the state queue
            if( expanding_from_s_queue ) {
                while( !s_queue.empty() ) {
                    ao3_state_node_t<T> *s_node = s_queue.back();
                    s_queue.pop_back();
                    s_node->in_queue_ = false;
                    recompute(s_node, a_queue);
                }
                expanding_from_s_queue = false;
                expanding_from_a_queue = true;
            }

            // expand from the action queue
            if( expanding_from_a_queue ) {
                while( !a_queue.empty() ) {
                    ao3_action_node_t<T> *a_node = a_queue.back();
                    a_queue.pop_back();
                    recompute(a_node, s_queue);
                }
                expanding_from_a_queue = false;
                expanding_from_s_queue = true;
            }
        }
    }
    void recompute(ao3_state_node_t<T> *s_node, std::deque<ao3_action_node_t<T>*> &a_queue) const {
        if( s_node->children_.empty() ) {
            // insert tip node into priority queue
            if( s_node->depth_ < (int)depth_bound_ ) {
                insert_into_priority_queue(s_node);
            }
        } else {
            float best_value = s_node->children_[s_node->best_action_]->value_;
            if( s_node->in_best_policy_ ) {
                assert(s_node->delta_ >= 0);

                // compute Delta
                float Delta = std::numeric_limits<float>::max();
                for( int i = 0, isz = s_node->children_.size(); i < isz; ++i ) {
                    if( i != s_node->best_action_ ) {
                        ao3_action_node_t<T> *a_node = s_node->children_[i];
                        float d = a_node->value_ - best_value;
                        Delta = Utils::min(Delta, d);
                    }
                }

                // compute delta
                for( int i = 0, isz = s_node->children_.size(); i < isz; ++i ) {
                    ao3_action_node_t<T> *a_node = s_node->children_[i];
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
                    ao3_action_node_t<T> *a_node = s_node->children_[i];
                    a_node->delta_ = s_node->delta_ + best_value - a_node->value_;
                    a_node->in_best_policy_ = false;
                    assert(a_node->delta_ <= 0);
                    a_queue.push_back(a_node);
                }
            }
        }
    }
    void recompute(ao3_action_node_t<T> *a_node, std::deque<ao3_state_node_t<T>*> &s_queue) const {
        for( int i = 0, isz = a_node->children_.size(); i < isz; ++i ) {
            ao3_state_node_t<T> *s_node = a_node->children_[i].second;
            if( s_node->in_queue_ == false ) {
                float delta = std::numeric_limits<float>::max();
                bool in_best_policy = false;
                for( int j = 0, jsz = s_node->parents_.size(); j < jsz; ++j ) {
                    int child_index = s_node->parents_[j].first;
                    ao3_action_node_t<T> *parent = s_node->parents_[j].second;
                    assert(parent->children_[child_index].second == s_node);
                    float d = parent->delta_ /
                              (policy_t<T>::problem().discount() * parent->children_[child_index].first);
                    delta = Utils::min(delta, fabsf(d));
                    in_best_policy = in_best_policy || parent->in_best_policy_;
                }
                //delta /= s_node->parents_.size();
                s_node->delta_ = in_best_policy ? delta : -delta;
                s_node->in_best_policy_ = in_best_policy;
                s_queue.push_back(s_node);
                s_node->in_queue_ = true;
            }
        }
    }

    float evaluate(const T &s, unsigned depth) const {
        return Evaluation::evaluation(improvement_t<T>::base_policy_, s, 1, depth_bound_ - depth);
    }
};

}; // namespace Policy

#undef DEBUG

#endif

