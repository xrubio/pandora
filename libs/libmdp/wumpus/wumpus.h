#include <cassert>
#include <cstring>
#include <iostream>
#include <map>
#include <vector>
#include <limits>
#include <set>

#include "cow_vector.h"

#include "algorithm.h"
#include "parameters.h"
#include "heuristic.h"

#include "policy.h"
#include "rollout.h"
#include "dispatcher.h"

#define DISCOUNT 1.00

#define USE_COW_COMPONENTS // use copy-no-write belief components

#define POS_MASK     16383
#define POS_SHIFT    14
#define STATUS_SHIFT 28

#define GOLD         0
#define PIT          1
#define WUMPUS       2

#define GLITTER      0x1
#define BREEZE       0x2
#define STENCH       0x4

#define MOVE_UP      0
#define MOVE_RIGHT   1
#define MOVE_DOWN    2
#define MOVE_LEFT    3
#define GRAB_GOLD    4

inline int abs(int x) {
    return x >= 0 ? x : -x;
}

inline bool adjacent(int pos1, int pos2) {
    int row1 = pos1 & 127, col1 = pos1 >> 7;
    int row2 = pos2 & 127, col2 = pos2 >> 7;
    if( (row1 == row2) && (abs(col1 - col2) == 1) )
        return true;
    else if( (col1 == col2) && (abs(row1 - row2) == 1) )
        return true;
    else
        return false;
}

inline int calculate_new_pos(int pos, int drow, int dcol, int rows, int cols) {
    int row = pos & 127, col = pos >> 7;
    int nrow = row + drow, ncol = col + dcol;
    if( (nrow < 0) || (nrow >= rows) ) nrow = row;
    if( (ncol < 0) || (ncol >= cols) ) ncol = col;
    return (ncol << 7) | nrow;
}

inline int calculate_new_pos(int pos, int a, int rows, int cols) {
    if( a == MOVE_UP ) {
        return calculate_new_pos(pos, 1, 0, rows, cols);
    } else if( a == MOVE_RIGHT ) {
        return calculate_new_pos(pos, 0, 1, rows, cols);
    } else if( a == MOVE_DOWN ) {
        return calculate_new_pos(pos, -1, 0, rows, cols);
    } else if( a == MOVE_LEFT ) {
        return calculate_new_pos(pos, 0, -1, rows, cols);
    } else {
        return pos;
    }
}

#ifdef USE_COW_COMPONENTS

namespace COW {

struct belief_component_t {
    COW::vector_t<unsigned> *vector_;
    int type_;
   
    belief_component_t()
      : vector_(new COW::vector_t<unsigned>), type_(-1) { }
    belief_component_t(const belief_component_t &bc)
      : vector_(ref(bc.vector_)), type_(bc.type_) { }
    ~belief_component_t() { deallocate(vector_); }

    static void deallocate(COW::vector_t<unsigned> *vec) {
        COW::vector_t<unsigned>::deallocate(vec);
    }
    static COW::vector_t<unsigned>* ref(const COW::vector_t<unsigned> *vec) {
        return COW::vector_t<unsigned>::ref(vec);
    }

    unsigned hash() const {
        return vector_->hash();
    }
    float value() const {
        int value = 0;
        for( int i = 0; i < vector_->size(); ++i ) {
            unsigned sample = (*vector_)[i];
            int status = (sample >> STATUS_SHIFT) & 0x1;
            value += 1 - status;
        }
        return value;
    }

    int nrefs() const {
        return vector_->nrefs();
    }
    void reserve(int cap) {
        vector_->reserve(cap);
    }
    void clear() {
        if( nrefs() > 1 ) {
            deallocate(vector_);
            vector_ = new COW::vector_t<unsigned>;
        } else
            vector_->clear();
    }
    bool empty() const {
        return vector_->empty();
    }
    int size() const {
        return vector_->size();
    }

    bool got_gold() const {
        if( type_ != GOLD ) return false;
        for( int i = 0; i < vector_->size(); ++i ) {
            unsigned sample = (*vector_)[i];
            int status = (sample >> STATUS_SHIFT) & 0x1;
            if( status == 0 ) return false;
        }
        return true;
    }

    bool safe_pos(int pos) const {
        if( type_ == GOLD ) return true;
        for( int i = 0; i < vector_->size(); ++i ) {
            unsigned sample = (*vector_)[i];
            int other_pos = (sample >> POS_SHIFT) & POS_MASK;
            if( pos == other_pos ) return false;
        }
        return true;
    }

    void insert(int e) {
        if( nrefs() > 1 ) {
            deallocate(vector_);
            COW::vector_t<unsigned> *nvec = new COW::vector_t<unsigned>(*vector_);
            vector_ = nvec;
        }
        vector_->insert(e);
    }

    int operator[](int i) const { return (*vector_)[i]; }
    const belief_component_t& operator=(const belief_component_t &bc) {
        deallocate(vector_);
        vector_ = ref(bc.vector_);
        return *this;
    }
    bool operator==(const belief_component_t &bc) const {
        return *vector_ == *bc.vector_;
    }
    bool operator!=(const belief_component_t &bc) const {
        return *vector_ != *bc.vector_;
    }

    void apply_move(int rows, int cols, int drow, int dcol, belief_component_t &result, bool non_det = false) const {
        result.reserve(size());
        for( int i = 0; i < vector_->size(); ++i ) {
            unsigned sample = (*vector_)[i];
            int pos = sample & POS_MASK;
            int other_pos = (sample >> POS_SHIFT) & POS_MASK;
            int status = (sample >> STATUS_SHIFT) & 0x1;
            assert((type_ == GOLD) || (status == 0));

            int row = pos & 127, col = pos >> 7;
            int nrow = row + drow, ncol = col + dcol;
            if( (nrow < 0) || (nrow >= rows) ) nrow = row;
            if( (ncol < 0) || (ncol >= cols) ) ncol = col;
            int new_pos = (ncol << 7) | nrow;

            if( (type_ == GOLD) || (other_pos != new_pos) ) {
                result.insert((status << STATUS_SHIFT) | (other_pos << POS_SHIFT) | new_pos);
            }
        }
    }

    void apply_grab_gold(belief_component_t &result, bool non_det = false) const {
        result.reserve(size());
        for( int i = 0; i < vector_->size(); ++i ) {
            int sample = (*vector_)[i];
            if( type_ != GOLD ) {
                result.insert(sample);
            } else {
                int pos = sample & POS_MASK;
                int other_pos = (sample >> POS_SHIFT) & POS_MASK;
                int status = (sample >> STATUS_SHIFT) & 0x1;
                int nstatus = (status == 1) || (other_pos == pos) ? 1 : 0;
                result.insert((nstatus << STATUS_SHIFT) | (other_pos << POS_SHIFT) | pos);
            }
        }
    }

    void filter(int obs, belief_component_t &result) const {
        //std::cout << "filter w/ obs=" << obs << std::endl;
        result.reserve(size());
        for( int i = 0; i < vector_->size(); ++i ) {
            int sample = (*vector_)[i];
            int pos = sample & POS_MASK;
            int other_pos = (sample >> POS_SHIFT) & POS_MASK;
            bool sample_alive = true;

            if( type_ == GOLD ) {
                if( obs & GLITTER ) {
                    sample_alive = (pos == other_pos) || adjacent(pos, other_pos);
                } else {
                    sample_alive = (pos != other_pos) && !adjacent(pos, other_pos);
                }
            } else if( type_ == PIT ) {
                if( obs & BREEZE ) {
                    sample_alive = adjacent(pos, other_pos);
                } else {
                    sample_alive = !adjacent(pos, other_pos);
                }
            } else {
                assert(type_ == WUMPUS);
                if( obs & STENCH ) {
                    sample_alive = adjacent(pos, other_pos);
                } else {
                    sample_alive = !adjacent(pos, other_pos);
                }
            }

            if( sample_alive ) result.insert(sample);
        }
        //std::cout << "state="; print(std::cout); std::cout << std::endl << "filtd="; result.print(std::cout); std::cout << std::endl;
    }

    void print(std::ostream &os) const {
        os << "{";
        for( int i = 0; i < vector_->size(); ++i ) {
            int sample = (*vector_)[i];
            int pos = sample & POS_MASK;
            int row = pos & 127, col = pos >> 7;
            int other_pos = (sample >> POS_SHIFT) & POS_MASK;
            int orow = other_pos & 127, ocol = other_pos >> 7;
            os << "(" << col << "," << row << "):(" << ocol << "," << orow << "),";
        }
        os << "}";
    }
};

}; // namespace COW

inline std::ostream& operator<<(std::ostream &os, const COW::belief_component_t &cbc) {
    cbc.print(os);
    return os;
}

#endif

struct state_t {
#ifdef USE_COW_COMPONENTS
    std::vector<COW::belief_component_t> components_;
#else
    std::vector<belief_component_t> components_;
#endif
    static int rows_;
    static int cols_;
    static int npits_;
    static int nwumpus_;

    state_t() : components_(1 + npits_ + nwumpus_) {
       components_[0].type_ = GOLD;
       for( int i = 0; i < npits_; ++i )
           components_[1+i].type_ = PIT;
       for( int i = 0; i < nwumpus_; ++i )
           components_[1+npits_+i].type_ = WUMPUS;
    }
    state_t(const state_t &bel) : components_(bel.components_) { }
    ~state_t() { }

    static void initialize(int rows, int cols, int npits, int nwumpus) {
        rows_ = rows;
        cols_ = cols;
        npits_ = npits;
        nwumpus_ = nwumpus;
    }

    unsigned hash() const {
        unsigned value = 0;
        for( unsigned i = 0; i < components_.size(); ++i )
            value = value ^ components_[i].hash();
        return value;
    }

    unsigned ub_size() const {
        unsigned size = 1;
        for( unsigned i = 0; i < components_.size(); ++i )
            size *= components_[i].size();
        return size;
    }

    void clear() {
        for( unsigned i = 0; i < components_.size(); ++i )
            components_[i].clear();
    }

    bool alive() const {
        for( unsigned i = 0; i < components_.size(); ++i ) {
            if( components_[i].empty() )
                return false;
        }
        return true;
    }

    bool safe_pos(int pos) const {
        for( unsigned i = 0; i < components_.size(); ++i ) {
            if( !components_[i].safe_pos(pos) )
                return false;
        }
        return true;
    }

    bool goal() const { return components_[0].got_gold(); }
    bool dead_end() const { return !alive(); }

    void apply(int action, state_t &result, bool non_det = false) const {
        result.clear();
        for( unsigned i = 0; i < components_.size(); ++i ) {
            if( action == MOVE_UP ) {
                components_[i].apply_move(rows_, cols_, 1, 0, result.components_[i], non_det);
            } else if( action == MOVE_RIGHT ) {
                components_[i].apply_move(rows_, cols_, 0, 1, result.components_[i], non_det);
            } else if( action == MOVE_DOWN ) {
                components_[i].apply_move(rows_, cols_, -1, 0, result.components_[i], non_det);
            } else if( action == MOVE_LEFT ) {
                components_[i].apply_move(rows_, cols_, 0, -1, result.components_[i], non_det);
            } else if( action == GRAB_GOLD ) {
                components_[i].apply_grab_gold(result.components_[i], non_det);
            }
        }
    }
    void filter(int obs, state_t &result) const {
        result.clear();
        for( unsigned i = 0; i < components_.size(); ++i ) {
            components_[i].filter(obs, result.components_[i]);
        }
    }

    const state_t& operator=(const state_t &bel) {
        for( unsigned i = 0; i < components_.size(); ++i ) {
            components_[i] = bel.components_[i];
        }
        return *this;
    }
    bool operator==(const state_t &bel) const {
        for( unsigned i = 0; i < components_.size(); ++i ) {
            if( components_[i] != bel.components_[i] )
                return false;
        }
        return true;
    }
    bool operator!=(const state_t &bel) const {
        return *this == bel ? false : true;
    }

    void print(std::ostream &os) const {
        for( unsigned i = 0; i < components_.size(); ++i ) {
            os << "comp[" << i << "]=" << components_[i] << std::endl;
        }
    }
};

int state_t::rows_ = 0;
int state_t::cols_ = 0;
int state_t::npits_ = 0;
int state_t::nwumpus_ = 0;

inline std::ostream& operator<<(std::ostream &os, const state_t &b) {
    b.print(os);
    return os;
}

struct problem_t : public Problem::problem_t<state_t> {
    int rows_;
    int cols_;
    int npits_;
    int nwumpus_;
    bool contingent_;
    bool non_det_;
    state_t init_;

    problem_t(int rows, int cols, int npits, int nwumpus, bool contingent = true, bool non_det = false)
      : Problem::problem_t<state_t>(DISCOUNT),
        rows_(rows), cols_(cols), npits_(npits), nwumpus_(nwumpus),
        contingent_(contingent), non_det_(non_det) {

        // set initial belief
        int pos = 0;

        // place gold
        for( int gold = 0; gold < rows_ * cols_; ++gold ) {
            int grow = gold / cols_, gcol = gold % cols_;
            if( (grow != 0) || (gcol != 0) ) {
                int gold_pos = (gcol << 7) | grow;
                int sample = (gold_pos << POS_SHIFT) | pos;
                init_.components_[0].insert(sample);
            }
        }

        // place pits
        for( int i = 0; i < npits_; ++i ) {
            for( int pit = 0; pit < rows_ * cols_; ++pit ) {
                int prow = pit / cols_, pcol = pit % cols_;
                if( (prow > 1) || (pcol > 1) || ((prow == 1) && (pcol == 1)) ) {
                    int pit_pos = (pcol << 7) | prow;
                    int sample = (pit_pos << POS_SHIFT) | pos;
                    init_.components_[1 + i].insert(sample);
                }
            }
            if( init_.components_[1+i].empty() ) {
                std::cout << "error: can't place pit" << std::endl;
                exit(1);
            }
        }

        // place wumpuses
        for( int i = 0; i < nwumpus_; ++i ) {
            for( int wumpus = 0; wumpus < rows_ * cols_; ++wumpus ) {
                int wrow = wumpus / cols_, wcol = wumpus % cols_;
                if( (wrow > 1) || (wcol > 1) || ((wrow == 1) && (wcol == 1)) ) {
                    int wumpus_pos = (wcol << 7) | wrow;
                    int sample = (wumpus_pos << POS_SHIFT) | pos;
                    init_.components_[1 + npits_ + i].insert(sample);
                }
            }
            if( init_.components_[1 + npits_ + i].empty() ) {
                std::cout << "error: can't place wumpus" << std::endl;
                exit(1);
            }
        }

        //std::cout << "Initial Belief:" << std::endl << init_ << std::endl;
    }
    virtual ~problem_t() { }

    virtual Problem::action_t number_actions(const state_t &s) const {
        return 5;
    }
    virtual bool applicable(const state_t &s, Problem::action_t a) const {
        return true;
    }
    virtual const state_t& init() const { return init_; }
    virtual bool terminal(const state_t &s) const {
        return s.goal();
    }
    virtual bool dead_end(const state_t &s) const {
        return s.dead_end();
    }
    virtual float cost(const state_t &s, Problem::action_t a) const {
        return 1;
    }
    virtual void next(const state_t &s,
                      Problem::action_t a,
                      std::vector<std::pair<state_t, float> > &outcomes) const {

        //std::cout << "next " << s << " w/ a=" << a << " is:" << std::endl;
        ++expansions_;
        outcomes.clear();

        state_t next;
        s.apply(a, next, non_det_);

        if( !contingent_ || (a == GRAB_GOLD) || next.dead_end() ) {
            outcomes.reserve(1);
            outcomes.push_back(std::make_pair(next, 1));
            //std::cout << "    " << next << " w.p. " << 1 << std::endl;
        } else {
            outcomes.reserve(7);
            for( int obs = 0; obs < 8; ++obs ) {
                state_t filtered;
                next.filter(obs, filtered);
                if( filtered.alive() ) {
                    outcomes.push_back(std::make_pair(filtered, 0));
                }
            }
            for( unsigned i = 0; i < outcomes.size(); ++i ) {
                outcomes[i].second = 1.0 / (float)outcomes.size();
                //std::cout << "    " << outcomes[i].first << " w.p. " << outcomes[i].second << std::endl;
            }
        }
    }
    virtual void print(std::ostream &os) const { }

    void sample_state(state_t &state) const {
        state.clear();
        int pos = 0;

        // place gold
        int gold = Random::uniform(1, rows_ * cols_);
        int grow = gold / cols_, gcol = gold % cols_;
        int gold_pos = (gcol << 7) | grow;
        int sample = (gold_pos << POS_SHIFT) | pos;
        state.components_[0].insert(sample);

        // place pits
        for( int i = 0; i < npits_; ++i ) {
            int pit = Random::uniform(rows_ * cols_);
            int prow = pit / cols_, pcol = pit % cols_;
            while( (pit == gold) ||
                   ((prow <= 1) && (pcol <= 1) && ((prow != 1) || (pcol != 1))) ) {
                pit = Random::uniform(rows_ * cols_);
                prow = pit / cols_, pcol = pit % cols_;
            }
            int pit_pos = (pcol << 7) | prow;
            int sample = (pit_pos << POS_SHIFT) | pos;
            state.components_[1 + i].insert(sample);
        }

        // place wumpuses
        for( int i = 0; i < nwumpus_; ++i ) {
            int wumpus = Random::uniform(rows_ * cols_);
            int wrow = wumpus / cols_, wcol = wumpus % cols_;
            while( (wumpus == gold) ||
                   ((wrow <= 1) && (wcol <= 1) && ((wrow != 1) || (wcol != 1))) ) {
                wumpus = Random::uniform(rows_ * cols_);
                wrow = wumpus / cols_, wcol = wumpus % cols_;
            }
            int wumpus_pos = (wcol << 7) | wrow;
            int sample = (wumpus_pos << POS_SHIFT) | pos;
            state.components_[1 + npits_ + i].insert(sample);
        }
    }

    void apply(state_t &s, state_t &hidden, Problem::action_t a) const {
        ++expansions_;
        state_t nstate, nstate_hidden;
        s.apply(a, nstate, non_det_);
        hidden.apply(a, nstate_hidden, non_det_);
        if( nstate.dead_end() || nstate_hidden.dead_end() ) {
            s.clear();
            hidden.clear();
            return;
        }
        assert(nstate_hidden.ub_size() == 1);

        if( !contingent_ ) {
            s = nstate;
            hidden = nstate_hidden;
        } else {
            //std::cout << "n-hidden=" << nstate_hidden << std::endl;
            bool found_compatible_obs = false;
            for( int obs = 0; obs < 8; ++obs ) {
                state_t filtered_hidden;
                nstate_hidden.filter(obs, filtered_hidden);
                if( filtered_hidden.alive() ) {
                    nstate.filter(obs, s);
                    assert(s.alive());
                    hidden = filtered_hidden;
                    found_compatible_obs = true;
                    break;
                } else {
                    //std::cout << "obs=" << obs << std::endl;
                    //std::cout << "fltd=" << filtered_hidden << std::endl;
                }
            }
            assert(found_compatible_obs);
        }
        assert(hidden.ub_size() == 1);
    }

};

struct probability_heuristic_t : public Heuristic::heuristic_t<state_t> {
  public:
    probability_heuristic_t() { }
    virtual ~probability_heuristic_t() { }
    virtual float value(const state_t &s) const {
        return s.components_[0].value();
    }
    virtual void reset_stats() const { }
    virtual float setup_time() const { return 0; }
    virtual float eval_time() const { return 0; }
    virtual size_t size() const { return 0; }
    virtual void dump(std::ostream &os) const { }
    float operator()(const state_t &s) const { return value(s); }
};


