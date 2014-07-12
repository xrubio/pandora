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

#ifndef UCT2_H
#define UCT2_H

#include "policy.h"

#include <iostream>
#include <iomanip>
#include <cassert>
#include <limits>
#include <vector>
#include <math.h>

//#define DEBUG

namespace Policy {

namespace UCT2 {

////////////////////////////////////////////////
//
// Tree
//

template<typename T> struct node_t {
    mutable std::vector<unsigned> counts_;
    mutable std::vector<float> values_;
    node_t(int num_actions)
      : counts_(1+num_actions, 0),
        values_(1+num_actions, 0) {
    } 
    ~node_t() { }
};

////////////////////////////////////////////////
//
// Hash Table
//

template<typename T> struct map_functions_t {
    size_t operator()(const std::pair<unsigned, T> &p) const {
        return p.second.hash();
    }
};

struct data_t {
    std::vector<float> values_;
    std::vector<int> counts_;
    data_t(const std::vector<float> &values, const std::vector<int> &counts)
      : values_(values), counts_(counts) { }
};

template<typename T> class hash_t :
  public Hash::generic_hash_map_t<std::pair<unsigned, T>,
                                  data_t,
                                  map_functions_t<T> > {
  public:
    typedef typename Hash::generic_hash_map_t<std::pair<unsigned, T>,
                                              data_t,
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
};

////////////////////////////////////////////////
//
// Policy
//

template<typename T> class uct2_t : public improvement_t<T> {
  protected:
    unsigned width_;
    unsigned depth_bound_;
    float parameter_;
    mutable hash_t<T> table_;

  public:
    uct2_t(const policy_t<T> &base_policy, unsigned width, unsigned depth_bound, float parameter)
      : improvement_t<T>(base_policy),
        width_(width),
        depth_bound_(depth_bound),
        parameter_(parameter) {
    }
    virtual ~uct2_t() { }

    virtual Problem::action_t operator()(const T &s) const {
        ++policy_t<T>::decisions_;
        table_.clear();
        for( unsigned i = 0; i < width_; ++i ) {
            search_tree(s, 0);
        }
        typename hash_t<T>::iterator it = table_.find(std::make_pair(0, s));
        assert(it != table_.end());
        Problem::action_t action = select_action(s, it->second, 0, false);
        assert(policy_t<T>::problem().applicable(s, action));
        return action;
    }
    virtual const policy_t<T>* clone() const {
        return new uct2_t(improvement_t<T>::base_policy_, width_, depth_bound_, parameter_);
    }
    virtual void print_stats(std::ostream &os) const {
        os << "stats: policy-type=uct(width="
           << width_ << ",depth="
           << depth_bound_ << ",par="
           << parameter_ << ")" << std::endl;
        os << "stats: decisions=" << policy_t<T>::decisions_ << std::endl;
        improvement_t<T>::base_policy_.print_stats(os);
    }

    float value(const T &s, Problem::action_t a) const {
        typename hash_t<T>::const_iterator it = table_.find(std::make_pair(0, s));
        assert(it != table_.end());
        return it->second.values_[1+a];
    }
    unsigned count(const T &s, Problem::action_t a) const {
        typename hash_t<T>::const_iterator it = table_.find(std::make_pair(0, s));
        assert(it != table_.end());
        return it->second.counts_[1+a];
    }
    size_t size() const { return table_.size(); }
    void print_table(std::ostream &os) const {
        table_.print(os);
    }

    float search_tree(const T &s, unsigned depth) const {
#ifdef DEBUG
        std::cout << std::setw(2*depth) << "" << "search_tree(" << s << "):";
#endif

        if( (depth == depth_bound_) || policy_t<T>::problem().terminal(s) ) {
#ifdef DEBUG
            std::cout << " end" << std::endl;
#endif
            return 0;
        }

        if( policy_t<T>::problem().dead_end(s) ) {
#ifdef DEBUG
            std::cout << " dead-end" << std::endl;
#endif
            return policy_t<T>::problem().dead_end_value();
        }

        typename hash_t<T>::iterator it = table_.find(std::make_pair(depth, s));

        if( it == table_.end() ) {
            std::vector<float> values(1 + policy_t<T>::problem().number_actions(s), 0);
            std::vector<int> counts(1 + policy_t<T>::problem().number_actions(s), 0);
            table_.insert(std::make_pair(std::make_pair(depth, s), data_t(values, counts)));
            float value = evaluate(s, depth);
#ifdef DEBUG
            std::cout << " insert in tree w/ value=" << value << std::endl;
#endif
            return value;
        } else {
            // select action for this node and increase counts
            Problem::action_t a = select_action(s, it->second, depth, true);
            ++it->second.counts_[0];
            ++it->second.counts_[1+a];

            // sample next state
            std::pair<const T, bool> p = policy_t<T>::problem().sample(s, a);
            float cost = policy_t<T>::problem().cost(s, a);

#ifdef DEBUG
            std::cout << " count=" << it->second.counts_[0]-1
                      << " fetch " << std::setprecision(5) << it->second.values_[1+a]
                      << " a=" << a
                      << " next=" << p.first
                      << std::endl;
#endif

            // do recursion and update value
            float &old_value = it->second.values_[1+a];
            float n = it->second.counts_[1+a];
            float new_value = cost +
              policy_t<T>::problem().discount() * search_tree(p.first, 1 + depth);
            old_value += (new_value - old_value) / n;
            return old_value;
        }
    }

    Problem::action_t select_action(const T &state,
                                    const data_t &data,
                                    int depth,
                                    bool add_bonus) const {
        float log_ns = logf(data.counts_[0]);
        std::vector<Problem::action_t> best_actions;
        float best_value = std::numeric_limits<float>::max();

        for( Problem::action_t a = 0; a < policy_t<T>::problem().number_actions(state); ++a ) {
            if( policy_t<T>::problem().applicable(state, a) ) {
                // if this action has never been taken in this node, select it
                if( data.counts_[1+a] == 0 ) {
                    return a;
                }

                // compute score of action adding bonus (if applicable)
                assert(data.counts_[0] > 0);
                float par = parameter_ == 0 ? -data.values_[1+a] : parameter_;
                float bonus = add_bonus ? par * sqrtf(2 * log_ns / data.counts_[1+a]) : 0;
                float value = data.values_[1+a] + bonus;

                // update best action so far
                if( value <= best_value ) {
                    if( value < best_value ) {
                        best_value = value;
                        best_actions.clear();
                    }
                    best_actions.push_back(a);
                }
            }
        }
        assert(!best_actions.empty());
        return best_actions[Random::uniform(best_actions.size())];
    }

    float evaluate(const T &s, unsigned depth) const {
        return Evaluation::evaluation(improvement_t<T>::base_policy_,
                                      s, 1, depth_bound_ - depth);
    }
};

}; // namespace UCT2

template<typename T>
inline const policy_t<T>* make_uct2(const policy_t<T> &base_policy,
                                    unsigned width,
                                    unsigned depth_bound,
                                    float parameter) {
    return new UCT2::uct2_t<T>(base_policy, width, depth_bound, parameter);
}

}; // namespace Policy

#undef DEBUG

#endif

