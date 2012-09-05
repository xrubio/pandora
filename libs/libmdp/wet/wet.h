#include <iostream>
#include <cassert>
#include <math.h>

#define DISCOUNT   .95

#include "algorithm.h"
#include "parameters.h"
#include "heuristic.h"

#include "policy.h"
#include "rollout.h"
#include "mcts.h"
#include "dispatcher.h"

#define XVER          (version&0x1)
#define YVER          (version&0x2)
#define ZVER          (version&0x4)

int version = 0;
float kappa_table[] = { 0.0, 0.1, 0.3, 0.6 };

const Problem::action_t up = 1;
const Problem::action_t right = 2;
const Problem::action_t down = 3;
const Problem::action_t left = 4;
const char *asymb[] = { "-", "^", ">", "v", "<" };

class state_t {
    unsigned s_;
  protected:
    static size_t size_;
  public:
    state_t(unsigned x, unsigned y) : s_((x*size_) + y) { }
    state_t(unsigned s = 0) : s_(s) { }
    ~state_t() { }
    size_t hash() const { return s_; }
    unsigned s() const { return s_; }
    unsigned x() const { return s_ / size_; }
    unsigned y() const { return s_ % size_; }
    state_t move(Problem::action_t a) const {
        switch( a ) {
            case 1:
                if( y() == 0 ) return s();
                return state_t(x(), y() - 1);
                break;
            case 2:
                if( x() == size_ - 1 ) return s();
                return state_t(x() + 1, y());
                break;
            case 3:
                if( y() == size_ - 1 ) return s();
                return state_t(x(), y() + 1);
                break;
            case 4:
                if( x() == 0 ) return s();
                return state_t(x() - 1, y());
                break;
        }
        return s_;
    }

    const state_t& operator=(const state_t &s) {
        s_ = s.s_;
        return *this;
    }
    bool operator==(const state_t &s) const {
        return s_ == s.s_;
    }
    bool operator!=(const state_t &s) const {
        return s_ != s.s_;
    }
    bool operator<(const state_t &s) const {
        return s_ < s.s_;
    }
    void print(std::ostream &os) const {
        os << "( " << x() << " , " << y() << " )";
    }
    friend class problem_t;
};

size_t state_t::size_ = 0;

inline std::ostream& operator<<(std::ostream &os, const state_t &s) {
    s.print(os);
    return os;
}

class problem_t : public Problem::problem_t<state_t> {
    size_t size_;
    state_t init_;
    state_t goal_;
    char *water_;
    float p_;
  public:
    problem_t(size_t size, float p, const state_t &init = state_t(0,0), const state_t &goal = state_t(0,0))
     : size_(size), init_(init), goal_(goal), p_(p) {
        state_t::size_ = size_;
        water_ = new char[size_ * size_];
        for( size_t x = 0; x < size_; ++x )
            for( size_t y = 0; y < size_; ++y )
                if( Random::real() < p_ )
                    water_[(x * size_) + y] = 1 + (lrand48() % (XVER ? 2 : 3));
    }
    virtual ~problem_t() { delete[] water_; }
    size_t size() const { return size_; }
    unsigned water(size_t x, size_t y) const { return water_[(x * size_) + y]; }
    const state_t& goal() const { return goal_; }
    virtual Problem::action_t number_actions(const state_t &s) const { return 5; }
    virtual const state_t& init() const { return init_; }
    virtual bool terminal(const state_t &s ) const { return s == goal_; }
    virtual bool applicable(const state_t &s, Problem::action_t a) const {
        return true;
    }
    virtual float cost(const state_t &s, Problem::action_t a) const {
        return terminal(s) ? 0 : 1;
    }
    virtual void next(const state_t &s, Problem::action_t a, std::vector<std::pair<state_t, float> > &outcomes) const {
        ++expansions_;
        outcomes.clear();
        float e = kappa_table[water(s.x(), s.y())];
        float e2 = e*e;
        if( YVER ) {
            outcomes.reserve(4);
            outcomes.push_back(std::make_pair(s.move(a), 1.0 - e - e2));                               // kappa = 0
            if( e - e2 > 0.0 ) {
                outcomes.push_back(std::make_pair(s.move(1 + (a % 4)), (e - e2) / 2.0));               // kappa = 1
                outcomes.push_back(std::make_pair(s.move(1 + ((a+2) % 4)), (e - e2) / 2.0));           // kappa = 1
            }
            if( e2 > 0.0 ) outcomes.push_back(std::make_pair(s, 2 * e2));                              // kappa = 2
        } else if( ZVER ) {
            outcomes.reserve(3);
            outcomes.push_back(std::make_pair(s.move(a), 1.0 - e2));                                   // kappa = 0
            if( e2 > 0.0 ) {
                outcomes.push_back(std::make_pair(s.move(1 +(a % 4)), e2 / 2.0));                      // kappa = 2
                outcomes.push_back(std::make_pair(s.move(1 +((a+2) % 4)), e2 / 2.0));                  // kappa = 2
            }
        } else {
            outcomes.reserve(6);
            outcomes.push_back(std::make_pair(s.move(a), 1.0 - e - e2));                               // kappa = 0
            if( e - e2 > 0.0 ) {
                outcomes.push_back(std::make_pair((s.move(a)).move(1 + (a % 4)), (e - e2) / 4.0));     // kappa = 1
                outcomes.push_back(std::make_pair((s.move(a)).move(1 + ((a+2) % 4)), (e - e2) / 4.0)); // kappa = 1
                outcomes.push_back(std::make_pair(s.move(1 + (a % 4)), (e - e2) / 4.0));               // kappa = 1
                outcomes.push_back(std::make_pair(s.move(1 + ((a+2) % 4)), (e - e2) / 4.0));           // kappa = 1
            }
            if( e2 > 0.0 ) outcomes.push_back(std::make_pair(s, 2 * e2));                              // kappa = 2
        }
    }
    virtual void print(std::ostream &os) const {
        os << "size = " << size_ << std::endl
           << "init = " << init_ << std::endl
           << "goal = " << goal_ << std::endl;
        for( size_t x = 0; x < size_; ++x ) os << "--";
        os << "---" << std::endl;
        for( size_t y = 0; y < size_; ++y ) {
            os << "|";
            for( size_t x = 0; x < size_; ++x ) {
                os << " " << water(x,y);
            }
            os << " |" << std::endl;
        }
        for( size_t x = 0; x < size_; ++x ) os << "--";
        os << "---" << std::endl;
    }
    void print_solution(std::ostream &os, const Problem::hash_t<state_t> &hash) const {
        for( size_t x = 0; x < size_; ++x ) os << "--";
        os << "---" << std::endl;
        for( size_t y = 0; y < size_; ++y ) {
            os << "|";
            for( size_t x = 0; x < size_; ++x ) {
                if( terminal(state_t(x, y)) ) {
                    os << " *";
                } else {
                    std::pair<Problem::action_t, float> p = hash.bestQValue(state_t(x, y));
                    os << " " << asymb[p.first];
                }
            }
            os << " |" << std::endl;
        }
        for( size_t x = 0; x < size_; ++x ) os << "--";
        os << "---" << std::endl;
    }
};

inline std::ostream& operator<<(std::ostream &os, const problem_t &p) {
    p.print(os);
    return os;
}

