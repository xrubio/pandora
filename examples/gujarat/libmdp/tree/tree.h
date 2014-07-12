#include <iostream>
#include <iomanip>
#include <tr1/unordered_set>

#define DISCOUNT   1

#include "algorithm.h"
#include "parameters.h"
#include "heuristic.h"

#include "policy.h"
#include "rollout.h"
#include "mcts.h"
#include "dispatcher.h"

const Problem::action_t onelfwd = 0;
const Problem::action_t onerfwd = 1;

class bits_t {
    unsigned bits_;

  public:
    bits_t(unsigned bits) : bits_(bits) { }
    ~bits_t() { }
    void print(std::ostream &os) const {
        for( unsigned i = 8 * sizeof(unsigned); i > 0; --i )
            os << ((bits_ >> (i-1)) % 2);
    }
};

inline std::ostream& operator<<(std::ostream &os, const bits_t &b) {
    b.print(os);
    return os;
}

class state_t {
    unsigned data1_;
    unsigned data2_;

  public:
    state_t() : data1_(0), data2_(0) { }
    state_t(const state_t &s) : data1_(s.data1_), data2_(s.data2_) { }
    ~state_t() { }
    size_t hash() const { return data1_ ^ data2_; }
    unsigned depth() const { return data1_ >> 26; }
    unsigned branch1() const { return data1_ & ~(63 << 26); }
    std::pair<unsigned, unsigned> branch() const {
        return std::make_pair(branch1(), data2_);
    }
    unsigned onebits() const {
        unsigned count = 0;
        std::pair<unsigned, unsigned> b = branch();
        for( unsigned i = 0; i < 8 * sizeof(unsigned); ++i ) {
            count += b.first % 2;
            b.first = b.first >> 1;
            count += b.second % 2;
            b.second = b.second >> 1;
        }
        return count;
    }
    void onelfwd(unsigned n) {
        if( depth() < n-1 ) {
            unsigned d = depth() + 1;
            data1_ = (d << 26) | branch1();
        }
    }
    void onerfwd(unsigned n) {
        if( depth() < n-1 ) {
            unsigned d = depth() + 1;
            data1_ = (d << 26) | branch1();
            if( depth() <= 32 )
                data2_ ^= (1 << (depth() - 1));
            else
                data1_ ^= (1 << (depth() - 33));
        }
    }
    void onebwd() {
        if( depth() > 0 ) {
            if( depth() <= 32 )
                data2_ &= ~(1 << (depth() - 1));
            else
                data1_ &= ~(1 << (depth() - 33));
            unsigned d = depth() - 1;
            data1_ = (d << 26) | branch1();
        }
    }
    const state_t& operator=(const state_t &s) {
        data1_ = s.data1_;
        data2_ = s.data2_;
        return *this;
    }
    bool operator==(const state_t &s) const {
        return (data1_ == s.data1_) && (data2_ == s.data2_);
    }
    bool operator!=(const state_t &s) const {
        return (data1_ != s.data1_) || (data2_ != s.data2_);
    }
    bool operator<(const state_t &s) const {
        return (data1_ < s.data1_) ||
               ((data1_ == s.data1_) && (data2_ < s.data2_));
    }
    void print(std::ostream &os) const {
        bits_t b1(branch1()), b2(data2_);
        os << "( " << depth()
           << " , [" << branch1()
           << "|" << data2_
           << "]:" << b1
           << "|" << b2 << " )";
    }
    friend class problem_t;
};

inline std::ostream& operator<<(std::ostream &os, const state_t &s) {
    s.print(os);
    return os;
}

class problem_t : public Problem::problem_t<state_t> {
    unsigned n_;
    float p_;
    float q_;
    float r_;
    state_t init_;
    std::tr1::unordered_set<state_t, Hash::hash_function_t<state_t> > noisy_;

  public:
    problem_t(unsigned n, float p, float q = 0.0, float r = 0.0)
      : n_(n), p_(p), q_(q), r_(r) {
        fill_noisy_states();
    }
    virtual ~problem_t() { }

    void fill_noisy_states() {
        if( r_ > 0.0 ) {
            Problem::hash_t<state_t> hash(*this);
            Algorithm::generate_space(*this, init_, hash);
            for( Problem::hash_t<state_t>::const_iterator hi = hash.begin(); hi != hash.end(); ++hi ) {
                if( Random::real() < r_ ) noisy_.insert((*hi).first);
            }
        }
    }
    bool noisy(const state_t &s) const {
        return r_ > 0.0 ? (noisy_.find(s) != noisy_.end()) : false;
    }

    virtual Problem::action_t number_actions(const state_t &s) const { return 2; }
    virtual const state_t& init() const { return init_; }
    virtual bool terminal(const state_t &s) const {
        return s.depth() == n_ - 1;
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
        if( a == Problem::noop ) {
            outcomes.reserve(1);
            outcomes.push_back(std::make_pair(s, 1.0));
        } else {
            outcomes.reserve(3);
            float p = pow(p_, 1 + s.onebits());
            float q = noisy(s) ? p * q_ : 0.0;
            if( p - q > 0 ) outcomes.push_back(std::make_pair(s, p - q));
            if( q > 0 ) outcomes.push_back(std::make_pair(s, q));
            if( 1 - p > 0 ) outcomes.push_back(std::make_pair(s, 1 - p));

            unsigned j = 0;
            if( a == onelfwd ) {
                if( p - q > 0 ) outcomes[j++].first.onelfwd(n_);
                if( q > 0 ) outcomes[j++].first.onerfwd(n_);
            } else if( a == onerfwd ) {
                if( p - q > 0 ) outcomes[j++].first.onerfwd(n_);
                if( q > 0 ) outcomes[j++].first.onelfwd(n_);
            }
            if( 1 - p > 0 ) outcomes[j++].first.onebwd();
        }
    }
    virtual void print(std::ostream &os) const { }
};

inline std::ostream& operator<<(std::ostream &os, const problem_t &p) {
    p.print(os);
    return os;
}

