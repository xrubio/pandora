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

#ifndef HEURISTIC_H
#define HEURISTIC_H

#include "hash.h"
#include "problem.h"
#include "parameters.h"
#include "utils.h"

#include <iostream>
#include <cassert>
#include <limits>
#include <limits.h>

//#define DEBUG

// forward reference
namespace Algorithm {
  template<typename T> size_t value_iteration(const Problem::problem_t<T>&, const T &s, Problem::hash_t<T>&, const parameters_t&);
};

namespace Heuristic {

template<typename T> class heuristic_t {
  protected:
    std::string name_;

  public:
    heuristic_t(const std::string &name) : name_(name) { }
    heuristic_t() : name_("") { }
    virtual ~heuristic_t() { }
    void set_name(const std::string &name) { name_ = name; }
    const std::string& name() const { return name_; }
    virtual float value(const T &s) const = 0;
    virtual void reset_stats() const = 0;
    virtual float setup_time() const = 0;
    virtual float eval_time() const = 0;
    virtual size_t size() const = 0;
    virtual void dump(std::ostream &os) const = 0;
    float total_time() const { return setup_time() + eval_time(); }
};

template<typename T> class zero_heuristic_t : public heuristic_t<T> {
  public:
    zero_heuristic_t() : heuristic_t<T>("zero()") { }
    virtual ~zero_heuristic_t() { }
    virtual float value(const T &s) const { return 0; }
    virtual void reset_stats() const { }
    virtual float setup_time() const { return 0; }
    virtual float eval_time() const { return 0; }
    virtual size_t size() const { return 0; }
    virtual void dump(std::ostream &os) const { }
};

template<typename T> class min_min_heuristic_t : public heuristic_t<T> {
  protected:
    float divisor_;
    const Problem::problem_t<T> &problem_;
    mutable Problem::min_hash_t<T> hash_;
    mutable float time_;

  public:
    min_min_heuristic_t(const Problem::problem_t<T> &problem, float divisor = 1.0)
      : heuristic_t<T>("min-min()"), divisor_(divisor), problem_(problem), hash_(problem), time_(0) {

      Algorithm::parameters_t parameters;
      parameters.vi.max_number_iterations_ = std::numeric_limits<unsigned>::max();

      float start_time = Utils::read_time_in_seconds();
      Algorithm::value_iteration<T>(problem_, problem_.init(), hash_, parameters);
      float end_time = Utils::read_time_in_seconds();
      time_ = end_time - start_time;
    } 
    virtual ~min_min_heuristic_t() { }
    virtual float value(const T &s) const { return hash_.value(s) / divisor_; }
    virtual void reset_stats() const { }
    virtual float setup_time() const { return time_; }
    virtual float eval_time() const { return 0; }
    virtual size_t size() const { return hash_.size(); }
    virtual void dump(std::ostream &os) const { hash_.dump(os); }
};

template<typename T> class hash_heuristic_t : public heuristic_t<T> {
  protected:
    const Problem::hash_t<T> &hash_;

  public:
    hash_heuristic_t(const Problem::hash_t<T> &hash)
      : heuristic_t<T>("hash()"), hash_(hash) { }
    virtual ~hash_heuristic_t() { }
    virtual float value(const T &s) const { return hash_.value(s); }
    virtual float setup_time() const { return 0; }
    virtual float eval_time() const { return 0; }
    virtual size_t size() const { return hash_.size(); }
    virtual void dump(std::ostream &os) const { hash_.dump(os); }
};

template<typename T> struct wrapper_t : public Hash::hash_map_t<T>::eval_function_t {
    const heuristic_t<T> *heuristic_;
    wrapper_t(const heuristic_t<T> *heuristic = 0) : heuristic_(heuristic) { }
    virtual ~wrapper_t() { }
    float operator()(const T &s) const {
        return heuristic_ == 0 ? 0 : heuristic_->value(s);
    }
};

}; // namespace Heuristic

#undef DEBUG

#endif

