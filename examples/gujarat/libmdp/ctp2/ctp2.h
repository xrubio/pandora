#include <cassert>
#include <cstring>
#include <iostream>
#include <map>
#include <vector>
#include <limits.h>

#include "graph.h"
#include "algorithm.h"
#include "parameters.h"
#include "heuristic.h"

#include "policy.h"
#include "rollout.h"
#include "dispatcher.h"

#define DISCOUNT 1.00

#define WORDS_FOR_NODES 1 // max. 32 nodes
#define WORDS_FOR_EDGES 1 // max. 32 edges

template<typename T> struct bits_t {
    T field_;
    int base_;
    bits_t(const T field, int base) : field_(field), base_(base) { }
    void print(std::ostream &os) const {
        if( field_ != 0 ) {
            T aux = field_;
            for( int i = 0; aux != 0; ++i, aux = aux >> 1 ) {
                if( aux & 0x1 ) {
                    os << base_ + i << ",";
                }
            }
        }
    }
};

template<typename T> inline std::ostream& operator<<(std::ostream &os, const bits_t<T> &bits) {
    bits.print(os);
    return os;
}

struct state_t {
    int current_;
    unsigned long known_;
    unsigned long blocked_;
    unsigned long visited_;
    std::vector<int> distances_;
    int heuristic_;


  public:
    state_t(int current = -1)
      : current_(current),
        known_(0), blocked_(0), visited_(0),
        heuristic_(0) { }
    state_t(const state_t &s) { *this = s; }
    ~state_t() { }

    static void initialize(const CTP::graph_t &graph) {
        if( graph.num_nodes_ > (int)(WORDS_FOR_NODES * sizeof(unsigned) * 8) ) {
            std::cout << "error: number of nodes must be <= "
                      <<  WORDS_FOR_NODES * sizeof(unsigned) * 8
                      << std::endl;
            exit(1);
        }

        if( graph.num_edges_ > (int)(WORDS_FOR_EDGES * sizeof(unsigned) * 8) ) {
            std::cout << "error: number of nodes must be <= "
                      <<  WORDS_FOR_EDGES * sizeof(unsigned) * 8
                      << std::endl;
            exit(1);
        }
    }


    size_t hash() const { return current_ + (known_ ^ blocked_); }

    bool known(int edge) const {
        int mask = 1 << edge;
        return known_ & mask;
    }
    bool traversable(int edge) const {
        int mask = 1 << edge;
        return (blocked_ & mask) != 0 ? false : true;
    }
    bool visited(int node) const {
        int mask = 1 << node;
        return visited_ & mask;
    }
    bool reachable(int node) const {
        return distances_[node] < INT_MAX;
    }
    bool perimeter(int node, const CTP::graph_t &graph) const {
        return !visited(node) && reachable(node);
    }
    int distance_to(int node) const {
        return distances_[node];
    }
    bool is_dead_end() const {
        return (current_ != -1) && (heuristic_ == INT_MAX);
    }

    void set_edge_status(int edge, bool blocked) {
        int mask = 1 << edge;
        known_ |= mask;
        if( blocked )
            blocked_ |= mask;
        else
            blocked_ &= ~mask;
    }
    void move_to(int node) {
        int mask = 1 << node;
        current_ = node;
        visited_ |= mask;
    }

    void preprocess(const CTP::graph_t &graph) {
        dijkstra(graph, current_, distances_);
        //dijkstra(graph, current_, static_distances_, true);
        //heuristic_ = static_distances_[graph.num_nodes_ - 1];
        heuristic_ = bfs(graph, current_, graph.num_nodes_ - 1, true);
    }

    void clear() {
        current_ = 0;
        known_ = 0;
        blocked_ = 0;
        visited_ = 0;
        distances_.clear();
        heuristic_ = 0;
    }

    const state_t& operator=(const state_t &s) {
        current_ = s.current_;
        known_ = s.known_;
        blocked_ = s.blocked_;
        visited_ = s.visited_;
        distances_ = s.distances_;
        heuristic_ = s.heuristic_;
        return *this;
    }
    bool operator==(const state_t &s) const {
        return (current_ == s.current_) &&
               (known_ == s.known_) &&
               (blocked_ == s.blocked_) &&
               (visited_ == s.visited_);
    }
    bool operator!=(const state_t &s) const {
        return *this == s ? false : true;
    }
    bool operator<(const state_t &s) const {
        return (current_ < s.current_) ||
               ((current_ == s.current_) && (known_ < s.known_)) ||
               ((current_ == s.current_) && (known_ == s.known_) && (blocked_ < s.blocked_)) ||
               ((current_ == s.current_) && (known_ == s.known_) && (blocked_ == s.blocked_) && (visited_ < s.visited_));
    }
    void print(std::ostream &os) const {
        os << "(" << current_
           << ",K={" << bits_t<unsigned>(known_, 0) << "}"
           << ",B={" << bits_t<unsigned>(blocked_, 0) << "}"
           << ",V={" << bits_t<unsigned>(visited_, 0) << "}";
        if( is_dead_end() ) os << ",DEAD";
        os << ")";
    }

    int bfs(const CTP::graph_t &graph,
            int start,
            int goal,
            bool optimistic = false) const {
        return graph.bfs(start, goal, known_, blocked_, optimistic);
    }
    void dijkstra(const CTP::graph_t &graph,
                  int source,
                  std::vector<int> &distances,
                  bool optimistic = false) const {
        graph.dijkstra(source, distances, known_, blocked_, optimistic);
    }
};


inline std::ostream& operator<<(std::ostream &os, const state_t &s) {
    s.print(os);
    return os;
}


class problem_t : public Problem::problem_t<state_t> {
  public:
    const CTP::graph_t &graph_;
    const state_t init_;
    int start_, goal_;

  public:
    problem_t(CTP::graph_t &graph)
      : Problem::problem_t<state_t>(DISCOUNT, 1e3), // change dead_end_value
        graph_(graph), init_(-1), start_(0), goal_(graph_.num_nodes_ - 1) { }
    virtual ~problem_t() { }

    virtual Problem::action_t number_actions(const state_t &s) const {
        return s.current_ == -1 ? 1 : graph_.num_nodes_;
    }
    virtual bool applicable(const state_t &s, Problem::action_t a) const {
        return ((s.current_ == -1) && (a == 0)) ||
               ((s.current_ != -1) && s.perimeter(a, graph_));
    }
    virtual const state_t& init() const { return init_; }
    virtual bool terminal(const state_t &s) const {
        return s.current_ == goal_;
    }
    virtual bool dead_end(const state_t &s) const {
        return s.is_dead_end();
    }
    virtual float cost(const state_t &s, Problem::action_t a) const {
        return s.current_ == -1 ? 0 : s.distance_to(a);
    }
    virtual void next(const state_t &s,
                      Problem::action_t a,
                      std::vector<std::pair<state_t, float> > &outcomes) const {
        //std::cout << "next" << s << " w/ a=" << a << " is:" << std::endl;

        ++expansions_;
        outcomes.clear();

        int to_node = -1;
        if( s.current_ == -1 ) {
            to_node = start_;
        } else {
            to_node = a;
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
                next.set_edge_status(e, subset & 1);
                subset = subset >> 1;
            }
            next.move_to(to_node);
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

    virtual void next(const state_t &s,
                      Problem::action_t a,
                      std::vector<std::pair<state_t, float> > &outcomes) const {
        ++expansions_;
        outcomes.clear();
        outcomes.reserve(1);

        int to_node = -1;
        if( s.current_ == -1 ) {
            to_node = start_;
        } else {
            to_node = a;
            assert(to_node != s.current_);
        }

        // set unique outcome using hidden state
        state_t next(s);
        for( int i = 0, isz = graph_.at_[to_node].size(); i < isz; ++i ) {
            int e = graph_.at_[to_node][i];
            next.set_edge_status(e, hidden_.traversable(e) ? false : true);
        }
        next.move_to(to_node);
        next.preprocess(graph_);
        outcomes.push_back(std::make_pair(next, 1));
    }
};


class min_min_t : public Heuristic::heuristic_t<state_t> {
  public:
    min_min_t() { }
    virtual ~min_min_t() { }
    virtual float value(const state_t &s) const { return (float)s.heuristic_; }
    virtual void reset_stats() const { }
    virtual float setup_time() const { return 0; }
    virtual float eval_time() const { return 0; }
    virtual size_t size() const { return 0; }
    virtual void dump(std::ostream &os) const { }
    float operator()(const state_t &s) const { return value(s); }
};


inline void sample_weather(const CTP::graph_t &graph, state_t &state) {
    state.clear();
    int num_edges = graph.with_shortcut_ ? graph.num_edges_ - 1 : graph.num_edges_;
    for( int e = 0; e < num_edges; ++e ) {
        float p = graph.prob(e);
        if( Random::real() < p ) {
            state.set_edge_status(e, false);
        } else {
            state.set_edge_status(e, true);
        }
    }
    if( graph.with_shortcut_ ) state.set_edge_status(graph.num_edges_ - 1, true);
}

inline float probability_bad_weather(const CTP::graph_t &graph,
                                     unsigned nsamples) {
    float prob = 0;
    state_t weather(0);
    for( unsigned i = 0; i < nsamples; ++i ) {
        sample_weather(graph, weather);
        weather.preprocess(graph);
        prob += weather.reachable(graph.num_nodes_ - 1) ? 0 : 1;
    }
    return prob / nsamples;
}


