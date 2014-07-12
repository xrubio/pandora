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

#ifndef EVALUATION_H
#define EVALUATION_H

#include <iostream>
#include <sstream>
#include <iomanip>
#include <strings.h>

#include "aot.h"
#include "aot2.h"
#include "uct.h"
#include "uct2.h"

namespace Evaluation {

struct parameters_t {
    unsigned evaluation_trials_;
    unsigned evaluation_depth_;
    unsigned width_;
    unsigned depth_;
    float par1_;
    unsigned par2_;
    parameters_t()
      : evaluation_trials_(1000), evaluation_depth_(100),
        width_(0), depth_(0), par1_(0), par2_(0) { }
};

template<typename T>
inline std::pair<const Policy::policy_t<T>*, std::string>
  select_policy(const std::string &base_name,
                const std::string &policy_type,
                std::vector<std::pair<const Policy::policy_t<T>*, std::string> > bases,
                const parameters_t &par) {

    std::stringstream ss;

    // locate base policy
    const Policy::policy_t<T> *base = 0;
    std::string base_str = base_name;
    for( unsigned i = 0; i < bases.size(); ++i ) {
        if( bases[i].second == base_name ) {
            base = bases[i].first;
            break;
        }
    }
    if( base == 0 ) {
        ss << "inexistent base: " << base_name;
        return std::make_pair(base, ss.str());
    }

    // make compound policy
    const Policy::policy_t<T> *policy = 0;

    if( policy_type == "direct" ) {
        policy = base->clone();
        ss << base_str;
    } else if( policy_type == "rollout" ) {
        ss << "nrollout(" << base_str
           << ",width=" << par.width_
           << ",depth=" << par.depth_
           << ",nesting=" << par.par2_
           << ")";
        policy = Policy::make_nested_rollout(*base, par.width_, par.depth_, par.par2_);
    } else if( policy_type == "uct" ) {
        ss << "uct(" << base_str
           << ",width=" << par.width_
           << ",depth=" << par.depth_
           << ",par=" << par.par1_
           << ")";
        policy = Policy::make_uct(*base, par.width_, par.depth_, par.par1_);
    } else if( policy_type == "nuct" ) {
        ss << "nuct(" << base_str
           << ",width=" << par.width_
           << ",depth=" << par.depth_
           << ",par=" << par.par1_
           << ")";
        policy = Policy::make_uct2(*base, par.width_, par.depth_, par.par1_);
    } else if( policy_type == "aot" ) {
        ss << "aot(" << base_str
           << ",width=" << par.width_
           << ",depth=" << par.depth_
           << ",p=" << par.par1_
           << ",exp=" << par.par2_
           << ")";
        policy =
          Policy::make_aot(*base, par.width_, par.depth_, par.par1_, false, par.par2_);
    } else if( policy_type == "aot*" ) {
        ss << "aot*(" << base_str
           << ",width=" << par.width_
           << ",depth=" << par.depth_
           << ",p=" << par.par1_
           << ",exp=" << par.par2_
           << ")";
        policy =
          Policy::make_aot(*base, par.width_, par.depth_, par.par1_, true, par.par2_);
    } else if( policy_type == "aot-value" ) {
        ss << "aot-value(" << base_str
           << ",width=" << par.width_
           << ",depth=" << par.depth_
           << ",p=" << par.par1_
           << ",exp=" << par.par2_
           << ")";
        policy =
          Policy::make_aot(*base, par.width_, par.depth_, par.par1_, false, par.par2_);
    } else if( policy_type == "aot-random" ) {
        ss << "aot-random(" << base_str
           << ",width=" << par.width_
           << ",depth=" << par.depth_
           << ",p=" << par.par1_
           << ",exp=" << par.par2_
           << ")";
        policy =
          Policy::make_aot(*base, par.width_, par.depth_, par.par1_, false, par.par2_, 1, 1, 1);
    } else if( policy_type == "naot" ) {
        ss << "naot(" << base_str
           << ",width=" << par.width_
           << ",depth=" << par.depth_
           << ",p=" << par.par1_
           << ",exp=" << par.par2_
           << ")";
        policy =
          Policy::make_aot2(*base, par.width_, par.depth_, par.par1_, false, par.par2_);
    } else if( policy_type == "naot*" ) {
        ss << "naot*(" << base_str
           << ",width=" << par.width_
           << ",depth=" << par.depth_
           << ",p=" << par.par1_
           << ",exp=" << par.par2_
           << ")";
        policy =
          Policy::make_aot2(*base, par.width_, par.depth_, par.par1_, true, par.par2_);
    } else if( policy_type == "naot-value" ) {
        ss << "naot-value(" << base_str
           << ",width=" << par.width_
           << ",depth=" << par.depth_
           << ",p=" << par.par1_
           << ",exp=" << par.par2_
           << ")";
        policy =
          Policy::make_aot2(*base, par.width_, par.depth_, par.par1_, false, par.par2_);
    } else if( policy_type == "naot-random" ) {
        ss << "naot-random(" << base_str
           << ",width=" << par.width_
           << ",depth=" << par.depth_
           << ",p=" << par.par1_
           << ",exp=" << par.par2_
           << ")";
        policy =
          Policy::make_aot2(*base, par.width_, par.depth_, par.par1_, false, par.par2_, 1, 1, 1);
    } else {
        ss << "inexistent policy: " << policy_type;
    }

    return std::make_pair(policy, ss.str());
}

template<typename T>
inline std::pair<std::pair<float, float>, float>
  evaluate_policy(const Policy::policy_t<T> &policy,
                  const parameters_t &par,
                  bool verbose = false) {
    float start_time = Utils::read_time_in_seconds();
    std::pair<float, float> value =
      Evaluation::evaluation_with_stdev(policy,
                                        policy.problem().init(),
                                        par.evaluation_trials_,
                                        par.evaluation_depth_,
                                        verbose);
    float time = Utils::read_time_in_seconds() - start_time;
    return std::make_pair(value, time);
}

}; // namespace Evaluation

#endif

