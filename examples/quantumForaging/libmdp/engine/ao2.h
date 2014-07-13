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

#ifndef AO2_H
#define AO2_H

#include "policy.h"

#include <iostream>
#include <cassert>
#include <limits>
#include <vector>
#include <queue>

//#define DEBUG

namespace Policy {


////////////////////////////////////////////////


template<typename T> struct ao2_node_t {
    mutable float value_;
    unsigned depth_;
    unsigned priority_;
    mutable float delta_;
    const ao2_node_t *parent_;
    mutable std::vector<const ao2_node_t*> children_;

    ao2_node_t(float value = 0, unsigned depth = 0, unsigned priority = 0, const ao2_node_t<T> *parent = 0)
      : value_(value), depth_(depth), priority_(priority), delta_(0), parent_(parent) { }

    virtual ~ao2_node_t() { }
    virtual void print(std::ostream &os, bool indent = true) const = 0;

    void print_tree(std::ostream &os) const {
        print(os);
        os << std::endl;
        for( unsigned i = 0, isz = ao2_node_t<T>::children_.size(); i < isz; ++i )
            ao2_node_t<T>::children_[i]->print_tree(os);
    }
};

template<typename T> struct ao2_state_node_t;
template<typename T> struct ao2_action_node_t : public ao2_node_t<T> {
    Problem::action_t action_;
    std::vector<float> probability_;

    ao2_action_node_t(Problem::action_t action, unsigned depth, unsigned priority, const ao2_node_t<T> *parent = 0)
      : ao2_node_t<T>(0, depth, priority, parent), action_(action) { }

    virtual ~ao2_action_node_t() {
        for( unsigned i = 0, isz = ao2_node_t<T>::children_.size(); i < isz; ++i )
            delete ao2_node_t<T>::children_[i];
    }

    virtual void print(std::ostream &os, bool indent = true) const {
        if( indent ) os << std::setw(2 * ao2_node_t<T>::depth_) << "";
        os << "[action=" << action_
           << ",value=" << ao2_node_t<T>::value_
           << ",depth=" << ao2_node_t<T>::depth_
           << ",priority=" << ao2_node_t<T>::priority_
           << ",delta=" << ao2_node_t<T>::delta_
           << "]";
    }
};

template<typename T> struct ao2_state_node_t : public ao2_node_t<T> {
    const T state_;
    mutable int best_action_;

    ao2_state_node_t(const T &state, unsigned depth, unsigned priority, const ao2_node_t<T> *parent = 0)
      : ao2_node_t<T>(0, depth, priority, parent),
        state_(state), best_action_(-1) { }

    virtual ~ao2_state_node_t() {
        for( unsigned i = 0, isz = ao2_node_t<T>::children_.size(); i < isz; ++i )
            delete ao2_node_t<T>::children_[i];
    }

    Problem::action_t best_action() const {
        if( best_action_ == -1 )
            return Problem::noop;
        else
            return static_cast<const ao2_action_node_t<T>*>(ao2_node_t<T>::children_[best_action_])->action_;
    }

    virtual void print(std::ostream &os, bool indent = true) const {
        if( indent ) os << std::setw(2 * ao2_node_t<T>::depth_) << "";
        os << "[state=" << state_
           << ",best_action=" << best_action()
           << ",value=" << ao2_node_t<T>::value_
           << ",depth=" << ao2_node_t<T>::depth_
           << ",priority=" << ao2_node_t<T>::priority_
           << ",delta=" << ao2_node_t<T>::delta_
           << "]";
    }
};

template<typename T> struct ao2_min_priority_t {
    bool operator()(const ao2_state_node_t<T> *n1, const ao2_state_node_t<T> *n2) {
        return fabs(n1->delta_) > fabs(n2->delta_);
    }
};


////////////////////////////////////////////////


template<typename T> class ao2_t : public improvement_t<T> {
  protected:
    unsigned width_;
    unsigned depth_bound_;
    mutable unsigned num_nodes_;
    mutable ao2_state_node_t<T> *root_;
    mutable std::priority_queue<const ao2_state_node_t<T>*, std::vector<const ao2_state_node_t<T>*>, ao2_min_priority_t<T> > priority_queue_;

  public:
    ao2_t(const policy_t<T> &base_policy, unsigned width, unsigned depth_bound)
      : improvement_t<T>(base_policy),
        width_(width),
        depth_bound_(depth_bound),
        num_nodes_(0),
        root_(0) {
    }
    virtual ~ao2_t() { }

    virtual Problem::action_t operator()(const T &s) const {
        //std::cout << std::endl << "new iteration" << std::endl;
        // initialize tree and priority queue
        clear();
        root_ = new ao2_state_node_t<T>(s, 0, 0);
        priority_queue_.push(root_);

        // expand leaves and propagate values
        for( unsigned i = 0; (i < width_) && !priority_queue_.empty(); ++i ) {
            //std::cout << std::endl;
            for( unsigned j = 0; (j < 1) && !priority_queue_.empty(); ++j ) {
                const ao2_state_node_t<T> *node = expand();
                propagate(node);
            }
            clear_priority_queue();
            recompute_delta(root_, std::numeric_limits<float>::max(), true);
            //std::cout << "root->value=" << root_->value_ << std::endl;
        }
        assert((width_ == 0) || ((root_ != 0) && policy_t<T>::problem().applicable(s, root_->best_action())));

        // select best action
        return width_ == 0 ? improvement_t<T>::base_policy_(s) : root_->best_action();
    }

    void clear_priority_queue() const {
        while( !priority_queue_.empty() ) priority_queue_.pop();
    }
    void clear() const {
        num_nodes_ = 0;
        if( root_ != 0 ) {
            delete root_;
            root_ = 0;
        }
        clear_priority_queue();
    }
    unsigned size() const { return num_nodes_; }
    void print_tree(std::ostream &os) const {
        if( root_ == 0 ) {
            os << "(empty)" << std::endl;
        } else {
            //root_->print_tree(os);
            root_->print(os); os << std::endl;
        }
    }

    const ao2_state_node_t<T>* expand() const {
        // get best open node
        const ao2_state_node_t<T> *node = priority_queue_.top();
        priority_queue_.pop();
        //std::cout << "pop " << node << " " << std::flush; node->print(std::cout, false); std::cout << std::endl;
        assert(node->children_.empty());
        node->children_.reserve(policy_t<T>::problem().number_actions(node->state_));

        // expand node
        std::vector<std::pair<T, float> > outcomes;
        for( Problem::action_t a = 0; a < policy_t<T>::problem().number_actions(node->state_); ++a ) {
            if( policy_t<T>::problem().applicable(node->state_, a) ) {
                // create action node for this action
                unsigned priority = 0;//node->priority_ + (a == best_action ? 0 : 1);
                ao2_action_node_t<T> *a_node = new ao2_action_node_t<T>(a, node->depth_, priority, node);
                node->children_.push_back(a_node);
                ++num_nodes_;

                // generate successor states
                policy_t<T>::problem().next(node->state_, a, outcomes);
                a_node->children_.reserve(outcomes.size());
                a_node->probability_.reserve(outcomes.size());
                for( unsigned i = 0, isz = outcomes.size(); i < isz; ++i ) {
                    const T &state = outcomes[i].first;
                    float prob = outcomes[i].second;
                    ao2_state_node_t<T> *s_node = new ao2_state_node_t<T>(state, 1 + node->depth_, priority, a_node);
                    a_node->children_.push_back(s_node);
                    a_node->probability_.push_back(prob);
                    ++num_nodes_;

                    // set default value of child using base policy
                    s_node->value_ = evaluate(state, 1 + node->depth_);
                    a_node->value_ += prob * s_node->value_;

#if 0
                    // insert child into priority queue
                    if( s_node->depth_ <= depth_bound_ ) {
                        priority_queue_.push(s_node);
                        //std::cout << "push " << s_node << " "; s_node->print(std::cout, false); std::cout << std::endl;
                    }
#endif
                }

                // set value for new action node
                a_node->value_ = policy_t<T>::problem().cost(node->state_, a) + policy_t<T>::problem().discount() * a_node->value_;
                //std::cout << "child: a=" << a << ", value=" << a_node->value_ << std::endl;
            }
        }
        return node;
    }

    void propagate(const ao2_state_node_t<T> *s_node) const {
        while( s_node != 0 ) {
            float value = std::numeric_limits<float>::max();
            for( unsigned i = 0, isz = s_node->children_.size(); i < isz; ++i ) {
                float child_value = s_node->children_[i]->value_;
                if( child_value < value ) {
                    value = child_value;
                    s_node->best_action_ = i; //static_cast<const ao2_action_node_t<T>*>(s_node->children_[i])->action_;
                }
            }
            s_node->value_ = value;
            //std::cout << "propagate: s_node->value=" << s_node->value_ << std::endl;
            if( s_node->parent_ != 0 ) {
                const ao2_action_node_t<T> *a_node = static_cast<const ao2_action_node_t<T>*>(s_node->parent_);
                float value = 0;
                for( unsigned i = 0, isz = a_node->children_.size(); i < isz; ++i ) {
                    value += a_node->probability_[i] * a_node->children_[i]->value_;
                }
                assert(a_node->parent_ != 0);
                s_node = static_cast<const ao2_state_node_t<T>*>(a_node->parent_);
                a_node->value_ = policy_t<T>::problem().cost(s_node->state_, a_node->action_) + policy_t<T>::problem().discount() * value;
            } else {
                s_node = 0;
            }
        }
    }

    void recompute_delta(const ao2_state_node_t<T> *s_node, float delta, bool in_best_policy) const {
        float Delta = std::numeric_limits<float>::max();
        for( unsigned i = 0, isz = s_node->children_.size(); i < isz; ++i ) {
            const ao2_action_node_t<T> *a_node = static_cast<const ao2_action_node_t<T>*>(s_node->children_[i]);
            float dif = a_node->value_ - s_node->value_;
            if( dif > 0 ) Delta = Utils::min(Delta, dif);
        }
        //Delta = Delta == std::numeric_limits<float>::max() ? 0 : Delta;
        //std::cout << std::setw(2*s_node->depth_) << "" << "s_node->value=" << s_node->value_ << ", #children=" << s_node->children_.size() << ", Delta=" << Delta << std::endl;

        for( unsigned i = 0, isz = s_node->children_.size(); i < isz; ++i ) {
            const ao2_action_node_t<T> *a_node = static_cast<const ao2_action_node_t<T>*>(s_node->children_[i]);
            float dif = a_node->value_ - s_node->value_;
            if( dif == 0 ) {
                recompute_delta(a_node, delta, Delta, true, in_best_policy);
            } else {
                recompute_delta(a_node, delta, dif, false, in_best_policy);
            }
        }

        // insert tip node into priority queue
        if( (s_node->depth_ < depth_bound_) && s_node->children_.empty() ) {
            priority_queue_.push(s_node);
            //std::cout << std::setw(2*s_node->depth_) << "" << "push " << s_node << " "; s_node->print(std::cout, false); std::cout << std::endl;
        }
    }
    void recompute_delta(const ao2_action_node_t<T> *a_node, float delta, float Delta,
                         bool best_action, bool in_best_policy) const {
        //std::cout << "#children=" << a_node->children_.size() << std::endl;
        for( unsigned i = 0, isz = a_node->children_.size(); i < isz; ++i ) {
            float prob = a_node->probability_[i];
            const ao2_state_node_t<T> *s_node = static_cast<const ao2_state_node_t<T>*>(a_node->children_[i]);
            float ndelta = 0;
            if( in_best_policy && best_action ) {
                assert(delta >= 0);
                // delta_{s´} = min { delta_s, Delta } / gamma * P(s'|sa)
                // Delta = min_{ a != a* } T(sa) - T(sa*)
                ndelta = Utils::min(delta, Delta);
                //std::cout << std::setw(2*s_node->depth_) << "" << "case1: ndelta=" << ndelta << std::endl;
                assert(ndelta >= 0);
            } else if( in_best_policy ) {
                assert(delta >= 0);
                // delta_{s´} = Delta / gamma * P(s'|sa) for a != a*
                // Delta = T(sa) - T(sa*)
                ndelta = -Delta;
                in_best_policy = false;
                //std::cout << std::setw(2*s_node->depth_) << "" << "case2: ndelta=" << ndelta << std::endl;
                assert(ndelta <= 0);
            } else {
                // delta_{s´} = (delta_s + Delta) / gamma * P(s'|sa) for all a
                // Delta = T(sa) - T(sa*)
                ndelta = -Delta;
                //std::cout << std::setw(2*s_node->depth_) << "" << "case3: ndelta=" << ndelta << std::endl;
                assert(ndelta <= 0);
            }
            ndelta = ndelta / (policy_t<T>::problem().discount() * prob);
            s_node->delta_ = ndelta;
            recompute_delta(s_node, ndelta, in_best_policy);
        }
    }

    float evaluate(const T &s, unsigned depth) const {
        return Evaluation::evaluation(improvement_t<T>::base_policy_, s, 1, depth_bound_ - depth);
    }
};

}; // namespace Policy

#undef DEBUG

#endif

