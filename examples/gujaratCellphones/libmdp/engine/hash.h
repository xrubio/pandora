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

#ifndef HASH_H
#define HASH_H

#include "utils.h"

#include <iostream>
#include <cassert>
#include <limits>
#include <limits.h>
#include <vector>

#include <tr1/unordered_map>
#include <tr1/unordered_set>

//#define DEBUG

namespace Problem {

#ifndef __ACTION_TYPE
#define __ACTION_TYPE
    typedef int action_t;
    const action_t noop = -1;
#endif

};

namespace Hash {

// class for stored data values
class data_t {
    float value_;
    bool solved_;
    bool marked_;
    size_t count_;
    Problem::action_t action_;

    union {
        struct {
            size_t low_;
            size_t idx_;
        } scc_;

        struct {
            float g_;
            const data_t *parent_;
        } bfs_;
    };

  public:
    data_t(float value = 0, bool solved = false, bool marked = false, size_t count = 0)
      : value_(value), solved_(solved), marked_(marked),
        count_(count), action_(Problem::noop) {
        scc_.low_ = std::numeric_limits<unsigned>::max();
        scc_.idx_ = std::numeric_limits<unsigned>::max();
    }
    ~data_t() { }

    float value() const { return value_; }
    void update(float value) {
        //assert(value_ <= value);
        //value_ = Utils::max(value, value_);
        value_ = value; // TODO: restore!
    }

    bool solved() const { return solved_; }
    void solve() { solved_ = true; }
    void unsolve() { solved_ = false; }

    bool marked() const { return marked_ ; }
    void mark() { marked_ = true; }
    void unmark() { marked_ = false; }

    size_t count() const { return count_; }
    void inc_count() { ++count_; }
    void clear_count() { count_ = 0; }

    Problem::action_t action() const { return action_; }
    void set_action(Problem::action_t action) { action_ = action; }

    size_t scc_low() const { return scc_.low_; }
    void set_scc_low(size_t low) { scc_.low_ = low; }
    size_t scc_idx() const { return scc_.idx_; }
    void set_scc_idx(size_t idx) { scc_.idx_ = idx; }

    float g() const { return bfs_.g_; }
    void set_g(float g) { bfs_.g_ = g; }
    float h() const { return value_; }
    float f() const { return bfs_.g_ + value_; }
    const data_t* parent() const { return bfs_.parent_; }
    void set_parent(const data_t *parent) { bfs_.parent_ = parent; }

    void print(std::ostream &os) const {
        os << "(" << value_
           << ", " << (solved_ ? 1 : 0)
           << ", " << (marked_ ? 1 : 0)
           << ", " << count_
           << ", " << action_
           << ")";
    }
};

}; // namespace Hash


inline std::ostream& operator<<(std::ostream &os, const Hash::data_t &data) {
    data.print(os);
    return os;
}


namespace Hash {

// Hash function for state
template<typename T>
class hash_function_t {
  public:
    size_t operator()(const T &s) const { return s.hash(); }
};

template<typename T, typename D, typename F=Hash::hash_function_t<T> >
class generic_hash_map_t : public std::tr1::unordered_map<T, D, F> {
};

template<typename T, typename F=Hash::hash_function_t<T> >
class generic_hash_set_t : public std::tr1::unordered_map<T, F> {
};

template<typename T, typename F=Hash::hash_function_t<T> >
class hash_map_t : public generic_hash_map_t<T, Hash::data_t*, F> {
  public: // iterators
    typedef generic_hash_map_t<T, Hash::data_t*, F> base_type;
    typedef typename base_type::iterator iterator;
    typedef typename base_type::const_iterator const_iterator;
    const_iterator begin() const { return base_type::begin(); }
    const_iterator end() const { return base_type::end(); }
    iterator begin() { return base_type::begin(); }
    iterator end() { return base_type::end(); }

  public: // evaluation functions
    struct eval_function_t {
        virtual ~eval_function_t() { }
        virtual float operator()(const T &s) const = 0;
    };

    struct default_eval_function_t : public eval_function_t {
        float operator()(const T &s) const { return 0; }
    };

  protected:
    const eval_function_t *eval_function_;

    Hash::data_t* push(const T &s, Hash::data_t *d) {
        base_type::insert(std::make_pair(s, d));
        return d;
    }

    iterator lookup(const T &s) { return base_type::find(s); }
    const_iterator lookup(const T &s) const { return const_cast<hash_map_t<T>*>(this)->find(s); }

  public:
    hash_map_t(eval_function_t *eval_function = 0)
      : eval_function_(eval_function) {
    }
    virtual ~hash_map_t() {
        for( iterator hi = begin(); hi != end(); ++hi )
            delete (*hi).second;
    }

    float default_value(const T &s) const { return eval_function_ == 0 ? 0 : (*eval_function_)(s); }

    Hash::data_t* data_ptr(const T &s) {
        iterator di = lookup(s);
        if( di == end() )
            return push(s, new Hash::data_t(default_value(s)) );
         else
            return (*di).second;
    }

    float value(const T &s) const {
        const_iterator di = lookup(s);
        return di == end() ? default_value(s) : (*di).second->value();
    }
    void update(const T &s, float value) {
        iterator di = lookup(s);
        if( di == end() )
            push(s, new Hash::data_t(value, false, false));
        else
            (*di).second->update(value);
     }

    bool solved(const T &s) const {
        const_iterator di = lookup(s);
        return di == end() ? false : (*di).second->solved();
    }
    void solve(const T &s) {
        iterator di = lookup(s);
        if( di == end() )
            push(s, new Hash::data_t(default_value(s), true, false));
        else
            (*di).second->solve();
    }

    bool marked(const T &s) const {
        const_iterator di = lookup(s);
        return di == end() ? false : (*di).second->marked();
    }
    void mark(const T &s) {
        iterator di = lookup(s);
        if( di == end() )
            push(s, new Hash::data_t(default_value(s), false, true));
        else
            (*di).second->mark();
    }
    void unmark(const T &s) {
        iterator di = lookup(s);
        if( di != end() )
            (*di).second->unmark();
    }
    void unmark_all() {
        for( iterator di = begin(); di != end(); ++di )
            (*di).second->unmark();
    }

    size_t count(const T &s) const {
        const_iterator di = lookup(s);
        return di == end() ? 0 : (*di).second->count();
    }
    void inc_count(const T &s) {
        iterator di = lookup(s);
        if( di == end() )
            push(s, new Hash::data_t(default_value(s), false, false, 1));
        else
            (*di).second->inc_count();
    }
    void clear_count(const T &s) {
        iterator di = lookup(s);
        if( di != end() )
            (*di).second->clear_count();
    }

    Problem::action_t action(const T &s) const {
        const_iterator di = lookup(s);
        if( di == end() )
            return Problem::noop;
        else
            return (*di).second->action();
    }
    void set_action(const T &s, Problem::action_t action) {
        iterator di = lookup(s);
        if( di != end() )
            (*di).second->set_action(action);
    }

    size_t scc_low(const T &s) const {
        const_iterator di = lookup(s);
        if( di == end() )
            return std::numeric_limits<unsigned>::max();
        else
            return (*di).second->scc_low();
    }
    size_t scc_idx(const T &s) const {
        const_iterator di = lookup(s);
        if( di == end() )
            return std::numeric_limits<unsigned>::max();
        else
            return (*di).second->scc_idx();
    }

    void dump(std::ostream &os) const {
        for( const_iterator di = begin(); di != end(); ++di )
            os << (*di).first << " : " << *(*di).second << std::endl;
    }

};

}; // namespace Hash

#undef DEBUG

#endif

