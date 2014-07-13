#include <iostream>
#include <vector>
#include <queue>
#include <limits.h>

namespace CTP {

struct open_list_cmp {
    bool operator()(const std::pair<int, int> &p1, const std::pair<int, int> &p2) {
        return p1.second > p2.second;
    }
};

struct bfs_open_list_cmp {
    bool operator()(const std::pair<int, std::pair<int, int> > &p1, const std::pair<int, std::pair<int, int> > &p2) {
        return p1.second.first + p1.second.second > p2.second.first + p2.second.second;
    }
};

struct graph_t {
    int num_nodes_;
    int num_edges_;
    bool with_shortcut_;
    int shortcut_cost_;

    struct edge_t {
        int from_, to_;
        int cost_;
        float prob_; // probability that edge is traversable (priv. comm. from Malte et al.)
        edge_t(int from, int to, int cost, float prob)
          : from_(from), to_(to), cost_(cost), prob_(prob) { }
    };

    std::vector<edge_t> edge_list_;
    std::vector<std::vector<int> > at_;
    int *edges_;
    int *h_opt_;
    int *in_queue_;

    graph_t(bool with_shortcut = false, int shortcut_cost = 1000)
      : num_nodes_(0), num_edges_(0),
        with_shortcut_(with_shortcut), shortcut_cost_(shortcut_cost),
        edges_(0), h_opt_(0), in_queue_(0) { }
    ~graph_t() {
        delete[] edges_;
        delete[] h_opt_;
        delete[] in_queue_;
    }

    int heuristic(int node) const { return h_opt_[node]; }
    int from(int edge) const { return edge_list_[edge].from_; }
    int to(int edge) const { return edge_list_[edge].to_; }
    int cost(int edge) const { return edge_list_[edge].cost_; }
    float prob(int edge) const { return edge_list_[edge].prob_; }

    int degree(int node) const { return at_[node].size(); }
    int degree() const {
        int d = 0;
        for( int node = 0; node < num_nodes_; ++node ) {
            int dn = degree(node);
            d = dn > d ? dn : d;
        }
        return d;
    }

    int add_edge(const edge_t &edge) {
        assert((int)edge_list_.size() == num_edges_);
        edge_list_.push_back(edge);
        at_[edge.from_].push_back(num_edges_);
        at_[edge.to_].push_back(num_edges_);
        edges_[edge.from_ * num_nodes_ + edge.to_] = num_edges_;
        edges_[edge.to_ * num_nodes_ + edge.from_] = num_edges_;
        return num_edges_++;
    }

    bool parse(std::istream &is) {
        // read graph from file
        edge_list_.clear();
        at_.clear();

        std::string token;
        is >> token;
        if( token == "p" ) {
            int n_edges;
            is >> num_nodes_ >> n_edges;

            edges_ = new int[num_nodes_ * num_nodes_];
            for( int n1 = 0; n1 < num_nodes_; ++n1 ) {
                for( int n2 = 0; n2 < num_nodes_; ++n2 )
                    edges_[n1*num_nodes_ + n2] = -1;
            }

            at_.resize(num_nodes_);
            for( int e = 0; e < n_edges; ++e ) {
                is >> token;
                if( token == "e" ) {
                    int from, to, cost;
                    float prob;
                    is >> from >> to >> prob >> cost;
                    --from;
                    --to;
                    add_edge(edge_t(from, to, cost, prob));
                } else {
                    std::cout << "error reading input file: road doesn't start with 'e'." << std::endl;
                    return false;
                }
            }
        } else {
            std::cout << "error reading input file: file doesn't start with 'p'." << std::endl;
            return false;
        }

        // insert shortcut (s,t) edge
        if( with_shortcut_ ) {
            std::cout << "info: adding (s,t) shortcut w/ cost " << shortcut_cost_ << std::endl;
            add_edge(edge_t(0, num_nodes_ - 1, shortcut_cost_, 1.0));
        }

        std::cout << "info: #nodes=" << num_nodes_
                  << ", #edges=" << num_edges_
                  << ", degree=" << degree()
                  << std::endl;

        // compute optimistic shortest-paths to goal
        h_opt_ = new int[num_nodes_];
        in_queue_ = new int[num_nodes_];
        for( int node = 0; node < num_nodes_; ++node )
            h_opt_[node] = INT_MAX;

        std::priority_queue<std::pair<int, int>,
                            std::vector<std::pair<int, int> >,
                            open_list_cmp> queue;

        int goal = num_nodes_ - 1;
        h_opt_[goal] = 0;
        queue.push(std::make_pair(goal, 0));
        while( !queue.empty() ) {
            std::pair<int, int> p = queue.top();
            queue.pop();
            if( p.second <= h_opt_[p.first] ) {
                for( int i = 0, isz = at_[p.first].size(); i < isz; ++i ) {
                    const edge_t &e = edge_list_[at_[p.first][i]];
                    int cost = p.second + e.cost_;
                    int to = e.from_ == p.first ? e.to_ : e.from_;
                    if( cost < h_opt_[to] ) {
                        h_opt_[to] = cost;
                        queue.push(std::make_pair(to, cost));
                    }
                }
            }
        }

        return true;
    }

    bool known(int edge, const unsigned *bitmap) const {
        int n = edge >> 5, offset = edge & 0x1F;
        int mask = 1 << offset;
        return bitmap[n] & mask;
    }
    bool traversable(int edge, const unsigned *bitmap) const {
        int n = edge >> 5, offset = edge & 0x1F;
        int mask = 1 << offset;
        return (bitmap[n] & mask) != 0 ? false : true;
    }
    bool good(int edge, const unsigned *k_bitmap, const unsigned *b_bitmap, bool optimistic) const {
        return (known(edge, k_bitmap) && traversable(edge, b_bitmap)) ||
               (optimistic && !known(edge, k_bitmap));
    }
    int edge(int n1, int n2,
             const unsigned *k_bitmap,
             const unsigned *b_bitmap,
             bool optimistic) const {
        int edge = edges_[n1*num_nodes_ + n2];
        if( edge != -1 ) {
            return good(edge, k_bitmap, b_bitmap, optimistic) ? edge : -1;
        } else {
            return -1;
        }
    }

    int bfs(int start,
            int goal,
            const unsigned *k_bitmap,
            const unsigned *b_bitmap,
            bool optimistic = false) const {

        static std::priority_queue<std::pair<int, std::pair<int, int> >,
                                   std::vector<std::pair<int, std::pair<int, int> > >,
                                   bfs_open_list_cmp> queue;

        int cost_to_goal = INT_MAX;
        memset(in_queue_, 0, num_nodes_ * sizeof(int));
        queue.push(std::make_pair(start, std::make_pair(0, heuristic(start))));
        in_queue_[start] = 1;
        while( !queue.empty() ) {
            std::pair<int, std::pair<int, int> > p = queue.top();
            queue.pop();

            if( p.first == goal ) {
                cost_to_goal = p.second.first;
                break;
            }

            for( int i = 0, isz = at_[p.first].size(); i < isz; ++i ) {
                int idx = at_[p.first][i];
                if( good(idx, k_bitmap, b_bitmap, optimistic) ) {
                    const edge_t &edge = edge_list_[idx];
                    int cost = p.second.first + edge.cost_;
                    int next = p.first == edge.to_ ? edge.from_ : edge.to_;
                    if( in_queue_[next] == 0 ) {
                        queue.push(std::make_pair(next, std::make_pair(cost, heuristic(next))));
                        in_queue_[next] = 1;
                    }
                }
            }
        }

        while( !queue.empty() ) queue.pop();
        return cost_to_goal;
    }

    void dijkstra(int source,
                  int *distances,
                  const unsigned *k_bitmap,
                  const unsigned *b_bitmap,
                  bool optimistic = false) const {

        static std::priority_queue<std::pair<int, int>,
                                   std::vector<std::pair<int, int> >,
                                   open_list_cmp> queue;

        // initialization
        for( int node = 0; node < num_nodes_; ++node )
            distances[node] = INT_MAX;

        // Dijsktra's
        distances[source] = 0;
        queue.push(std::make_pair(source, 0));
        while( !queue.empty() ) {
            std::pair<int, int> p = queue.top();
            queue.pop();
            if( p.second <= distances[p.first] ) {
                for( int i = 0, isz = at_[p.first].size(); i < isz; ++i ) {
                    int idx = at_[p.first][i];
                    if( good(idx, k_bitmap, b_bitmap, optimistic) ) {
                        const edge_t &edge = edge_list_[idx];
                        int cost = p.second + edge.cost_;
                        int next = p.first == edge.to_ ? edge.from_ : edge.to_;
                        if( cost < distances[next] ) {
                            distances[next] = cost;
                            queue.push(std::make_pair(next, cost));
                        }
                    }
                }
            }
        }
    }

    void floyd_warshall(int *distances,
                        const unsigned *k_bitmap,
                        const unsigned *b_bitmap,
                        bool optimistic = false) const {

        // initialization
        for( int n = 0; n < num_nodes_ * num_nodes_; ++n ) {
            distances[n] = INT_MAX;
        }
        for( int n = 0; n < num_nodes_; ++n ) {
            distances[n*num_nodes_ + n] = 0;
        }
        for( int e = 0; e < num_edges_; ++e ) {
            if( good(e, k_bitmap, b_bitmap, optimistic) ) {
                int n1 = to(e), n2 = from(e), edge_cost = cost(e);
                distances[n1*num_nodes_ + n2] = edge_cost;
                distances[n2*num_nodes_ + n1] = edge_cost;
            }
        }

        // Floyd-Warshall's
        for( int k = 0; k < num_nodes_; ++k ) {
            for( int i = 0; i < num_nodes_; ++i ) {
                if( distances[i*num_nodes_+k] == INT_MAX ) continue;
                for( int j = 0; j < num_nodes_; ++j ) {
                    if( distances[k*num_nodes_+j] == INT_MAX ) continue;
                    int ncost = distances[i*num_nodes_+k]+distances[k*num_nodes_+j];
                    if( ncost < distances[i*num_nodes_+j] ) {
                        distances[i*num_nodes_+j] = ncost;
                    }
                }
            }
        }
    }
};

}; // namespace CTP

