#include <cassert>
#include <iostream>
#include <map>
#include <vector>

#include "parsing.h"
#include "algorithm.h"
#include "parameters.h"
#include "heuristic.h"

#include "policy.h"
#include "rollout.h"
#include "dispatcher.h"

#define DISCOUNT 1.00

class state_t {
    short x_;
    short y_;
    short dx_;
    short dy_;

  public:
    state_t(short x = 0, short y = 0, short dx = 0, short dy = 0)
      : x_(x), y_(y), dx_(dx), dy_(dy) {
    }
    state_t(const state_t &s)
      : x_(s.x_), y_(s.y_), dx_(s.dx_), dy_(s.dy_) {
    }
    ~state_t() { }

    short x() const { return x_; }
    short y() const { return y_; }
    short dx() const { return dx_; }
    short dy() const { return dy_; }

    size_t hash() const {
        return (x_ | (y_<<16)) ^ (dx_ | (dy_<<16));
    }

    const state_t& operator=(const state_t &s) {
        x_ = s.x_;
        y_ = s.y_;
        dx_ = s.dx_;
        dy_ = s.dy_;
        return *this;
    }
    bool operator==(const state_t &s) const {
        return (x_ == s.x_) && (y_ == s.y_) && (dx_ == s.dx_) && (dy_ == s.dy_);
    }
    bool operator!=(const state_t &s) const {
        return (x_ != s.x_) || (y_ != s.y_) || (dx_ != s.dx_) || (dy_ != s.dy_);
    }
    bool operator<(const state_t &s) const {
        return (x_ < s.x_) ||
               ((x_ == s.x_) && (y_ < s.y_)) ||
               ((x_ == s.x_) && (y_ == s.y_) && (dx_ < s.dx_)) ||
               ((x_ == s.x_) && (y_ == s.y_) && (dx_ == s.dx_) && (dy_ < s.dy_));
    }
    void print(std::ostream &os) const {
        os << "(" << x_ << "," << y_ << "," << dx_ << "," << dy_ << ")";
    }
    friend class problem_t;
};

inline std::ostream& operator<<(std::ostream &os, const state_t &s) {
    s.print(os);
    return os;
}

class ecache_t : public std::tr1::unordered_map<size_t, std::pair<state_t, state_t> > { };
//class ecache_t : public map<size_t,pair<state_t,state_t> > { };

class problem_t : public Problem::problem_t<state_t> {
    const grid_t &grid_;
    float p_;
    size_t rows_;
    size_t cols_;
    state_t init_;
    std::vector<state_t> inits_;
    std::vector<state_t> goals_;
    mutable ecache_t *ecache_[9];

  public:
    problem_t(grid_t &grid, float p = 1.0)
      : Problem::problem_t<state_t>(DISCOUNT),
        grid_(grid), p_(p), rows_(grid.rows()), cols_(grid.cols()),
        init_(std::numeric_limits<short>::max(), std::numeric_limits<short>::max(),
              std::numeric_limits<short>::max(), std::numeric_limits<short>::max()) {
        for( size_t i = 0; i < grid_.starts().size(); ++i ) {
            size_t s = grid_.start(i);
            inits_.push_back(state_t(s / cols_, s % cols_));
        }
        for( size_t i = 0; i < grid_.goals().size(); ++i ) {
            size_t s = grid_.goal(i);
            goals_.push_back(state_t(s / cols_, s % cols_));
        }
        for( size_t i = 0; i < 9; ++i )
            ecache_[i] = new ecache_t[rows_*cols_];
    }
    virtual ~problem_t() {
        for( size_t i = 0; i < 9; ++i )
            delete[] ecache_[i];
    }

    virtual Problem::action_t number_actions(const state_t &s) const { return 9; }
    virtual const state_t& init() const { return init_; }
    virtual bool terminal(const state_t &s) const {
        return (s != init_) && grid_.goal_pos(s.x(), s.y());
    }
    virtual bool dead_end(const state_t &s) const { return false; }
    virtual bool applicable(const state_t &s, Problem::action_t a) const {
        return (a == 0) || (s != init_);
    }
    virtual float cost(const state_t &s, Problem::action_t a) const {
        return terminal(s) ? 0 : 1;
    }
    virtual void next(const state_t &s, Problem::action_t a, std::vector<std::pair<state_t, float> > &outcomes) const {
        ++expansions_;
        outcomes.clear();
        if( s == init_ ) {
            outcomes.reserve(inits_.size());
            for( size_t j = 0; j < inits_.size(); ++j )
                outcomes.push_back(std::make_pair(inits_[j], 1.0 / (float)inits_.size()));
        } else {
            outcomes.reserve(2);
            size_t off = s.x() * cols_ + s.y();
            size_t key = (((unsigned short)s.dx()) << 16) | (unsigned short)s.dy();
            ecache_t::const_iterator ci = ecache_[a][off].find(key);
            if( ci != ecache_[a][off].end() ) {
                if( p_ > 0.0 ) {
                    outcomes.push_back(std::make_pair(ci->second.first, p_));
                    assert(grid_.valid_pos(ci->second.first.x(), ci->second.first.y()));
                }
                if( 1 - p_ > 0.0 ) {
                    outcomes.push_back(std::make_pair(ci->second.second, 1 - p_));
                    assert(grid_.valid_pos(ci->second.second.x(), ci->second.second.y()));
                }
            } else {
                std::pair<state_t, state_t> entry;
                short ox = 0, oy = 0, ux = (a/3) - 1, uy = (a%3) - 1;
                if( p_ > 0.0 ) {
                    int dx = s.dx() + ux, dy = s.dy() + uy;
                    int x = s.x() + dx, y = s.y() + dy;
                    int rv = grid_.valid_path(s.x(), s.y(), x, y, ox, oy);
                    if( rv == 0 ) {
                        entry.first = state_t(x, y, dx, dy);
                    } else {
                        entry.first = state_t(ox, oy, 0, 0);
                    }
                    assert(grid_.valid_pos(entry.first.x(), entry.first.y()));
                    outcomes.push_back(std::make_pair(entry.first, p_));
                }
                if( 1 - p_ > 0.0 ) {
                    int dx = s.dx(), dy = s.dy();
                    int x = s.x() + dx, y = s.y() + dy;
                    int rv = grid_.valid_path(s.x(), s.y(), x, y, ox, oy);
                    if( rv == 0 ) {
                        entry.second = state_t(x, y, dx, dy);
                    } else {
                        entry.second = state_t(ox, oy, 0, 0);
                    }
                    assert(grid_.valid_pos(entry.first.x(), entry.first.y()));
                    outcomes.push_back(std::make_pair(entry.second, 1 - p_));
                }
                ecache_[a][off].insert(std::make_pair(key, entry));
            }
        }
    }
    virtual void print(std::ostream &os) const { }
};

inline std::ostream& operator<<(std::ostream &os, const problem_t &p) {
    p.print(os);
    return os;
}

