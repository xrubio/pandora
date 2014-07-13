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

#ifndef ONLINE_RTDP_H
#define ONLINE_RTDP_H

#include "policy.h"

#include <iostream>
#include <cassert>
#include <limits>
#include <vector>
#include <queue>

//#define DEBUG

namespace Online {

namespace Policy {

namespace RTDP {

////////////////////////////////////////////////
//
// Hash Table
//

template<typename T> struct node_ref_t;

template<typename T> struct node_t : public std::pair<T, unsigned> {
    node_t(const T &s, unsigned d)
      : std::pair<T, unsigned>(s, d) { }
    node_t(const node_ref_t<T> &node);
    ~node_t() { }
    const T& state() const { return std::pair<T, unsigned>::first; }
    unsigned depth() const { return std::pair<T, unsigned>::second; }
    void set_state(const T &s) { std::pair<T, unsigned>::first = s; }
    void set_depth(unsigned d) { std::pair<T, unsigned>::second = d; }
};

template<typename T> struct node_ref_t : public std::pair<const T&, unsigned> {
    node_ref_t(const T &s, unsigned d)
      : std::pair<const T&, unsigned>(s, d) { }
    node_ref_t(const node_t<T> &node)
      : std::pair<const T&, unsigned>(node.state(), node.depth()) { }
    ~node_ref_t() { }
    const T& state() const { return std::pair<const T&, unsigned>::first; }
    unsigned depth() const { return std::pair<const T&, unsigned>::second; }
    void set_state(const T &s) { std::pair<const T&, unsigned>::first = s; }
    void set_depth(unsigned d) { std::pair<const T&, unsigned>::second = d; }
};

template<typename T> node_t<T>::node_t(const node_ref_t<T> &node)
  : std::pair<T, unsigned>(node.state(), node.depth()) {
}

template<typename T> struct map_functions_t {
    size_t operator()(const node_t<T> &node) const {
        return node.state().hash();
    }
};

struct data_t {
    float value_;
    bool labeled_;
    data_t() : value_(std::numeric_limits<float>::max()), labeled_(false) { }
    data_t(const data_t &data)
      : value_(data.value_), labeled_(data.labeled_) { }
};

template<typename T> class hash_table_t :
  public Hash::generic_hash_map_t<node_t<T>, data_t*, map_functions_t<T> > {

  public:
    typedef typename Hash::generic_hash_map_t<node_t<T>, data_t*, map_functions_t<T> >
            base_type;
    typedef typename base_type::const_iterator const_iterator;
    const_iterator begin() const { return base_type::begin(); }
    const_iterator end() const { return base_type::end(); }

  public:
    hash_table_t() { }
    virtual ~hash_table_t() { }

    const data_t* data_ptr(const node_ref_t<T> &node) const {
        const_iterator it = find(node);
        return it == end() ? 0 : it->second;
    }

    data_t* data_ptr(const node_ref_t<T> &node) {
        const_iterator it = find(node);
        return it == end() ? 0 : it->second;
    }

    data_t* insert(const node_ref_t<T> &node) {
        data_t *dptr = new data_t;
        base_type::insert(std::make_pair(node, dptr));
        return dptr;
    }

    data_t* get_data_ptr(const node_ref_t<T> &node) {
        data_t *dptr = data_ptr(node);
        return dptr == 0 ? insert(node) : dptr;
    }

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

template<typename T> class finite_horizon_lrtdp_t : public policy_t<T> {
  protected:
    const Heuristic::heuristic_t<T> &heuristic_;
    unsigned horizon_;
    unsigned max_trials_;
    bool labeling_;
    bool random_ties_;
    mutable hash_table_t<T> table_;
    mutable unsigned total_number_expansions_;

  public:
    finite_horizon_lrtdp_t(const Problem::problem_t<T> &problem,
                           const Heuristic::heuristic_t<T> &heuristic,
                           unsigned horizon,
                           unsigned max_trials,
                           bool labeling,
                           bool random_ties)
      : policy_t<T>(problem), heuristic_(heuristic),
        horizon_(horizon), max_trials_(max_trials),
        labeling_(labeling), random_ties_(random_ties) {
        std::stringstream name_stream;
        name_stream << "finite-horizon-lrtdp("
                    << "horizon=" << horizon_
                    << ",max-trials=" << max_trials_
                    << ",labeling=" << (labeling_ ? "true" : "false")
                    << ")";
        policy_t<T>::set_name(name_stream.str());
    }
    virtual ~finite_horizon_lrtdp_t() { }

    virtual Problem::action_t operator()(const T &s) const {
        // initialize
        ++policy_t<T>::decisions_;
        clear_table();
        node_ref_t<T> root(s, 0);
        data_t *root_dptr = table_.get_data_ptr(root);

        // perform trials
        for( unsigned trial = 0; (trial < max_trials_) && !labeled(root_dptr); ++trial ) {
            lrtdp_trial(root, root_dptr);
        }
#ifdef DEBUG
        std::cout << "finite_horizon_lrtdp_t: root-value=" << root_dptr->value_
                  << ", labeled=" << (root_dptr->labeled_ ? "true" : "false")
                  << std::endl;
#endif
        return best_action(root, random_ties_);
    }

    virtual const policy_t<T>* clone() const {
        return new finite_horizon_lrtdp_t(policy_t<T>::problem(), heuristic_, horizon_, max_trials_, labeling_, random_ties_);
    }

    virtual void print_stats(std::ostream &os) const {
        os << "stats: policy=" << policy_t<T>::name() << std::endl;
        os << "stats: decisions=" << policy_t<T>::decisions_ << std::endl;
        os << "stats: #expansions=" << total_number_expansions_ << std::endl;
    }

    void clear_table() const {
        for( typename hash_table_t<T>::const_iterator it = table_.begin(); it != table_.end(); ++it ) {
            delete it->second;
        }
        table_.clear();
    }

    Problem::action_t best_action(const node_ref_t<T> &node, bool random_ties) const {
        std::vector<Problem::action_t> actions;
        int nactions = policy_t<T>::problem().number_actions(node.state());
        float best_value = std::numeric_limits<float>::max();
        actions.reserve(random_ties ? nactions : 1);
        for( Problem::action_t a = 0; a < nactions; ++a ) {
            if( policy_t<T>::problem().applicable(node.state(), a) ) {
                std::pair<float, bool> p = QValue(node, a);
                if( p.first <= best_value ) {
                    if( p.first < best_value ) {
                        best_value = p.first;
                        actions.clear();
                    }
                    if( random_ties || actions.empty() )
                        actions.push_back(a);
                }
            }
        }
        assert(!actions.empty());
        assert(actions.size() == 1);
        return actions[Random::uniform(actions.size())];
    }

    std::pair<float, bool> QValue(const node_ref_t<T> &node, Problem::action_t a) const {
        ++total_number_expansions_;
        float qvalue = 0;
        bool all_children_labeled = true;
        std::vector<std::pair<T, float> > outcomes;
        policy_t<T>::problem().next(node.state(), a, outcomes);
        for( int i = 0, isz = outcomes.size(); i < isz; ++i ) {
            const T &state = outcomes[i].first;
            float prob = outcomes[i].second;
            node_ref_t<T> next_node(state, 1 + node.depth());
            std::pair<float, bool> p = value(next_node);
            qvalue += prob * p.first;
            all_children_labeled = all_children_labeled && p.second;
        }
        qvalue += policy_t<T>::problem().cost(node.state(), a);
        return std::make_pair(qvalue, all_children_labeled);
    }

    std::pair<std::pair<float, Problem::action_t>, bool> bestQValue(const node_ref_t<T> &node) const {
        bool all_labeled = false;
        Problem::action_t best_action = Problem::noop;
        float best_value = std::numeric_limits<float>::max();
        int nactions = policy_t<T>::problem().number_actions(node.state());
        for( Problem::action_t a = 0; a < nactions; ++a ) {
            if( policy_t<T>::problem().applicable(node.state(), a) ) {
                std::pair<float, bool> p = QValue(node, a);
                if( (best_action == Problem::noop) || (p.first < best_value) ) {
                    all_labeled = p.second;
                    best_value = p.first;
                    best_action = a;
                }
            }
        }
        assert(best_action != Problem::noop);
        return std::make_pair(std::make_pair(best_value, best_action), all_labeled);
    }

    std::pair<float, bool> value(const node_ref_t<T> &node) const {
        const data_t *dptr = table_.data_ptr(node);
        if( dptr != 0 ) {
            return std::make_pair(dptr->value_, dptr->labeled_);
        } else {
            float hvalue = 0; // default value for terminal node
            if( dead_end(node) ) {
                hvalue = policy_t<T>::problem().dead_end_value();
            } else if( !terminal(node) ) {
                hvalue = heuristic_.value(node.state());
            }
            return std::make_pair(hvalue, false);
        }
    }

    void update_value(data_t *dptr, float value) const {
        assert(dptr != 0);
        dptr->value_ = value;
    }

    void update_value(const node_ref_t<T> &node, float value) const {
        data_t *dptr = table_.data_ptr(node);
        if( dptr == 0 ) dptr = table_.insert(node);
        update_value(dptr, value);
    }

    bool labeled(const data_t *dptr) const {
        assert(dptr != 0);
        return dptr->labeled_;
    }

    bool labeled(const node_ref_t<T> &node) const {
        if( !labeling_ ) {
            return false;
        } else {
            data_t *dptr = table_.data_ptr(node);
            return dptr == 0 ? false : labeled(dptr);
        }
    }

    bool try_label(const node_ref_t<T> &node, data_t *dptr) const {
        bool labeled = true;
        if( dead_end(node) ) {
            dptr->value_ = policy_t<T>::problem().dead_end_value();
        } else if( terminal(node) ) {
            dptr->value_ = 0;
        } else {
            std::pair<std::pair<float, Problem::action_t>, bool> p = bestQValue(node);
            if( (p.first.first == dptr->value_) && p.second ) {
                labeled = true;
            } else {
                dptr->value_ = p.first.first;
                labeled = p.second;
            }
        }
        dptr->labeled_ = labeled;
        return labeled;
    }

    bool terminal(const node_ref_t<T> &node) const {
        return (node.depth() >= horizon_) || policy_t<T>::problem().terminal(node.state());
    }

    bool dead_end(const node_ref_t<T> &node) const {
        return policy_t<T>::problem().dead_end(node.state());
    }

    void lrtdp_trial(const node_ref_t<T> &root, data_t *root_dptr) const {
        // set up queue of visited nodes (for labeling)
        std::vector<std::pair<T, data_t*> > visited;
        if( labeling_ ) {
            visited.reserve(horizon_);
            visited.push_back(std::make_pair(root.state(), root_dptr));
        }

#ifdef DEBUG
        std::cout << "lrtdp_trial: new trial" << std::endl;
#endif

        // lrtdp trial
        node_t<T> node(root);
        data_t *dptr = root_dptr;
        bool node_is_dead_end = dead_end(node);
        while( !labeled(dptr) && !terminal(node) && !node_is_dead_end ) {
#ifdef DEBUG
            std::cout << "lrtdp_trial: state=" << node.state()
                      << ", depth=" << node.depth()
                      << ", dptr=" << dptr << std::endl;
#endif
            std::pair<float, Problem::action_t> p = bestQValue(node).first;
            Problem::action_t best_action = p.second;
            update_value(node, p.first);
            node.set_state(policy_t<T>::problem().sample(node.state(), best_action).first);
            node.set_depth(1 + node.depth());
            node_is_dead_end = dead_end(node);
            dptr = table_.get_data_ptr(node);
            if( labeling_ ) visited.push_back(std::make_pair(node.state(), dptr));
        }

#ifdef DEBUG
        std::cout << "lrtdp_trial: end state=" << node.state()
                  << ", depth=" << node.depth()
                  << ", dptr=" << dptr << std::endl;
#endif

        if( !labeled(dptr) ) {
            dptr->value_ = node_is_dead_end ? policy_t<T>::problem().dead_end_value() : 0;
            if( labeling_ ) dptr->labeled_ = true;
        }

        // try labeling nodes in reverse visited order
        for( unsigned depth = visited.size(); depth > 0; --depth ) {
            node_t<T> node(visited[depth - 1].first, depth - 1);
            data_t *dptr = visited[depth - 1].second;
            bool has_label = labeled(dptr) || try_label(node, dptr);
#ifdef DEBUG
            std::cout << "lrtdp_trial: labeling: state=" << node.state()
                      << ", dptr=" << dptr
                      << std::endl;
#endif
            if( !has_label ) break;
        }
    }

};

}; // namespace RTDP

template<typename T>
inline const policy_t<T>* make_finite_horizon_lrtdp(const Problem::problem_t<T> &problem,
                                                    const Heuristic::heuristic_t<T> &heuristic,
                                                    unsigned horizon,
                                                    unsigned max_trials,
                                                    bool labeling,
                                                    bool random_ties) {
    return new RTDP::finite_horizon_lrtdp_t<T>(problem,
                                               heuristic,
                                               horizon,
                                               max_trials,
                                               labeling,
                                               random_ties);
}

}; // namespace Policy

}; // namespace Online

#undef DEBUG

#endif

