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

#ifndef ROLLOUT_H
#define ROLLOUT_H

#include "policy.h"

#include <iostream>
#include <sstream>
#include <cassert>
#include <limits>
#include <vector>

//#define DEBUG

namespace Online {

namespace Policy {

namespace Rollout {

// nested rollout policy
template<typename T> class rollout_t : public improvement_t<T> {
  protected:
    unsigned width_;
    unsigned depth_;
    unsigned nesting_;

  public:
    rollout_t(const policy_t<T> &base_policy,
              unsigned width,
              unsigned depth,
              unsigned nesting)
      : improvement_t<T>(base_policy),
        width_(width), depth_(depth), nesting_(nesting) {
        std::stringstream name_stream;
        name_stream << "rollout("
                    << "width=" << width_
                    << ",depth=" << depth_
                    << ",nesting=" << nesting_
                    << ")";
        policy_t<T>::set_name(name_stream.str());
    }
    virtual ~rollout_t() { }

    virtual const policy_t<T>* clone() const {
        return new rollout_t(improvement_t<T>::base_policy_, width_, depth_, nesting_);
    }

    virtual Problem::action_t operator()(const T &s) const {
        ++policy_t<T>::decisions_;
        Problem::action_t best_action = Problem::noop;
        float best_value = std::numeric_limits<float>::max();
        for( Problem::action_t a = 0; a < policy_t<T>::problem().number_actions(s); ++a ) {
            if( policy_t<T>::problem().applicable(s, a) ) {
                float value = 0;
                for( unsigned trial = 0; trial < width_; ++trial ) {
                    std::pair<T, bool> p = policy_t<T>::problem().sample(s, a);
                    value +=
                      policy_t<T>::problem().cost(s, a) +
                      policy_t<T>::problem().discount() * evaluate(p.first);
                }
                value /= width_;
                if( value < best_value ) {
                    best_value = value;
                    best_action = a;
                }
            }
        }
        assert(best_action != Problem::noop);
        return best_action;
    }
    virtual void print_stats(std::ostream &os) const {
        os << "stats: policy=" << policy_t<T>::name() << std::endl;
        os << "stats: decisions=" << policy_t<T>::decisions_ << std::endl;
        improvement_t<T>::base_policy_.print_stats(os);
    }

    float evaluate(const T &s) const {
        return Evaluation::evaluation(improvement_t<T>::base_policy_, s, 1, depth_);
    }
};

}; // namespace Rollout

template<typename T>
inline const policy_t<T>* make_nested_rollout(const policy_t<T> &base_policy,
                                              unsigned width,
                                              unsigned depth,
                                              unsigned nesting = 1) {
    std::vector<const policy_t<T>*> nested_policies;
    nested_policies.reserve(1 + nesting);
    nested_policies.push_back(&base_policy);
    for( unsigned level = 0; level < nesting; ++level ) {
        const policy_t<T> *policy = nested_policies.back();
        policy_t<T> *rollout =
          new Rollout::rollout_t<T>(*policy, width, depth, 1+level);
        nested_policies.push_back(rollout);
    }
    return nested_policies.back();
}

}; // namespace Policy

}; // namespace Online

#undef DEBUG

#endif

