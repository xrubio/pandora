#include <iostream>
#include <iomanip>
#include <strings.h>

#include "algorithm.h"
#include "parameters.h"
#include "heuristic.h"

#include "policy.h"
#include "rollout.h"
#include "dispatcher.h"

#define DISCOUNT .95

class state_t {
    short x_;
    short y_;
    short wind_;

  public:
    enum { Away = 0, Down = 1, Cross = 2, Up = 3, Into = 4 }; // tacks

  public:
    state_t(short x = 0, short y = 0, short wind = 0)
      : x_(x), y_(y), wind_(wind) { }
    state_t(const state_t &s)
      : x_(s.x_), y_(s.y_), wind_(s.wind_) { }
    ~state_t() { }

    size_t hash() const {
        return (x_ << ((8*sizeof(short)) + 3)) | (y_ << 3) | wind_;
    }

    int tack(Problem::action_t a) const {
        int d = Utils::abs(a - wind_);
        return d < 8 - d ? d : 8 - d;
    }
    bool in_lake(short rows, short cols) const {
        return (x_ >= 0) && (x_ < rows) && (y_ >= 0) && (y_ < cols);
    }
    std::pair<int, int> direction(Problem::action_t a) const {
        switch( a ) {
            case 0: return std::make_pair(0, 1);
            case 1: return std::make_pair(1, 1);
            case 2: return std::make_pair(1, 0);
            case 3: return std::make_pair(1, -1);
            case 4: return std::make_pair(0, -1);
            case 5: return std::make_pair(-1, -1);
            case 6: return std::make_pair(-1, 0);
            case 7: return std::make_pair(-1, 1);
            default: return std::make_pair(-1, -1);
        }
    }

    state_t apply(Problem::action_t a) const {
        std::pair<int, int> dir = direction(a);
        return state_t(x_ + dir.first, y_ + dir.second);
    }

    const state_t& operator=( const state_t &s) {
        x_ = s.x_;
        y_ = s.y_;
        wind_ = s.wind_;
        return *this;
    }
    bool operator==(const state_t &s) const {
        return (x_ == s.x_) && (y_ == s.y_) && (wind_ == s.wind_);
    }
    bool operator!=(const state_t &s) const {
        return (x_ != s.x_) || (y_ != s.y_) || (wind_ != s.wind_);
    }
    bool operator<(const state_t &s) const {
        return (x_ < s.x_) ||
               ((x_ == s.x_) && (y_ < s.y_)) ||
               ((x_ == s.x_) && (y_ == s.y_) && (wind_ < s.wind_));
    }
    void print(std::ostream &os) const {
        os << "(" << x_ << "," << y_ << "," << wind_ << ")";
    }
    friend class problem_t;
};

inline std::ostream& operator<<(std::ostream &os, const state_t &s) {
    s.print(os);
    return os;
}

class problem_t : public Problem::problem_t<state_t> {
    int rows_;
    int cols_;
    float wind_transition_[64];
    float costs_[5];
    state_t init_;
    state_t goal_;

    static const float default_wind_transition_[];
    static const float default_costs_[];

  public:
    problem_t(int rows, int cols,
              int init_x = 0, int init_y = 0,
              int goal_x = std::numeric_limits<int>::max(),
              int goal_y = std::numeric_limits<int>::max(),
              float *wind_transition = 0, float *costs = 0)
      : Problem::problem_t<state_t>(DISCOUNT),
        rows_(rows), cols_(cols), init_(init_x, init_y), goal_(goal_x, goal_y) {

        if( (goal_x == std::numeric_limits<int>::max()) ||
            (goal_y == std::numeric_limits<int>::max()) ) {
            goal_ = state_t(rows - 1, cols - 1);
        }
        
        if( wind_transition != 0 ) {
            bcopy(wind_transition, wind_transition_, 64 * sizeof(float));
        } else {
            bcopy(default_wind_transition_, wind_transition_, 64 * sizeof(float));
        }

        if( costs != 0 ) {
            bcopy(costs, costs_, 5 * sizeof(float));
        } else {
            bcopy(default_costs_, costs_, 5 * sizeof(float));
        }
    }
    virtual ~problem_t() { }

    virtual Problem::action_t number_actions(const state_t &s) const { return 8; }
    virtual const state_t& init() const { return init_; }
    virtual bool terminal(const state_t &s) const {
        return (s.x_ == goal_.x_) && (s.y_ == goal_.y_);
    }
    virtual bool dead_end(const state_t &s) const { return false; }
    virtual bool applicable(const state_t &s, ::Problem::action_t a) const {
        return s.tack(a) == state_t::Into ? false : s.apply(a).in_lake(rows_, cols_);
    }
    virtual float cost(const state_t &s, Problem::action_t a) const {
        return terminal(s) ? 0 : costs_[s.tack(a)];
    }
    virtual void next(const state_t &s, Problem::action_t a, std::vector<std::pair<state_t,float> > &outcomes) const {
        ++expansions_;
        outcomes.clear();
        outcomes.reserve(8);
        state_t next_s = s.apply(a);
        assert(next_s.in_lake(rows_, cols_));
        for( int nwind = 0; nwind < 8; ++nwind ) {
            float p = wind_transition_[s.wind_ * 8 + nwind];
            if( p > 0 ) {
                next_s.wind_ = nwind;
                outcomes.push_back(std::make_pair(next_s, p));
            }
        }
    }
    virtual void print(std::ostream &os) const { }
};

const float problem_t::default_wind_transition_[] = {
    0.4, 0.3, 0.0, 0.0, 0.0, 0.0, 0.0, 0.3,
    0.4, 0.3, 0.3, 0.0, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.4, 0.3, 0.3, 0.0, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.4, 0.3, 0.3, 0.0, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.4, 0.2, 0.4, 0.0, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.3, 0.3, 0.4, 0.0,
    0.0, 0.0, 0.0, 0.0, 0.0, 0.3, 0.3, 0.4,
    0.4, 0.0, 0.0, 0.0, 0.0, 0.0, 0.3, 0.3
};

const float problem_t::default_costs_[] = {
    1, 2, 3, 4, std::numeric_limits<float>::max()
};

inline std::ostream& operator<<(std::ostream &os, const problem_t &p) {
    p.print(os);
    return os;
}

class scaled_heuristic_t : public Heuristic::heuristic_t<state_t> {
    const Heuristic::heuristic_t<state_t> *h_;
    float multiplier_;
  public:
    scaled_heuristic_t(const Heuristic::heuristic_t<state_t> *h, float multiplier = 1.0)
      : h_(h), multiplier_(multiplier) { }
    virtual ~scaled_heuristic_t() { }
    virtual float value(const state_t &s) const {
        return h_->value(s) * multiplier_;
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


