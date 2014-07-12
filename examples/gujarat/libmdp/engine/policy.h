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

#ifndef POLICY_H
#define POLICY_H

#include "problem.h"
#include "heuristic.h"
#include "random.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>
#include <limits>
#include <vector>
#include <math.h>

//#define DEBUG

namespace Online {

namespace Policy {

// Abstract class that defines the interface for action-selection polcicies
template<typename T> class policy_t {
  protected:
    std::string name_;
    const Problem::problem_t<T> &problem_;
    mutable unsigned decisions_;

  public:
    policy_t(const std::string &name, const Problem::problem_t<T> &problem)
      : name_(name), problem_(problem), decisions_(0) {
    }
    policy_t(const Problem::problem_t<T> &problem)
      : name_(""), problem_(problem), decisions_(0) {
    }
    virtual ~policy_t() { }
    void set_name(const std::string &name) { name_ = name; }
    const std::string& name() const { return name_; }
    const Problem::problem_t<T>& problem() const { return problem_; }
    unsigned decisions() const { return decisions_; }
    virtual Problem::action_t operator()(const T &s) const = 0;
    virtual const policy_t<T>* clone() const = 0;
    virtual void print_stats(std::ostream &os) const = 0;
};

// Abstract class for improvement of a base policy
template<typename T> class improvement_t : public policy_t<T> {
  protected:
    const policy_t<T> &base_policy_;

  public:
    improvement_t(const std::string &name, const policy_t<T> &base_policy)
      : policy_t<T>(name, base_policy.problem()),
        base_policy_(base_policy) {
    }
    improvement_t(const policy_t<T> &base_policy)
      : policy_t<T>(base_policy.problem()),
        base_policy_(base_policy) {
    }
    virtual ~improvement_t() { }
};

// Random base policy: select a random applicable action
template<typename T> class random_t : public policy_t<T> {
  public:
    random_t(const Problem::problem_t<T> &problem) : policy_t<T>("random()", problem) { }
    virtual ~random_t() { }
    virtual const policy_t<T>* clone() const {
        return new random_t(policy_t<T>::problem());
    }

    virtual Problem::action_t operator()(const T &s) const {
        ++policy_t<T>::decisions_;
        if( policy_t<T>::problem().dead_end(s) ) return Problem::noop;
        std::vector<Problem::action_t> actions;
        actions.reserve(policy_t<T>::problem().number_actions(s));
        for( Problem::action_t a = 0; a < policy_t<T>::problem().number_actions(s); ++a ) {
            if( policy_t<T>::problem().applicable(s, a) ) {
                actions.push_back(a);
            }
        }
        return actions.empty() ? Problem::noop : actions[Random::uniform(actions.size())];
    }
    virtual void print_stats(std::ostream &os) const {
        os << "stats: policy=" << policy_t<T>::name() << std::endl;
        os << "stats: decisions=" << policy_t<T>::decisions_ << std::endl;
    }
};

// Hash-based based policy: select best action using bestQValue method of hash table
template<typename T> class hash_policy_t : public policy_t<T> {
  protected:
    const Problem::hash_t<T> &hash_;

  public:
    hash_policy_t(const Problem::hash_t<T> &hash)
      : policy_t<T>("hash()", hash.problem()), hash_(hash) {
    }
    virtual ~hash_policy_t() { }
    virtual const policy_t<T>* clone() const { return new hash_policy_t(hash_); }

    virtual Problem::action_t operator()(const T &s) const {
        ++policy_t<T>::decisions_;
        std::pair<Problem::action_t, float> p = hash_.bestQValue(s);
        assert(policy_t<T>::problem().applicable(s, p.first));
        return p.first;
    }
    virtual void print_stats(std::ostream &os) const {
        os << "stats: policy=" << policy_t<T>::name() << std::endl;
        os << "stats: decisions=" << policy_t<T>::decisions_ << std::endl;
    }
};

// Base class for greedy policies wrt 1-step-lookahead of heuristic
template<typename T> class base_greedy_t : public policy_t<T> {
  protected:
    const Heuristic::heuristic_t<T> &heuristic_;
    bool optimistic_;
    bool random_ties_;

  public:
    base_greedy_t(const std::string &name,
                  const Problem::problem_t<T> &problem,
                  const Heuristic::heuristic_t<T> &heuristic,
                  bool optimistic,
                  bool random_ties)
      : policy_t<T>(name, problem), heuristic_(heuristic),
        optimistic_(optimistic), random_ties_(random_ties) {
    }
    base_greedy_t(const Problem::problem_t<T> &problem,
                  const Heuristic::heuristic_t<T> &heuristic,
                  bool optimistic,
                  bool random_ties)
      : policy_t<T>(problem), heuristic_(heuristic),
        optimistic_(optimistic), random_ties_(random_ties) {
        std::stringstream name_stream;
        name_stream << "greedy("
                    << "optimistic=" << (optimistic_ ? "true" : "false")
                    << ",random_ties=" << (random_ties_ ? "true" : "false")
                    << ")";
        policy_t<T>::set_name(name_stream.str());
    }
    virtual ~base_greedy_t() { }
    virtual const policy_t<T>* clone() const {
        return new base_greedy_t(policy_t<T>::name(), policy_t<T>::problem(), heuristic_, optimistic_, random_ties_);
    }

    virtual Problem::action_t operator()(const T &s) const {
        ++policy_t<T>::decisions_;
        std::vector<std::pair<T, float> > outcomes;
        std::vector<Problem::action_t> best_actions;
        int nactions = policy_t<T>::problem().number_actions(s);
        float best_value = std::numeric_limits<float>::max();
        best_actions.reserve(random_ties_ ? nactions : 1);
        for( Problem::action_t a = 0; a < nactions; ++a ) {
            if( policy_t<T>::problem().applicable(s, a) ) {
                float value = optimistic_ ? std::numeric_limits<float>::max() : 0;
                policy_t<T>::problem().next(s, a, outcomes);
                for( size_t i = 0, isz = outcomes.size(); i < isz; ++i ) {
                    float hval = heuristic_.value(outcomes[i].first);
                    if( optimistic_ ) {
                        value = hval < value ? hval : value;
                    } else {
                        value += outcomes[i].second * hval;
                    }
                }
                value += policy_t<T>::problem().cost(s, a);

                if( value <= best_value ) {
                    if( value < best_value ) {
                        best_value = value;
                        best_actions.clear();
                    }
                    if( random_ties_ || best_actions.empty() )
                        best_actions.push_back(a);
                }
            }
        }
        return best_actions[Random::uniform(best_actions.size())];
    }
    virtual void print_stats(std::ostream &os) const {
        os << "stats: policy=" << policy_t<T>::name() << std::endl;
        os << "stats: decisions=" << policy_t<T>::decisions_ << std::endl;
    }
};

// Greedy policy with fixed tie breaking
template<typename T> class greedy_t : public base_greedy_t<T> {
  public:
    greedy_t(const Problem::problem_t<T> &problem, const Heuristic::heuristic_t<T> &heuristic)
      : base_greedy_t<T>(problem, heuristic, false, false) { }
    virtual ~greedy_t() { }
};

// Greedy policy with random tie breaking
template<typename T> class random_greedy_t : public base_greedy_t<T> {
  public:
    random_greedy_t(const Problem::problem_t<T> &problem, const Heuristic::heuristic_t<T> &heuristic)
      : base_greedy_t<T>(problem, heuristic, false, true) { }
    virtual ~random_greedy_t() { }
};

// Optimistic greedy policy with fixed tie breaking
template<typename T> class optimistic_greedy_t : public policy_t<T> {
  public:
    optimistic_greedy_t(const Problem::problem_t<T> &problem, const Heuristic::heuristic_t<T> &heuristic)
      : base_greedy_t<T>(problem, heuristic, true, false) { }
    virtual ~optimistic_greedy_t() { }
};

// Optimistic greedy policy with random tie breaking
template<typename T> class random_optimistic_greedy_t : public policy_t<T> {
  public:
    random_optimistic_greedy_t(const Problem::problem_t<T> &problem, const Heuristic::heuristic_t<T> &heuristic)
      : base_greedy_t<T>(problem, heuristic, true, true) { }
    virtual ~random_optimistic_greedy_t() { }
};

}; // namespace Policy


// Online evaluation
namespace Evaluation {

template<typename T>
inline float evaluation_trial(const Policy::policy_t<T> &policy, const T &s, unsigned max_depth) {
    T state = s;
    size_t steps = 0;
    float cost = 0;
    float discount = 1;
    if( policy.problem().dead_end(state) ) return policy.problem().dead_end_value();
    while( (steps < max_depth) && !policy.problem().terminal(state) ) {
        //std::cout << "evaluation_trial: " << state << std::flush;
        Problem::action_t action = policy(state);
        //std::cout << ", a=" << action << std::endl;
        if( action == Problem::noop ) {
            //std::cout << "no applicable action" << std::endl;
            return cost + policy.problem().dead_end_value();
        }
        assert(policy.problem().applicable(state, action));
        std::pair<T, bool> p = policy.problem().sample(state, action);
        cost += discount * policy.problem().cost(state, action);
        discount *= policy.problem().discount();
        state = p.first;
        ++steps;
        if( policy.problem().dead_end(state) ) {
            return cost + policy.problem().dead_end_value();
        }
    }
    return cost;
}

template<typename T>
inline float evaluation(const Policy::policy_t<T> &policy, const T &s, unsigned number_trials, unsigned max_depth, bool verbose = false) {
    float value = 0;
    if( verbose ) std::cout << "#trials=" << number_trials << ":";
    for( unsigned i = 0; i < number_trials; ++i ) {
        if( verbose ) std::cout << " " << i << std::flush;
        value += evaluation_trial(policy, s, max_depth);
    }
    if( verbose ) std::cout << std::endl;
    return value / number_trials;
}

template<typename T>
inline std::pair<float, float>
  evaluation_with_stdev(const Policy::policy_t<T> &policy,
                        const T &s,
                        unsigned number_trials,
                        unsigned max_depth,
                        bool verbose = false) {
    float sum = 0;
    std::vector<float> values;
    values.reserve(number_trials);
    if( verbose ) std::cout << "#trials=" << number_trials << ":";
    for( unsigned trial = 0; trial < number_trials; ++trial ) {
        if( verbose ) std::cout << " " << trial << std::flush;
        values.push_back(evaluation_trial(policy, s, max_depth));
        sum += values.back();
        if( verbose ) {
            std::cout << "(" << std::setprecision(1) << sum/(1+trial) << ")"
                      << std::flush;
        }
    }
    if( verbose ) std::cout << std::endl;

    // compute average
    float avg = 0;
    for( unsigned i = 0; i < number_trials; ++i ) {
        avg += values[i];
    }
    avg /= number_trials;

    // compute stdev
    float stdev = 0;
    for( unsigned i = 0; i < number_trials; ++i ) {
        stdev += (avg - values[i]) * (avg - values[i]);
    }
    stdev = sqrt(stdev) / (number_trials - 1);
    return std::make_pair(avg, stdev);
}

}; // namespace Evaluation

}; // namespace Online

#undef DEBUG

#endif

