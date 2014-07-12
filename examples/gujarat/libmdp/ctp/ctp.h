#include <cassert>
#include <iostream>
#include <map>
#include <vector>
#include <limits>

//#define  DISCOUNT  .95
#define  DISCOUNT  1

#include "parsing.h"
#include "algorithm.h"
#include "parameters.h"
#include "heuristic.h"

#include "policy.h"
#include "rollout.h"
#include "mcts.h"
#include "dispatcher.h"

struct state_t {
    int current_;
    unsigned long known_;
    unsigned long blocked_;
    bool is_dead_end_;

  public:
    state_t(int current = -1) : current_(current), known_(0), blocked_(0), is_dead_end_(false) { }
    state_t(const state_t &s) : current_(s.current_), known_(s.known_), blocked_(s.blocked_), is_dead_end_(s.is_dead_end_) { }
    ~state_t() { }

    size_t hash() const { return current_ + (known_ ^ blocked_); }

    bool known(int e) const {
        int mask = 1 << e;
        return known_ & mask;
    }
    bool traversable(int e) const {
        int mask = 1 << e;
        return (blocked_ & mask) != 0 ? false : true;
    }
    void set(int e, int blocked) {
        int mask = 1 << e;
        known_ |= mask;
        if( blocked == 1 )
            blocked_ |= mask;
        else
            blocked_ &= ~mask;
    }
    void preprocess(const CTP::graph_t &graph) {
        std::vector<int> distances;
        compute_distances(graph, distances, true);
        is_dead_end_ = distances[current_] == std::numeric_limits<int>::max();
    }

    const state_t& operator=(const state_t &s) {
        current_ = s.current_;
        known_ = s.known_;
        blocked_ = s.blocked_;
        is_dead_end_ = s.is_dead_end_;
        return *this;
    }
    bool operator==(const state_t &s) const {
        return (current_ == s.current_) && (known_ == s.known_) && (blocked_ == s.blocked_);
    }
    bool operator!=(const state_t &s) const {
        return (current_ != s.current_) || (known_ != s.known_) || (blocked_ != s.blocked_);
    }
    bool operator<(const state_t &s) const {
        return (current_ < s.current_) ||
               ((current_ == s.current_) && (known_ < s.known_)) ||
               ((current_ == s.current_) && (known_ == s.known_) && (blocked_ < s.blocked_));
    }
    void print(std::ostream &os) const {
        os << "("
           << current_
           << "," << known_
           << "," << blocked_;
        if( is_dead_end_ ) os << ",DE";
        os << ")";
    }

    void compute_distances(const CTP::graph_t &graph, std::vector<int> &dist, bool optimistic = false) const {
        dist.clear();
        dist.reserve(graph.num_nodes_);

        // compute all shortest-paths from current node in known (or optimistic) graph.
        std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int> >, CTP::open_list_cmp> queue;

        // initialization of values
        for( int n = 0; n < graph.num_nodes_; ++n ) {
            dist.push_back(std::numeric_limits<int>::max());
        }

        // Dijsktra's with goal node as seed
        int goal = graph.num_nodes_ - 1;
        dist[goal] = 0;
        queue.push(std::make_pair(goal, 0));
        while( !queue.empty() ) {
            std::pair<int, int> p = queue.top();
            queue.pop();
            if( p.second <= dist[p.first] ) {
                for( int i = 0, isz = graph.at_[p.first].size(); i < isz; ++i ) {
                    int j = graph.at_[p.first][i];
                    if( (known(j) && traversable(j)) || (optimistic && !known(j)) ) {
                        const CTP::graph_t::edge_t &e = graph.edge_list_[j];
                        int cost = p.second + e.cost_;
                        int next = p.first == e.to_ ? e.from_ : e.to_;
                        if( cost < dist[next] ) {
                            dist[next] = cost;
                            queue.push(std::make_pair(next, cost));
                        }
                    }
                }
            }
        }
    }

    void print_path(const CTP::graph_t &graph, const std::vector<int> &dist) const {
        int path_cost = 0;
        std::cout << "path=<" << std::flush;
        if( dist[current_] < std::numeric_limits<int>::max() ) {
            int node = current_;
            std::cout << "n=" << node << std::flush;
            while( node != graph.num_nodes_ - 1 ) {
                int best = -1, cost = std::numeric_limits<int>::max();
                for( int i = 0, isz = graph.at_[node].size(); i < isz; ++i ) {
                    int j = graph.at_[node][i];
                    if( known(j) && traversable(j) ) {
                        const CTP::graph_t::edge_t &e = graph.edge_list_[j];
                        int next = node == e.to_ ? e.from_ : e.to_;
                        if( dist[next] < cost ) {
                            best = j;
                            cost = dist[next];
                        }
                    } else {
                        //std::cout << "edge " << j << " is unknown or non-traversable" << std::endl;
                    }
                }
                const CTP::graph_t::edge_t &e = graph.edge_list_[best];
                int next = node == e.to_ ? e.from_ : e.to_;
                node = next;
                path_cost += e.cost_;
                std::cout << ",e=" << best << std::flush;
                std::cout << ",n=" << node << std::flush;
            }
            std::cout << ">, cost=" << path_cost << std::endl;
        }
    }

};

inline std::ostream& operator<<(std::ostream &os, const state_t &s) {
    s.print(os);
    return os;
}

bool cmp_function(const std::pair<state_t, float> &p1, const std::pair<state_t, float> &p2) {
    return p1.second > p2.second;
}

class problem_t : public Problem::problem_t<state_t> {
  public:
    const CTP::graph_t &graph_;
    const state_t init_;
    int start_, goal_;

  public:
    problem_t(CTP::graph_t &graph) : graph_(graph), init_(-1), start_(0), goal_(graph_.num_nodes_ - 1) { }
    virtual ~problem_t() { }

    virtual Problem::action_t number_actions(const state_t &s) const {
        return s.current_ == -1 ? 1 : graph_.at_[s.current_].size();
    }
    virtual bool applicable(const state_t &s, Problem::action_t a) const {
        return ((s.current_ == -1) && (a == 0)) ||
               ((s.current_ != -1) && s.traversable(graph_.at_[s.current_][a]));
    }
    virtual const state_t& init() const { return init_; }
    virtual bool terminal(const state_t &s) const { return s.current_ == goal_; }
    virtual bool dead_end(const state_t &s) const { return s.is_dead_end_; }
    virtual float cost(const state_t &s, Problem::action_t a) const {
        return s.current_ == -1 ? 0 : graph_.cost(graph_.at_[s.current_][a]);
    }
    virtual void next(const state_t &s, Problem::action_t a, std::vector<std::pair<state_t, float> > &outcomes) const {
        //std::cout << "next" << s << " w/ a=" << a << " is:" << std::endl;

        ++expansions_;
        outcomes.clear();

        int to_node = -1;
        if( s.current_ == -1 ) {
            to_node = start_;
        } else {
            // determine node at the other side of the edge (action)
            int e = graph_.at_[s.current_][a];
            to_node = graph_.to(e) == s.current_ ? graph_.from(e) : graph_.to(e);
            assert(to_node != s.current_);
        }

        // collect edges adjacent at to_node of unknown status
        int k = 0;
        std::vector<int> unknown_edges;
        unknown_edges.reserve(graph_.num_edges_);
        for( int i = 0, isz = graph_.at_[to_node].size(); i < isz; ++i ) {
            int e = graph_.at_[to_node][i];
            if( !s.known(e) ) {
                unknown_edges.push_back(e);
                ++k;
            }
        }

        // generate subsets of unknowns edges and update weathers
        outcomes.reserve(1 << k);
        for( int i = 0, isz = 1 << k; i < isz; ++i ) {
            state_t next(s);
            float p = 1;
            int subset = i;
            for( int j = 0; j < k; ++j ) {
                int e = unknown_edges[j];
                p *= (subset & 1) ? 1 - graph_.prob(e) : graph_.prob(e);
                next.set(e, subset & 1);
                subset = subset >> 1;
            }
            next.current_ = to_node;
            if( p > 0 ) {
                next.preprocess(graph_);
                outcomes.push_back(std::make_pair(next, p));
                //std::cout << "    " << next << " w.p. " << p << std::endl;
            }
        }
    }
    virtual void print(std::ostream &os) const { }
};

inline std::ostream& operator<<(std::ostream &os, const problem_t &p) {
    p.print(os);
    return os;
}

class problem_with_hidden_state_t : public problem_t {
    mutable state_t hidden_;

  public:
    problem_with_hidden_state_t(CTP::graph_t &graph) : problem_t(graph) { }
    virtual ~problem_with_hidden_state_t() { }

    void set_hidden(state_t &hidden) const {
        hidden_ = hidden;
    }

    virtual void next(const state_t &s, Problem::action_t a, std::vector<std::pair<state_t, float> > &outcomes) const {
        ++expansions_;
        outcomes.clear();
        outcomes.reserve(1);

        int to_node = -1;
        if( s.current_ == -1 ) {
            to_node = start_;
        } else {
            // determine node at the other side of the edge (action)
            int e = graph_.at_[s.current_][a];
            to_node = graph_.to(e) == s.current_ ? graph_.from(e) : graph_.to(e);
            assert(to_node != s.current_);
        }

        // set unique outcome using hidden state
        state_t next(s);
        for( int i = 0, isz = graph_.at_[to_node].size(); i < isz; ++i ) {
            int e = graph_.at_[to_node][i];
            next.set(e, hidden_.traversable(e) ? 0 : 1);
        }
        next.current_ = to_node;
        outcomes.push_back(std::make_pair(next, 1));
    }
};

inline state_t sample_weather(const CTP::graph_t &graph) {
    state_t state(0);
    int num_edges = graph.with_shortcut_ ? graph.num_edges_ - 1 : graph.num_edges_;
    for( int e = 0; e < num_edges; ++e ) {
        float p = graph.prob(e);
        if( Random::real() < p ) {
            state.set(e, 0);
        } else {
            state.set(e, 1);
        }
    }
    if( graph.with_shortcut_ ) state.set(graph.num_edges_ - 1, 1);
    return state;
}

inline float probability_bad_weather(const CTP::graph_t &graph, unsigned nsamples) {
    std::vector<int> distances;
    float prob = 0;
    for( unsigned i = 0; i < nsamples; ++i ) {
        state_t weather = sample_weather(graph);
        weather.compute_distances(graph, distances);
        prob += distances[0] < std::numeric_limits<int>::max() ? 0 : 1;
    }
    return prob / nsamples;
}

