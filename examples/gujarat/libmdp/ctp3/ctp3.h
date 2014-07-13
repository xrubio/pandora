#include <cassert>
#include <cstring>
#include <iostream>
#include <map>
#include <vector>
#include <limits.h>
#include <tr1/unordered_map>

#include "graph.h"
#include "algorithm.h"
#include "parameters.h"
#include "heuristic.h"

#include "policy.h"
#include "rollout.h"
#include "dispatcher.h"

#define DISCOUNT 1.00

#define WORDS_FOR_NODES 4 // max. 128 nodes
#define WORDS_FOR_EDGES 9 // max. 288 edges


inline unsigned rotation(unsigned x) {
    return (x << 16) | (x >> 16);
}


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



struct state_info_t {
    unsigned known_[WORDS_FOR_EDGES];
    unsigned blocked_[WORDS_FOR_EDGES];
    static int words_for_edges_;

    state_info_t() {
        memset(known_, 0, WORDS_FOR_EDGES * sizeof(unsigned));
        memset(blocked_, 0, WORDS_FOR_EDGES * sizeof(unsigned));
    }
    ~state_info_t() { }

    static void initialize(int words_for_edges) {
        words_for_edges_ = words_for_edges;
    }

    size_t hash() const {
        unsigned rv = 0;
        for( int i = 0; i < words_for_edges_; ++i )
            rv = rv ^ known_[i];
        for( int i = 0; i < words_for_edges_; ++i )
            rv = rv ^ rotation(blocked_[i]);
        return rv;
    }

    void clear() {
        memset(known_, 0, words_for_edges_ * sizeof(unsigned));
        memset(blocked_, 0, words_for_edges_ * sizeof(unsigned));
    }

    bool known(int edge) const {
        int n = edge >> 5, offset = edge & 0x1F;
        int mask = 1 << offset;
        return known_[n] & mask;
    }
    bool traversable(int edge) const {
        int n = edge >> 5, offset = edge & 0x1F;
        int mask = 1 << offset;
        return (blocked_[n] & mask) != 0 ? false : true;
    }
    void set_edge_status(int edge, bool blocked) {
        int n = edge >> 5, offset = edge & 0x1F;
        int mask = 1 << offset;
        known_[n] |= mask;
        if( blocked )
            blocked_[n] |= mask;
        else
            blocked_[n] &= ~mask;
    }

    const state_info_t& operator=(const state_info_t &info) {
        memcpy(known_, info.known_, words_for_edges_ * sizeof(unsigned));
        memcpy(blocked_, info.blocked_, words_for_edges_ * sizeof(unsigned));
        return *this;
    }

    bool operator==(const state_info_t &info) const {
        return (memcmp(known_, info.known_, words_for_edges_ * sizeof(unsigned)) == 0) &&
               (memcmp(blocked_, info.blocked_, words_for_edges_ * sizeof(unsigned)) == 0);
    }
    bool operator!=(const state_info_t &info) const {
        return *this == info ? false : true;
    }
    bool operator<(const state_info_t &info) const {
        return (memcmp(known_, info.known_, words_for_edges_ * sizeof(unsigned)) < 0) ||
               ((memcmp(known_, info.known_, words_for_edges_ * sizeof(unsigned)) == 0) &&
                (memcmp(blocked_, info.blocked_, words_for_edges_ * sizeof(unsigned)) < 0));
    }

    void print(std::ostream &os) const {
        os << "K={";
        for( int i = 0; i < words_for_edges_; ++i )
            os << bits_t<unsigned>(known_[i], i << 5);
        os << "},B={";
        for( int i = 0; i < words_for_edges_; ++i )
            os << bits_t<unsigned>(blocked_[i], i << 5);
        os << "}";
    }
};

int state_info_t::words_for_edges_ = 0;

inline std::ostream& operator<<(std::ostream &os, const state_info_t &info) {
    info.print(os);
    return os;
}


struct cache_functions_t {
    bool operator()(const state_info_t &info1, const state_info_t &info2) const {
        return info1 == info2;
    }
    size_t operator()(const state_info_t &info) const {
        return info.hash();
    }
};

class shortest_path_cache_item_t
  : public std::tr1::unordered_map<state_info_t,
                                   int*,
                                   cache_functions_t,
                                   cache_functions_t> {
  public:
    void clear() {
        for( iterator it = begin(); it != end(); ++it )
            delete[] it->second;
        std::tr1::unordered_map<state_info_t, int*, cache_functions_t, cache_functions_t>::clear();
    }

    void print_stats(std::ostream &os) const {
        int maxsz = 0;
        for( int i = 0; i < (int)bucket_count(); ++i ) {
            int sz = bucket_size(i);
            maxsz = sz > maxsz ? sz : maxsz;
        }
        os << "#entries=" << size()
           << ", #buckets=" << bucket_count()
           << ", max-bucket-size=" << maxsz;
    }
};

class shortest_path_cache_t {
    int num_nodes_;
    int *distances_;
    shortest_path_cache_item_t **items_;
    unsigned capacity_;
    unsigned size_;
    unsigned lookups_;
    unsigned hits_;

  public:
    shortest_path_cache_t()
      : num_nodes_(0), items_(0),
        capacity_(0), size_(0),
        lookups_(0), hits_(0) { }
    ~shortest_path_cache_t() {
        for( int i = 0; i < num_nodes_; ++i )
            delete items_[i];
        delete[] items_;
    }

    void initialize(int num_nodes, unsigned capacity) {
        num_nodes_ = num_nodes;
        capacity_ = capacity;
        distances_ = new int[num_nodes_];
        items_ = new shortest_path_cache_item_t*[num_nodes_];
        for( int i = 0; i < num_nodes_; ++i )
            items_[i] = new shortest_path_cache_item_t;
    }

    void clear() {
        for( int i = 0; i < num_nodes_; ++i )
            items_[i]->clear();
    }

    std::pair<bool, const int*> lookup(const CTP::graph_t &graph, int source, const state_info_t &info) {
        ++lookups_;
        shortest_path_cache_item_t *item = items_[source];
        shortest_path_cache_item_t::const_iterator it = item->find(info);
        if( it != item->end() ) {
            ++hits_;
            return std::make_pair(true, it->second);
        } else {
            if( size_ < capacity_ ) {
                ++size_;
                int *distances = new int[num_nodes_];
                graph.dijkstra(source, distances, info.known_, info.blocked_, false);
                distances[source] = graph.bfs(source, num_nodes_ - 1, info.known_, info.blocked_, true);
                item->insert(std::make_pair(info, distances));
                return std::make_pair(true, distances);
            } else {
                graph.dijkstra(source, distances_, info.known_, info.blocked_, false);
                distances_[source] = graph.bfs(source, num_nodes_ - 1, info.known_, info.blocked_, true);
                return std::make_pair(false, distances_);
            }
        }
    }

    unsigned lookups() const { return lookups_; }
    float hit_ratio() const { return (float)hits_ / (float)lookups_; }
    void print_stats(std::ostream &os) {
        if( capacity_ > 0 ) {
            os << "cache: #entries=" << size_
               << ", #lookups=" << lookups()
               << ", %hit=" << hit_ratio()
               << std::endl;
            for( int i = 0; i < num_nodes_; ++i ) {
                os << "  item[" << i << "]: ";
                items_[i]->print_stats(os);
                os << std::endl;
            }
        }
    }
};

struct state_t {
    int current_;
    state_info_t info_;
    unsigned visited_[WORDS_FOR_NODES];
    mutable bool shared_;
    //mutable const int *distances_;
    mutable int *distances_;
    mutable int heuristic_;

    static const CTP::graph_t *graph_;
    static int num_nodes_;
    static int num_edges_;
    static int words_for_nodes_;
    static int words_for_edges_;
    static shortest_path_cache_t cache_;
    static bool use_cache_;

  public:
    state_t(int current = -1)
      : current_(current), shared_(true), distances_(0), heuristic_(-1) {
        memset(visited_, 0, WORDS_FOR_NODES * sizeof(unsigned));
    }
    state_t(const state_t &s)
      : shared_(true), distances_(0) {
        *this = s;
    }
    ~state_t() {
        if( !shared_ ) delete[] distances_;
    }

    static void initialize(const CTP::graph_t &graph, bool use_cache, int cache_capacity) {
        graph_ = &graph;
        num_nodes_ = graph.num_nodes_;
        num_edges_ = graph.num_edges_;

        if( num_nodes_ > (int)(WORDS_FOR_NODES * sizeof(unsigned) * 8) ) {
            std::cout << "error: number of nodes must be <= "
                      <<  WORDS_FOR_NODES * sizeof(unsigned) * 8
                      << std::endl;
            exit(1);
        }

        if( num_edges_ > (int)(WORDS_FOR_EDGES * sizeof(unsigned) * 8) ) {
            std::cout << "error: number of nodes must be <= "
                      <<  WORDS_FOR_EDGES * sizeof(unsigned) * 8
                      << std::endl;
            exit(1);
        }

        words_for_nodes_ = num_nodes_ >> 5;
        if( (num_nodes_ & 0x1F) != 0 ) ++words_for_nodes_;
        words_for_edges_ = num_edges_ >> 5;
        if( (num_edges_ & 0x1F) != 0 ) ++words_for_edges_;
        std::cout << "init: #nodes=" << num_nodes_
                  << ", #edges=" << num_edges_
                  << ", #words-for-nodes=" << words_for_nodes_
                  << ", #words-for-edges=" << words_for_edges_
                  << std::endl;
        state_info_t::initialize(words_for_edges_);
        use_cache_ = use_cache;
        cache_.initialize(num_nodes_, cache_capacity);
    }

    static void clear_cache() {
        cache_.clear();
    }

    static void print_stats(std::ostream &os) {
        cache_.print_stats(os);
    }

    size_t hash() const { return info_.hash(); }

    void compute_heuristic() const {
        if( heuristic_ == -1 ) {
            heuristic_ = graph_->bfs(current_ == -1 ? 0 : current_, num_nodes_ - 1,
                                     info_.known_, info_.blocked_, true);
        }
    }

    bool known(int edge) const { return info_.known(edge); }
    bool traversable(int edge) const { return info_.traversable(edge); }
    bool visited(int node) const {
        int n = node >> 5, offset = node & 0x1F;
        int mask = 1 << offset;
        return visited_[n] & mask;
    }
    bool reachable(int node) const {
        preprocess();
        return distances_[node] < INT_MAX;
    }
    int distance_to(int node) const {
        preprocess();
        return distances_[node];
    }
    bool perimeter(int node) const {
        return !visited(node) && reachable(node);
    }
    bool is_dead_end() const {
        compute_heuristic();
        return (current_ != -1) && (heuristic_ == INT_MAX);
    }

    void set_edge_status(int edge, bool blocked) {
        info_.set_edge_status(edge, blocked);
    }
    void move_to(int node) {
        int n = node >> 5, offset = node & 0x1F;
        int mask = 1 << offset;
        current_ = node;
        visited_[n] |= mask;
    }

    void preprocess() const {
        if( distances_ == 0 ) {
            assert(shared_);
            std::pair<bool, const int*> p(false, 0);
            if( use_cache_ ) p = cache_.lookup(*graph_, current_, info_);
            if( p.first ) {
                if( !shared_ ) delete[] distances_;
                distances_ = const_cast<int*>(p.second);
                shared_ = true;
            } else {
                if( shared_ ) distances_ = new int[num_nodes_];
                assert(distances_ != 0);
                shared_ = false;
                if( p.second ) {
                    memcpy(const_cast<int*>(distances_), p.second, num_nodes_ * sizeof(int));
                } else {
                    graph_->dijkstra(current_, distances_, info_.known_, info_.blocked_, false);
                    //distances_[current_] = graph_->bfs(current_, num_nodes_ - 1, info_.known_, info_.blocked_, true);
                }
            }
            //heuristic_ = distances_[current_];
        }
    }

    void clear() {
        current_ = 0;
        info_.clear();
        memset(visited_, 0, words_for_nodes_ * sizeof(unsigned));
        if( !shared_ ) {
            delete[] distances_;
            shared_ = true;
        }
        distances_ = 0;
        heuristic_ = -1;
    }

    const state_t& operator=(const state_t &s) {
        current_ = s.current_;
        info_ = s.info_;
        memcpy(visited_, s.visited_, words_for_nodes_ * sizeof(unsigned));
        if( s.shared_ ) {
            if( !shared_ ) delete[] distances_;
            distances_ = s.distances_;
            shared_ = true;
        } else {
            if( shared_ ) {
                distances_ = new int[num_nodes_];
                shared_ = false;
            }
            assert(distances_ != 0);
            memcpy(const_cast<int*>(distances_), s.distances_, num_nodes_ * sizeof(int));
        }
        heuristic_ = s.heuristic_;
        return *this;
    }
    bool operator==(const state_t &s) const {
        return (current_ == s.current_) &&
               (info_ == s.info_) &&
               (memcmp(visited_, s.visited_, words_for_nodes_ * sizeof(unsigned)) == 0);
    }
    bool operator!=(const state_t &s) const {
        return *this == s ? false : true;
    }
    bool operator<(const state_t &s) const {
        return (current_ < s.current_) ||
               ((current_ == s.current_) && (info_ < s.info_)) ||
               ((current_ == s.current_) && (info_ == s.info_) &&
                (memcmp(visited_, s.visited_, words_for_nodes_ * sizeof(unsigned)) < 0));
    }
    void print(std::ostream &os) const {
        os << "(" << current_ << "," << info_ << ",V={";
        for( int i = 0; i < words_for_nodes_; ++i )
            os << bits_t<unsigned>(visited_[i], i << 5);
        os << "}";
        if( is_dead_end() ) os << ",DEAD";
        os << ")";
    }
};

const CTP::graph_t *state_t::graph_ = 0;
int state_t::num_nodes_ = 0;
int state_t::num_edges_ = 0;
int state_t::words_for_nodes_ = 0;
int state_t::words_for_edges_ = 0;
shortest_path_cache_t state_t::cache_;
bool state_t::use_cache_ = false;

inline std::ostream& operator<<(std::ostream &os, const state_t &s) {
    s.print(os);
    return os;
}


struct next_cache_functions_t {
    bool operator()(const state_t &s1, const state_t &s2) const {
        return s1 == s2;
    }
    size_t operator()(const state_t &s) const {
        return s.hash();
    }
};

class next_cache_item_t
  : public std::tr1::unordered_map<state_t,
                                   std::vector<std::pair<state_t, float> >*,
                                   next_cache_functions_t,
                                   next_cache_functions_t> {
  public:
    void print_stats(std::ostream &os) const {
        int maxsz = 0;
        for( int i = 0; i < (int)bucket_count(); ++i ) {
            int sz = bucket_size(i);
            maxsz = sz > maxsz ? sz : maxsz;
        }
        os << "#entries=" << size()
           << ", #buckets=" << bucket_count()
           << ", max-bucket-size=" << maxsz;
    }
};

class next_cache_t {
    int num_actions_;
    next_cache_item_t **items_;
    unsigned capacity_;
    unsigned size_;
    mutable unsigned lookups_;
    mutable unsigned hits_;

  public:
    next_cache_t()
      : num_actions_(0), items_(0),
        capacity_(0), size_(0),
        lookups_(0), hits_(0) { }
    ~next_cache_t() {
        for( int i = 0; i < num_actions_; ++i )
            delete items_[i];
        delete[] items_;
    }

    void initialize(int num_actions, unsigned capacity) {
        num_actions_ = num_actions;
        capacity_ = capacity;
        items_ = new next_cache_item_t*[num_actions_];
        for( int i = 0; i < num_actions_; ++i )
            items_[i] = new next_cache_item_t;
    }

    const std::vector<std::pair<state_t, float> >* lookup(const state_t &state,
                                                          Problem::action_t a) const {
        ++lookups_;
        next_cache_item_t *item = items_[a];
        next_cache_item_t::const_iterator it = item->find(state);
        if( it != item->end() ) {
            ++hits_;
            return it->second;
        } else
            return 0;
    }

    void insert(const state_t &state,
                Problem::action_t a,
                std::vector<std::pair<state_t, float> > &next) {
        if( size_ < capacity_ ) {
            ++size_;
            next_cache_item_t *item = items_[a];
            std::vector<std::pair<state_t, float> > *ptr
              = new std::vector<std::pair<state_t, float> >(next);
            item->insert(std::make_pair(state, ptr));
        }
    }

    unsigned lookups() const { return lookups_; }
    float hit_ratio() const { return (float)hits_ / (float)lookups_; }
    void print_stats(std::ostream &os) {
        if( capacity_ > 0 ) {
            os << "next_cache: #entries=" << size_
               << ", #lookups=" << lookups()
               << ", %hit=" << hit_ratio()
               << std::endl;
            for( int i = 0; i < num_actions_; ++i ) {
                os << "  item[" << i << "]: ";
                items_[i]->print_stats(os);
                os << std::endl;
            }
        }
    }
};


class problem_t : public Problem::problem_t<state_t> {
  public:
    const CTP::graph_t &graph_;
    const state_t init_;
    int start_, goal_;
    mutable int max_branching_;
    mutable float avg_branching_;
    bool use_cache_;
    mutable next_cache_t next_cache_;

  public:
    problem_t(CTP::graph_t &graph, float dead_end_value = 1e3, bool use_cache = false, unsigned cache_size = (int)1e4)
      : Problem::problem_t<state_t>(DISCOUNT, dead_end_value),
        graph_(graph), init_(-1), start_(0), goal_(graph_.num_nodes_ - 1),
        max_branching_(0), avg_branching_(0), use_cache_(use_cache) {
        next_cache_.initialize(graph.num_nodes_, cache_size);
    }
    virtual ~problem_t() { }

    virtual Problem::action_t number_actions(const state_t &s) const {
        return s.current_ == -1 ? 1 : graph_.num_nodes_;
    }
    virtual bool applicable(const state_t &s, Problem::action_t a) const {
        return ((s.current_ == -1) && (a == 0)) ||
               ((s.current_ != -1) && s.perimeter(a));
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

        if( use_cache_ ) {
            const std::vector<std::pair<state_t, float> > *ptr = next_cache_.lookup(s, a);
            if( ptr != 0 ) {
                outcomes = *ptr;
                return;
            }
        }

        //std::cout << "next" << s << " w/ a=" << a << " is:" << std::endl;

        ++expansions_;

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
        max_branching_ = (1<<k) > max_branching_ ? (1<<k) : max_branching_;
        avg_branching_ = (expansions_ - 1) * avg_branching_ + (float)(1<<k);
        avg_branching_ /= (float)expansions_;

        // generate subsets of unknowns edges and update weathers
        outcomes.clear();
        outcomes.reserve(1 << k);
        for( int i = 0, isz = 1 << k; i < isz; ++i ) {
            state_t next(s);
            float p = 1;
            int subset = i;
            for( int j = 0; j < k; ++j ) {
                int e = unknown_edges[j];
                p *= (subset & 1) ? 1 - graph_.prob(e) : graph_.prob(e);
                next.info_.set_edge_status(e, subset & 1);
                subset = subset >> 1;
            }
            next.move_to(to_node);
            next.heuristic_ = -1;
            if( next.shared_ ) {
                next.distances_ = 0;
            } else {
                assert(next.distances_ != 0);
                delete[] next.distances_;
                next.distances_ = 0;
                next.shared_ = true;
            }
            assert(next.distances_ == 0);
            if( p > 0 ) {
                //next.preprocess();
                outcomes.push_back(std::make_pair(next, p));
                //std::cout << "    " << next << " w.p. " << p << std::endl;
            }
        }

        if( use_cache_ ) next_cache_.insert(s, a, outcomes);
    }
    virtual void print(std::ostream &os) const { }

    void print_stats(std::ostream &os) {
        if( use_cache_ ) next_cache_.print_stats(os);
    }
};

inline std::ostream& operator<<(std::ostream &os, const problem_t &p) {
    p.print(os);
    return os;
}


class problem_with_hidden_state_t : public problem_t {
    mutable state_t hidden_;

  public:
    problem_with_hidden_state_t(CTP::graph_t &graph, float dead_end_value)
      : problem_t(graph, dead_end_value) { }
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
            next.info_.set_edge_status(e, hidden_.traversable(e) ? false : true);
        }
        next.move_to(to_node);
        next.heuristic_ = -1;
        if( next.shared_ ) {
            next.distances_ = 0;
        } else {
            assert(next.distances_ != 0);
            delete[] next.distances_;
            next.distances_ = 0;
            next.shared_ = true;
        }
        assert(next.distances_ == 0);
        //next.preprocess();
        outcomes.push_back(std::make_pair(next, 1));
    }
};


class min_min_t : public Heuristic::heuristic_t<state_t> {
    float multiplier_;
  public:
    min_min_t(float multiplier = 1.0) : multiplier_(multiplier) { }
    virtual ~min_min_t() { }
    virtual float value(const state_t &s) const {
        s.compute_heuristic();
        return (float)s.heuristic_ * multiplier_;
    }
    virtual void reset_stats() const { }
    virtual float setup_time() const { return 0; }
    virtual float eval_time() const { return 0; }
    virtual size_t size() const { return 0; }
    virtual void dump(std::ostream &os) const { }
    float operator()(const state_t &s) const { return value(s); }
};

class zero_heuristic_t: public Heuristic::heuristic_t<state_t> {
  public:
    zero_heuristic_t() { }
    virtual ~zero_heuristic_t() { }
    virtual float value(const state_t &s) const { return 0; }
    virtual void reset_stats() const { }
    virtual float setup_time() const { return 0; }
    virtual float eval_time() const { return 0; }
    virtual size_t size() const { return 0; }
    virtual void dump(std::ostream &os) const { }
    float operator()(const state_t &s) const { return value(s); }
};

class optimistic_policy_t : public Policy::policy_t<state_t> {
    const CTP::graph_t &graph_;
    float multiplier_;
  public:
    optimistic_policy_t(const Problem::problem_t<state_t> &problem, const CTP::graph_t &graph, float multiplier = 1.0)
      : Policy::policy_t<state_t>(problem), graph_(graph), multiplier_(multiplier) { }
    virtual ~optimistic_policy_t() { }
    virtual Problem::action_t operator()(const state_t &s) const {
        float best_cost = FLT_MAX;
        Problem::action_t best_action = Problem::noop;
        for( Problem::action_t a = 0; a < problem().number_actions(s); ++a ) {
            if( problem().applicable(s, a) ) {
                float cost = problem().cost(s, a);
                cost += graph_.bfs(a, graph_.num_nodes_ - 1, s.info_.known_, s.info_.blocked_, true) * multiplier_;
                if( cost < best_cost ) {
                    best_cost = cost;
                    best_action = a;
                }
            }
        }
        return best_action;
    }
    virtual const Policy::policy_t<state_t>* clone() const {
        return new optimistic_policy_t(problem(), graph_);
    }
    virtual void print_stats(std::ostream &os) const { }
};


inline
void sample_weather(const CTP::graph_t &graph, state_t &state) {
    state.clear();
    int num_edges = graph.with_shortcut_ ? graph.num_edges_ - 1 : graph.num_edges_;
    for( int e = 0; e < num_edges; ++e ) {
        float p = graph.prob(e);
        if( Random::real() < p ) {
            state.info_.set_edge_status(e, false);
        } else {
            state.info_.set_edge_status(e, true);
        }
    }
    if( graph.with_shortcut_ ) state.info_.set_edge_status(graph.num_edges_ - 1, true);
}

inline
float probability_bad_weather(const CTP::graph_t &graph, unsigned nsamples) {
    float prob = 0;
    state_t weather(0);
    for( unsigned i = 0; i < nsamples; ++i ) {
        sample_weather(graph, weather);
        //weather.preprocess();
        prob += weather.reachable(graph.num_nodes_ - 1) ? 0 : 1;
    }
    return prob / nsamples;
}

inline
std::pair<float, float> branching_factor(const problem_t &problem, unsigned nsamples) {
    Policy::random_t<state_t> random_policy(problem);
    for( unsigned i = 0; i < nsamples; ++i ) {
        state_t s = problem.init();
        while( !problem.terminal(s) ) {
            Problem::action_t a = random_policy(s);
            if( a == Problem::noop ) break;
            std::pair<state_t, bool> p = problem.sample(s, a);
            s = p.first;
        }
    }
    float avg = problem.avg_branching_;
    float max = problem.max_branching_;
    return std::make_pair(avg, max);
}

