#include <iostream>
#include <vector>
#include <queue>

namespace CTP {

struct open_list_cmp {
    bool operator()(const std::pair<int, int> &p1, const std::pair<int, int> &p2) {
        return p1.second > p2.second;
    }
};

struct graph_t {
    int num_nodes_;
    int num_edges_;
    bool with_shortcut_;
    int shortcut_cost_;
    int *h_opt_;

    struct edge_t {
        int from_, to_;
        int cost_;
        float prob_;
        edge_t(int from, int to, int cost, float prob) : from_(from), to_(to), cost_(cost), prob_(prob) { }
    };

    std::vector<edge_t> edge_list_;
    std::vector<std::vector<int> > at_;

    graph_t(bool with_shortcut = false, int shortcut_cost = 1000)
      : with_shortcut_(with_shortcut), shortcut_cost_(shortcut_cost), h_opt_(0) { }
    ~graph_t() { delete[] h_opt_; }

    int from(int edge) const { return edge_list_[edge].from_; }
    int to(int edge) const { return edge_list_[edge].to_; }
    int cost(int edge) const { return edge_list_[edge].cost_; }
    float prob(int edge) const { return edge_list_[edge].prob_; }

    void dump(std::ostream &os) const;

    bool parse(std::istream &is) {
        // read graph from file
        edge_list_.clear();
        at_.clear();

        std::string token;
        is >> token;
        if( token == "p" ) {
            is >> num_nodes_ >> num_edges_;
            ++num_edges_; // increase count to make space for fixed (s,t) edge
            if( num_edges_ > 64 ) {
                std::cout << "error: number of edges must be less than or equal to 64." << std::endl;
                return false;
            }

            at_.resize(num_nodes_);
            for( int e = 0; e < num_edges_ - 1; ++e ) {
                is >> token;
                if( token == "e" ) {
                    int from, to, cost;
                    float prob;
                    is >> from >> to >> prob >> cost;
                    --from;
                    --to;
                    edge_t edge(from, to, cost, prob);
                    edge_list_.push_back(edge);
                    at_[from].push_back(e);
                    at_[to].push_back(e);
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
            int from = 0, to = num_nodes_ - 1, e = num_edges_ - 1;
            edge_list_.push_back(edge_t(from, to, shortcut_cost_, 1));
            at_[from].push_back(e);
            at_[to].push_back(e);
        }

        // compute optimistic shortest-paths to goal
        h_opt_ = new int[num_nodes_];
        for( int n = 0; n < num_nodes_; ++n )
            h_opt_[n] = std::numeric_limits<int>::max();

        std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int> >, open_list_cmp> open;
        int goal = num_nodes_ - 1;
        h_opt_[goal] = 0;
        open.push(std::make_pair(goal, 0));
        while( !open.empty() ) {
            std::pair<int, int> p = open.top();
            open.pop();
            if( p.second <= h_opt_[p.first] ) {
                for( int i = 0, isz = at_[p.first].size(); i < isz; ++i ) {
                    const edge_t &e = edge_list_[at_[p.first][i]];
                    int cost = p.second + e.cost_;
                    int to = e.from_ == p.first ? e.to_ : e.from_;
                    if( cost < h_opt_[to] ) {
                        h_opt_[to] = cost;
                        open.push(std::make_pair(to, cost));
                    }
                }
            }
        }
        return true;
    }
};

}; // namespace CTP

