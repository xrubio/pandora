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

#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "problem.h"
#include "hash.h"
#include "algorithm.h"
#include "parameters.h"

#include "aot.h"
#include "aot_gh.h"
#include "aot_path.h"
#include "uct.h"
#include "online_rtdp.h"
#include "rollout.h"
#include "parameters.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>
#include <strings.h>

//#define DEBUG

namespace Dispatcher {

inline const char *algorithm_name(int index) {
    switch( index ) {
        case  0: return "vi";
        case  1: return "slrtdp";
        case  2: return "ulrtdp";
        case  3: return "blrtdp";
        case  4: return "ilao";
        case  5: return "check";
        case  6: return "hdp";
        case  7: return "ldfs+";
        case  8: return "ldfs";
        case  9: return "hdp-i";
        case 10: return "simple-a*";
    }
    return 0;
}

template<typename T> struct algorithm_table_t {
    typedef size_t (*type)(const Problem::problem_t<T>&, const T&, Problem::hash_t<T>&, const Algorithm::parameters_t&);
    type operator[](int i) const {
        switch( i ) {
            case  0: return Algorithm::value_iteration<T>;
            case  1: return Algorithm::standard_lrtdp<T>;
            case  2: return Algorithm::uniform_lrtdp<T>;
            case  3: return Algorithm::bounded_lrtdp<T>;
            case  4: return Algorithm::improved_lao<T>;
            case  5: return Algorithm::plain_check<T>;
            case  6: return Algorithm::hdp_driver<T>;
            case  7: return Algorithm::ldfs_plus_driver<T>;
            case  8: return Algorithm::ldfs_driver<T>;
            case  9: return 0; //Algorithm::hdp_i<T>
            case 10: return Algorithm::simple_astar<T>;
            default: return 0;
        }
        return 0;
    }
};

template<typename T> struct result_t {
    int algorithm_;
    const char *algorithm_name_;
    unsigned seed_;
    float value_;
    unsigned trials_;
    unsigned updates_;
    unsigned expansions_;
    unsigned psize_;
    float atime_;
    float htime_;
    Problem::hash_t<T> *hash_;
};

template<typename T>
inline void solve(const Problem::problem_t<T> &problem, const Heuristic::heuristic_t<T> *heuristic, const T &s, unsigned bitmap, const Algorithm::parameters_t &parameters, std::vector<result_t<T> > &results) {

    // solve problem with algorithms
    unsigned index = 0;
    while( bitmap != 0 ) {
        for(; bitmap % 2 == 0; ++index, bitmap = bitmap >> 1 );

        typename algorithm_table_t<T>::type algorithm = algorithm_table_t<T>()[index];
        if( algorithm != 0 ) {
            result_t<T> result;
            result.algorithm_ = index;
            result.algorithm_name_ = algorithm_name(index);
            result.seed_ = parameters.seed_;
            Random::set_seed(parameters.seed_);

            float start_time = Utils::read_time_in_seconds();
            result.hash_ = new Problem::hash_t<T>(problem, new Heuristic::wrapper_t<T>(heuristic));
            problem.clear_expansions();
            if( heuristic != 0 ) heuristic->reset_stats();
            result.trials_ = (*algorithm)(problem, s, *result.hash_, parameters);
            result.value_ = result.hash_->value(s);
            result.updates_ = result.hash_->updates();
            result.expansions_ = problem.expansions();
            result.psize_ = std::numeric_limits<unsigned>::max();
            if( algorithm != static_cast<typename algorithm_table_t<T>::type>(Algorithm::simple_astar<T>) )
                result.psize_ = problem.policy_size(*result.hash_, s);

            float end_time = Utils::read_time_in_seconds();
            result.htime_ = !heuristic ? 0 : heuristic->total_time();
            float dtime = !heuristic ? 0 : heuristic->eval_time();
            result.atime_ = end_time - start_time - dtime;
            results.push_back(result);
        }
        bitmap = bitmap >> 1;
        ++index;
    }
}

template<typename T>
inline void print_result(std::ostream &os, const result_t<T> *result) {
    os << std::fixed;
    if( result == 0 ) {
         os << std::setw(4) << "#" << " "
            << std::setw(7) << "alg" << " "
            << std::setw(12) << "V*(s0)" << " "
            << std::setw(12) << "trials" << " "
            << std::setw(12) << "updates" << " "
            << std::setw(12) << "expansions" << " "
            << std::setw(12) << "hashsz" << " "
            << std::setw(12) << "psize" << " "
            << std::setw(12) << "atime" << " "
            << std::setw(12) << "htime"
            << std::endl;
    } else {
         os << std::setw(4) << result->algorithm_ << " "
            << std::setw(7) << result->algorithm_name_ << " "
            << std::setw(12) << std::setprecision(5) << result->value_ << std::setprecision(2) << " "
            << std::setw(12) << result->trials_ << " "
            << std::setw(12) << result->updates_ << " "
            << std::setw(12) << result->expansions_ << " "
            << std::setw(12) << result->hash_->size() << " "
            << std::setw(12) << result->psize_ << " "
            << std::setw(12) << result->atime_ << " "
            << std::setw(12) << result->htime_
            << std::endl;
    }
}

}; // namespace Dispatcher


namespace Online {

namespace Evaluation {

template<typename T>
const Policy::policy_t<T>*
  fetch_policy(const std::string &name,
               std::vector<std::pair<const Policy::policy_t<T>*, std::string> > &base_policies) {
    const Policy::policy_t<T> *policy = 0;
    for( unsigned i = 0; i < base_policies.size(); ++i ) {
        if( base_policies[i].second == name ) {
            policy = base_policies[i].first;
            break;
        }
    }
    return policy;
}

template<typename T>
const Heuristic::heuristic_t<T>*
  fetch_heuristic(const std::string &name,
                  std::vector<std::pair<const Heuristic::heuristic_t<T>*, std::string> > &heuristics) {
    const Heuristic::heuristic_t<T> *heuristic = 0;
    for( unsigned i = 0; i < heuristics.size(); ++i ) {
        if( heuristics[i].second == name ) {
            heuristic = heuristics[i].first;
            break;
        }
    }
    return heuristic;
}

inline bool policy_requires_heuristic(const std::string &policy_type) {
    if( policy_type == "finite-horizon-lrtdp" ) {
        return true;
    } else if( !policy_type.compare(0, 3, "aot") ) {
        return (policy_type.find("heuristic") != std::string::npos) ||
               (policy_type.find("g+h") != std::string::npos) ||
               (policy_type.find("path") != std::string::npos);
    } else
        return false;
}

template<typename T>
inline std::pair<const Policy::policy_t<T>*, std::string>
  select_policy(const Problem::problem_t<T> &problem,
                const std::string &base_name,
                const std::string &policy_type,
                std::vector<std::pair<const Policy::policy_t<T>*, std::string> > &base_policies,
                std::vector<std::pair<const Heuristic::heuristic_t<T>*, std::string> > &heuristics,
                const parameters_t &par) {

    std::stringstream ss;

    // fetch base policy/heuristic
    const Policy::policy_t<T> *base_policy = 0;
    const Heuristic::heuristic_t<T> *heuristic = 0;
    if( policy_requires_heuristic(policy_type) ) {
        heuristic = fetch_heuristic(base_name, heuristics);
        if( heuristic == 0 ) {
            ss << "error: inexistent heuristic: " << base_name;
            return std::make_pair(base_policy, ss.str());
        }
    } else {
        base_policy = fetch_policy(base_name, base_policies);
        if( base_policy == 0 ) {
            ss << "error: inexistent base policy: " << base_name;
            return std::make_pair(base_policy, ss.str());
        }
    }

    // make compound policy
    const Policy::policy_t<T> *policy = 0;

    if( policy_type == "direct" ) {
        ss << base_name;
        policy = base_policy->clone();
    } else if( policy_type == "rollout" ) {
        ss << policy_type << "(" << base_name
           << ",width=" << par.width_
           << ",depth=" << par.depth_
           << ",nesting=" << par.par1_
           << ")";
        policy = Policy::make_nested_rollout(*base_policy, par.width_, par.depth_, par.par1_);
    } else if( (policy_type.length() >= 3) && !policy_type.compare(0, 3, "uct") ) {
        // UCT family
        ss << policy_type << "(" << base_name
           << ",width=" << par.width_
           << ",depth=" << par.depth_
           << ",par=" << par.par1_
           << ")";
        bool random_ties = policy_type == "uct/random-ties";
        policy = Policy::make_uct(*base_policy, par.width_, par.depth_, par.par1_, random_ties);
    } else if( (policy_type.length() >= 3) && !policy_type.compare(0, 3, "aot") ) {
        // Determine type and modifiers
        bool random_ties = false;
        bool delayed = false;
        bool random_leaf = false;
        bool g_plus_h = false;
        bool path = false;
        if( policy_type.length() > 3 ) {
            random_ties = policy_type.find("random-ties") != std::string::npos;
            delayed = policy_type.find("delayed") != std::string::npos;
            random_leaf = policy_type.find("random-leaf") != std::string::npos;
            g_plus_h = policy_type.find("g+h") != std::string::npos;
            path = policy_type.find("path") != std::string::npos;
        }

        // Constraint: delayed => not random-leaf, not heuristic, not g+h
        // Constraint: random-leaf => not delayed, not heuristic, not g+h
        // Constraint: g+h => not delayed, not random_leaf, heuristic
        if( delayed && random_leaf ) {
            ss << "error: AOT/delayed & AOT/random-leaf are incompatible.";
        } else if( delayed && (heuristic != 0) ) {
            ss << "error: AOT/delayed & AOT/heuristic are incompatible.";
        } else if( delayed && g_plus_h ) {
            ss << "error: AOT/delayed & AOT/g+h are incompatible.";
        } else if( random_leaf && heuristic ) {
            ss << "error: AOT/random-leaf & AOT/heuristic are incompatible.";
        } else if( random_leaf && g_plus_h ) {
            ss << "error: AOT/random-leaf & AOT/g+h are incompatible.";
        } else if( g_plus_h && (heuristic == 0) ) {
            ss << "error: AOT/g+h required AOT/heuristic.";
        }
        if( ss.str().length() > 0 ) return std::make_pair(policy, ss.str());
            
        // AOT family
        std::stringstream aot_name;
        aot_name << "aot/";
        if( heuristic ) aot_name << "heuristic,";
        if( random_leaf ) aot_name << "random-leaf,";
        if( g_plus_h ) aot_name << "g+h,";
        if( path ) aot_name << "path,";
        if( delayed ) aot_name << "delayed,";
        if( random_ties ) aot_name << "random-ties,";
        std::string tmp_name = aot_name.str();
        tmp_name.erase(tmp_name.size() - 1, 1);
        
        ss << tmp_name << "(" << base_name;
        if( g_plus_h ) ss << ",w=" << par.weight_;
        ss << ",width=" << par.width_
           << ",depth=" << par.depth_
           << ",p=" << par.par1_
           << ",exp=" << par.par2_
           << ")";

        // Make sure we have some base_policy to construct AOT
        if( base_policy == 0 ) base_policy = new Policy::random_t<T>(problem);
            
        if( random_leaf ) {
            policy = Policy::make_aot(*base_policy, par.width_, par.depth_, par.par1_, random_ties, false, par.par2_, 1, 1, 1);
        } else if( g_plus_h ) {
            policy = Policy::make_aot_gh(*base_policy, par.weight_, par.width_, par.depth_, par.par1_, random_ties, false, par.par2_);
        } else if( path ) {
#ifdef EXPERIMENTAL
            extern const Policy::policy_t<T> *global_base_policy;
            policy = Policy::make_aot_path(*global_base_policy, par.width_, par.depth_, par.par1_, random_ties, false, par.par2_);
#else
            std::cout << "'path' option not supported (enable EXPERIMENTAL setup)" << std::endl;
#endif
        } else {
            policy = Policy::make_aot(*base_policy, par.width_, par.depth_, par.par1_, random_ties, delayed, par.par2_);
        }

        if( heuristic != 0 ) {
            if( g_plus_h ) {
                dynamic_cast<const Online::Policy::AOT_GH::aot_t<T>*>(policy)->set_heuristic(heuristic);
            } else if( path ) {
                dynamic_cast<const Online::Policy::AOT_PATH::aot_t<T>*>(policy)->set_heuristic(heuristic);
            } else {
                dynamic_cast<const Online::Policy::AOT::aot_t<T>*>(policy)->set_heuristic(heuristic);
            }
        }
    } else if( policy_type == "finite-horizon-lrtdp" ) {
        ss << policy_type << "(" << base_name
           << ",horizon=" << par.depth_
           << ",max-trials=" << par.width_
           << ",labeling=" << (par.labeling_ ? "true" : "false")
           << ")";
        policy =
          Policy::make_finite_horizon_lrtdp(problem, *heuristic, par.depth_, par.width_, par.labeling_, false);
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

}; // namespace Online

#undef DEBUG

#endif

