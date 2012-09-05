#include <iostream>
#include <iomanip>

#define DISCOUNT   .95

#include "algorithm.h"
#include "parameters.h"
#include "heuristic.h"

#include "policy.h"
#include "rollout.h"
#include "mcts.h"
#include "dispatcher.h"

const Problem::action_t fwd = 0;
const Problem::action_t left = 1;
const Problem::action_t right = 2;

typedef unsigned short ushort_t;

class state_t {
    ushort_t row_;
    ushort_t col_;

  public:
    state_t(ushort_t row = 0, ushort_t col = 0) : row_(row), col_(col) { }
    state_t(const state_t &s) : row_(s.row_), col_(s.col_) { }
    ~state_t() { }
    size_t hash() const { return row_ ^ col_; }
    unsigned row() const { return row_; }
    unsigned col() const { return col_; }
    void fwd(unsigned rows) { if( row() < rows - 1 ) ++row_; }
    void left(unsigned cols) { if( col() > 0 ) --col_; }
    void right(unsigned cols) { if( col() < cols - 1 ) ++col_; }
    const state_t& operator=( const state_t &s) {
        row_ = s.row_;
        col_ = s.col_;
        return *this;
    }
    bool operator==(const state_t &s) const {
        return (row_ == s.row_) && (col_ == s.col_);
    }
    bool operator!=(const state_t &s) const {
        return (row_ != s.row_) || (col_ != s.col_);
    }
    bool operator<(const state_t &s) const {
        return (row_ < s.row_) || ((row_ == s.row_) && (col_ < s.col_));
    }
    void print(std::ostream &os) const {
        os << "( " << row() << " , " << col() << " )";
    }
    friend class problem_t;
};

inline std::ostream& operator<<(std::ostream &os, const state_t &s) {
    s.print(os);
    return os;
}

class problem_t : public Problem::problem_t<state_t> {
    unsigned rows_;
    unsigned cols_;
    float p_;
    state_t init_;

  public:
    problem_t(unsigned rows, unsigned cols, float p = 1.0)
      : rows_(rows), cols_(cols), p_(p), init_(0, cols / 2) {
    }
    virtual ~problem_t() { }

    virtual Problem::action_t number_actions(const state_t &s) const { return 3; }
    virtual const state_t& init() const { return init_; }
    virtual bool terminal(const state_t &s) const {
        return s.row() == rows_ - 1;
    }
    virtual bool applicable(const state_t &s, ::Problem::action_t a) const {
        return true;
    }
    virtual float cost(const state_t &s, Problem::action_t a) const {
        return terminal(s) ? 0 : 1;
    }
    virtual void next(const state_t &s, Problem::action_t a, std::vector<std::pair<state_t, float> > &outcomes) const {
        ++expansions_;
        outcomes.clear();
        if( a != fwd ) {
            outcomes.reserve(1);
            outcomes.push_back(std::make_pair(s, 1.0));
            if( a == ::left ) {
                outcomes.back().first.left(cols_);
            } else if( a == ::right ) {
                outcomes.back().first.right(cols_);
            }
        } else if( a == fwd ) {
            outcomes.reserve(3);
            if( p_ > 0 ) {
                outcomes.push_back(std::make_pair(s, p_));
                outcomes.back().first.fwd(rows_);
            }
            if( 1 - p_ > 0 ) {
                outcomes.push_back(std::make_pair(s, (1 - p_) / 2));
                outcomes.back().first.left(cols_);
                outcomes.push_back(std::make_pair(s, (1 - p_) / 2));
                outcomes.back().first.right(cols_);
            }
        }
    }
    virtual void print(std::ostream &os) const { }
};

inline std::ostream& operator<<(std::ostream &os, const problem_t &p) {
    p.print(os);
    return os;
}

