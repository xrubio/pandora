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

#ifndef PROBLEM_H
#define PROBLEM_H

#include "hash.h"
#include "random.h"
#include "utils.h"

#include <iostream>
#include <cassert>
#include <limits>
#include <vector>
#include <float.h>

//#define DEBUG

namespace Problem {

#ifndef __ACTION_TYPE
#define __ACTION_TYPE
    typedef int action_t;
    const action_t noop = -1;
#endif

template<typename T> class problem_t;

// The hash class implements a hash table that stores information related
// to the states of the problem which is used by different algorithms.

template<typename T> class hash_t : public Hash::hash_map_t<T> {

  public:
    typedef Hash::hash_map_t<T> base_type;

  protected:
    const problem_t<T> &problem_;
    unsigned updates_;

  public:
    hash_t(const problem_t<T> &problem, typename base_type::eval_function_t *heuristic = 0)
      : Hash::hash_map_t<T>(heuristic),
        problem_(problem), updates_(0) {
    }
    virtual ~hash_t() { }

    const problem_t<T>& problem() const { return problem_; }
    unsigned updates() const { return updates_; }
    void inc_updates() { ++updates_; }
    void update(const T &s, float value) {
        Hash::hash_map_t<T>::update(s, value);
        inc_updates();
    }

    virtual float QValue(const T &s, action_t a) const;
    std::pair<action_t, float> bestQValue(const T &s) const {
        action_t best_action = noop;
        float best_value = std::numeric_limits<float>::max();
        for( action_t a = 0; a < problem_.number_actions(s); ++a ) {
            if( problem_.applicable(s, a) ) {
                float value = QValue(s, a);
                if( value < best_value ) {
                    best_value = value;
                    best_action = a;
                }
            }
        }
        return std::make_pair(best_action, best_value);
    }
};

template<typename T> class min_hash_t : public hash_t<T> {
    typedef Hash::hash_map_t<T> hash_base_type;

  public:
    min_hash_t(const problem_t<T> &problem,
               typename hash_base_type::eval_function_t *heuristic = 0)
      : hash_t<T>(problem, heuristic) {
    }
    virtual ~min_hash_t() { }
    virtual float QValue(const T &s, action_t a) const;
};


// A instance of problem_t represents an MDP problem. It contains all the 
// necessary information to run the different algorithms.

template<typename T> class problem_t {

  protected:
    float discount_;
    float dead_end_value_;
    mutable size_t expansions_;

  public:
    problem_t(float discount = 1.0, float dead_end_value = 1e3)
      : discount_(discount), dead_end_value_(dead_end_value), expansions_(0) { }
    virtual ~problem_t() { }

    float discount() const { return discount_; }
    float dead_end_value() const { return dead_end_value_; }

    size_t expansions() const {
        return expansions_;
    }
    void clear_expansions() const {
        expansions_ = 0;
    }

    virtual action_t number_actions(const T &s) const = 0;
    virtual const T& init() const = 0;
    virtual bool terminal(const T &s) const = 0;
    virtual bool dead_end(const T &s) const = 0;
    virtual bool applicable(const T &s, action_t a) const = 0;
    virtual float cost(const T &s, action_t a) const = 0;
    virtual void next(const T &s, action_t a, std::vector<std::pair<T, float> > &outcomes) const = 0;

    // sample next state given action using problem's dynamics
    std::pair<T, bool> sample(const T &s, action_t a) const {
        std::vector<std::pair<T, float> > outcomes;
        next(s, a, outcomes);
        unsigned osize = outcomes.size();
        assert(osize > 0);

        float r = Random::real();
        for( unsigned i = 0; i < osize; ++i ) {
            if( r < outcomes[i].second ) {
                return std::make_pair(outcomes[i].first, true);
            }
            r -= outcomes[i].second;
        }
        return std::make_pair(outcomes[0].first, true);
    }

    // sample next state given action uniformly among all possible next states
    std::pair<T, bool> usample(const T &s, action_t a) const {
        std::vector<std::pair<T, float> > outcomes;
        next(s, a, outcomes);
        unsigned osize = outcomes.size();
        return std::make_pair(outcomes[Random::uniform(osize)].first, true);
    }

    // sample next (unlabeled) state given action; probabilities are re-weighted
    std::pair<T, bool> nsample(const T &s, action_t a, const hash_t<T> &hash) const {
        std::vector<std::pair<T, float> > outcomes;
        next(s, a, outcomes);
        unsigned osize = outcomes.size();
        std::vector<bool> label(osize, false);

        size_t n = 0;
        float mass = 0;
        for( unsigned i = 0; i < osize; ++i ) {
            if( (label[i] = hash.solved(outcomes[i].first)) ) {
                mass += outcomes[i].second;
                ++n;
            }
        }

        mass = 1.0 - mass;
        n = osize - n;
        if( n == 0 ) return std::make_pair(s, false);

        float d = Random::real();
        for( unsigned i = 0; i < osize; ++i ) {
            if( !label[i] && ((n == 1) || (d <= outcomes[i].second / mass)) ) {
                return std::make_pair(outcomes[i].first, true);
            } else if( !label[i] ) {
                --n;
                d -= outcomes[i].second / mass;
            }
        }
        assert(0);
        return std::make_pair(s, false);
    }

    // type of sampling function
    typedef std::pair<T, bool> (problem_t<T>::*sample_function)(int) const;

    // compute the size of policy stored at hash table for given state
    size_t policy_size(hash_t<T> &hash, const T &s) const {
        hash.unmark_all();
        size_t size = policy_size_aux(hash, s);
        return size;
    }

    size_t policy_size_aux(hash_t<T> &hash, const T &s) const {
        std::vector<std::pair<T, float> > outcomes;
        size_t size = 0;
        if( !terminal(s) && !hash.marked(s) ) {
            hash.mark(s);
            std::pair<action_t, float> p = hash.bestQValue(s);
            next(s, p.first, outcomes);
            unsigned osize = outcomes.size();
            for( unsigned i = 0; i < osize; ++i ) {
                size += policy_size_aux(hash, outcomes[i].first);
            }
            ++size;
        }
        return size;
    }

    // print problem description
    virtual void print(std::ostream &os) const = 0;
};

template<typename T>
inline float hash_t<T>::QValue(const T &s, action_t a) const {
    if( problem_.terminal(s) ) return 0;

    std::vector<std::pair<T, float> > outcomes;
    problem_.next(s, a, outcomes);
    unsigned osize = outcomes.size();

    float qv = 0.0;
    for( unsigned i = 0; i < osize; ++i ) {
        qv += outcomes[i].second * this->value(outcomes[i].first);
    }
    return problem_.cost(s, a) + problem_.discount() * qv;
}

template<typename T>
inline float min_hash_t<T>::QValue(const T &s, action_t a) const {
    if( hash_t<T>::problem_.terminal(s) ) return 0;

    std::vector<std::pair<T, float> > outcomes;
    hash_t<T>::problem_.next(s, a, outcomes);
    unsigned osize = outcomes.size();

    float qv = std::numeric_limits<float>::max();
    for( unsigned i = 0; i < osize; ++i ) {
        qv = Utils::min(qv, this->value(outcomes[i].first));
    }
    return qv == std::numeric_limits<float>::max() ? std::numeric_limits<float>::max() : hash_t<T>::problem_.cost(s, a) + qv;
}

}; // namespace Problem

template<typename T>
inline std::ostream& operator<<(std::ostream &os, const Problem::problem_t<T> &problem) {
    problem.print(os);
    return os;
}

#undef DEBUG

#endif

