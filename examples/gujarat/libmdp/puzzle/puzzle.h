#include <cassert>
#include <iostream>
#include <iomanip>

#include "algorithm.h"
#include "parameters.h"
#include "heuristic.h"

#include "policy.h"
#include "rollout.h"
#include "mcts.h"
#include "dispatcher.h"

const Problem::action_t up = 0;
const Problem::action_t left = 1;
const Problem::action_t down = 2;
const Problem::action_t right = 3;

class bits_t {
    unsigned bits_;
  public:
    bits_t(unsigned bits) : bits_(bits) { }
    ~bits_t() { }
    void print(std::ostream &os) const {
        for( unsigned i = 8*sizeof(unsigned); i > 0; --i )
            os << ((bits_ >> (i-1)) % 2);
    }
};

inline std::ostream& operator<<(std::ostream &os, const bits_t &b) {
    b.print(os);
    return os;
}

class state_t {
    unsigned d0_;
    unsigned d1_;
    unsigned d2_;
  public:
    state_t(unsigned d0 = 0, unsigned d1 = 0, unsigned d2 = 0) : d0_(d0), d1_(d1), d2_(d2) { }
    state_t(const state_t &s) : d0_(s.d0_), d1_(s.d1_), d2_(s.d2_) { }
    ~state_t() { }
    size_t hash() const { return d1_ ^ d2_; }
    unsigned rows() const { return (d0_>>4) & 0xF; }
    unsigned cols() const { return (d0_>>8) & 0xF; }
    bool applicable(unsigned rows, unsigned cols, Problem::action_t a) const {
        switch( a ) {
            case ::up: return blank() >= cols;
            case ::right: return blank() % cols < cols-1;
            case ::down: return blank() + cols < rows*cols;
            case ::left: return blank() % cols > 0;
        }
        return false;
    }
    void set_tile(unsigned t, unsigned pos) {
        unsigned shift = pos < 8 ? pos << 2 : (pos - 8) << 2;
        if( pos < 8 ) {
            d1_ &= ~(0xF << shift);
            d1_ |= t<<shift;
        } else {
            d2_ &= ~(0xF << shift);
            d2_ |= t << shift;
        }
    }
    void set_blank(unsigned pos) {
        d0_ &= ~(0xF);
        d0_ |= pos;
        set_tile(0, pos);
    }
    unsigned blank() const {
        return d0_ & 0xF;
    }
    unsigned tile(unsigned pos) const {
        unsigned shift = pos < 8 ? pos << 2 : (pos - 8) << 2;
        return pos < 8 ? (d1_ >> shift) & 0xF : (d2_ >> shift) & 0xF;
    }
    void up(unsigned rows, unsigned cols) {
        unsigned b = blank();
        if( b >= cols ) {
            unsigned p = b-cols, t = tile(p);
            set_tile(t, b);
            set_blank(p);
        }
    }
    void right(unsigned rows, unsigned cols) {
        unsigned b = blank();
        if( b%cols < cols-1 ) {
            unsigned p = b+1, t = tile(p);
            set_tile(t, b);
            set_blank(p);
        }
    }
    void down(unsigned rows, unsigned cols) {
        unsigned b = blank();
        if( b+cols < rows*cols ) {
            unsigned p = b+cols, t = tile(p);
            set_tile(t, b);
            set_blank(p);
        }
    }
    void left(unsigned rows, unsigned cols) {
        unsigned b = blank();
        if( b%cols > 0 ) {
            unsigned p = b-1,
            t = tile(p);
            set_tile(t, b);
            set_blank(p);
        }
    }
    void random_moves(unsigned rows, unsigned cols, unsigned n = 0) {
        for( unsigned i = 0; i < n; ++i ) {
            unsigned m = Random::uniform(4);
            if( m == 0 )
                up(rows, cols);
            else if( m == 1 )
                right(rows, cols);
            else if( m == 2 )
                down(rows, cols);
            else
                left(rows, cols);
        }
    }
    void set_goal(unsigned rows, unsigned cols) {
        d0_ = d1_ = d2_ = 0;
        d0_ |= (rows<<4);
        d0_ |= (cols<<8);
        set_blank(0);
        for( unsigned t = 0; t < rows*cols; ++t )
            set_tile(t, 1+t);
    }
    unsigned manhattan() const {
        unsigned sum = 0;
        for( unsigned p = 0; p < rows()*cols(); ++p )
            if( p != blank() ) {
                unsigned t = tile(p);
                int r = ((1+t)/cols()) - (p/cols()), c = ((1+t)%cols()) - (p%cols());
                sum += (r<0?-r:r) + (c<0?-c:c);
            }
        return sum;
    }

    const state_t& operator=(const state_t &s) {
        d0_ = s.d0_;
        d1_ = s.d1_;
        d2_ = s.d2_;
        return *this;
    }
    bool operator==(const state_t &s) const {
        return (d0_ == s.d0_) && (d1_ == s.d1_) && (d2_ == s.d2_);
    }
    bool operator!=(const state_t &s) const {
        return (d0_ != s.d0_) || (d1_ != s.d1_) || (d2_ != s.d2_);
    }
    bool operator<(const state_t &s) const {
        return (d0_ < s.d0_) ||
               ((d0_ == s.d0_) && (d1_ < s.d1_)) ||
               ((d0_ == s.d0_) && (d1_ == s.d1_) && (d2_ < s.d2_));
    }
    void print(std::ostream &os) const {
        os << "|" << std::setw(2) << blank() << "|";
        for( unsigned i = 0; i < rows() * cols(); ++i ) {
            os << (i == blank() ? 0 : 1+tile(i));
            os << (i < rows() * cols() - 1 ? "," : "|");
        }
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
    state_t goal_;

  public:
    problem_t(unsigned rows, unsigned cols, const state_t &init, float p = 1.0)
      : Problem::problem_t<state_t>(),
        rows_(rows), cols_(cols), p_(p), init_(init) {
        goal_.set_goal(rows_, cols_);
    }
    virtual ~problem_t() { }

    virtual ::Problem::action_t number_actions(const state_t &s) const { return 4; }
    virtual const state_t& init() const { return init_; }
    virtual bool terminal(const state_t &s) const {
        return s == goal_;
    }
    virtual bool applicable(const state_t &s, ::Problem::action_t a) const {
        return s.applicable(rows_, cols_, a);
    }
    virtual bool dead_end(const state_t &s) const { return false; }
    virtual float cost(const state_t &s, Problem::action_t a) const {
        return terminal(s) ? 0 : 1;
    }
    virtual void next(const state_t &s, Problem::action_t a, std::vector<std::pair<state_t, float> > &outcomes) const {
        ++expansions_;
        outcomes.clear();
        outcomes.reserve(2);
        if( p_ > 0 ) {
            outcomes.push_back(std::make_pair(s, p_));
            if( a == ::up ) {
                outcomes.back().first.up(rows_, cols_);
            } else if( a == ::right ) {
                outcomes.back().first.right(rows_, cols_);
            } else if( a == ::down ) {
                outcomes.back().first.down(rows_, cols_);
            } else {
                outcomes.back().first.left(rows_, cols_);
            }
        }
        if( 1-p_ > 0 ) {
            outcomes.push_back(std::make_pair(s, 1 - p_));
        }
    }
    virtual void print(std::ostream &os) const { }
};

inline std::ostream& operator<<(std::ostream &os, const problem_t &p) {
    p.print(os);
    return os;
}

class manhattan_t : public Heuristic::heuristic_t<state_t> {
  public:
    manhattan_t() { }
    virtual ~manhattan_t() { }
    virtual float value(const state_t &s) const { return (float)s.manhattan(); }
    virtual void reset_stats() const { }
    virtual float setup_time() const { return 0; }
    virtual float eval_time() const { return 0; }
    virtual size_t size() const { return 0; }
    virtual void dump(std::ostream &os) const { }
    float operator()(const state_t &s) const { return value(s); }
};

