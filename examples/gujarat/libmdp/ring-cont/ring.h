#include <cassert>
#include <cstring>
#include <iostream>
#include <map>
#include <vector>
#include <limits>
#include <set>

#include "algorithm.h"
#include "parameters.h"
#include "heuristic.h"

#include "policy.h"
#include "rollout.h"
#include "dispatcher.h"

#define DISCOUNT 1.00

#define USE_COW_COMPONENTS // use copy-no-write belief components

// 2 bits indicate status of component: open=0, closed=1, locked=2
// 8 bits indicate position of agent: <room>
// 8 bits indicate position of key: <room> or AGENT

#define UNKNOWN     -1
#define UNSET       -2

#define OPEN        0
#define CLOSED      1
#define LOCKED      2
#define AGENT       0xFF

#define MOVE_BWD    0
#define MOVE_FWD    1
#define CLOSE       2
#define LOCK        3
#define GRAB_KEY    4

inline unsigned rotation(unsigned x) {
    return (x << 16) | (x >> 16);
}

template<typename T> class cow_vector_t {
  private:
    mutable int nrefs_;
    int capacity_;
    int size_;
    T *vector_;

  public:
    cow_vector_t() : nrefs_(1), capacity_(0), size_(0), vector_(0) { }
    cow_vector_t(const cow_vector_t &vec)
      : nrefs_(1), capacity_(0), size_(0), vector_(0) {
        *this = vec;
    }
    ~cow_vector_t() {
        delete[] vector_;
    }

    static void deallocate(const cow_vector_t *vec) {
        assert(vec != 0);
        if( --vec->nrefs_ == 0 ) delete vec;
    }
    static cow_vector_t* ref(const cow_vector_t *vec) {
        assert(vec != 0);
        ++vec->nrefs_;
        return const_cast<cow_vector_t*>(vec);
    }

    void reserve(int ncapacity) {
        if( capacity_ < ncapacity ) {
            capacity_ = ncapacity;
            T *nvector = new T[capacity_];
            for( int i = 0; i < size_; ++i )
                nvector[i] = vector_[i];
            delete[] vector_;
            vector_ = nvector;
        }
    }

    int nrefs() const { return nrefs_; }
    void clear() { size_ = 0; }
    bool empty() const { return size_ == 0; }
    int size() const { return size_; }

    void push_back(const T &element) {
        if( size_ == capacity_ ) reserve(1 + capacity_);
        vector_[size_++] = element;
    }
    void insert(const T &element) {
        if( size_ == 0 ) {
            reserve(1);
            vector_[0] = element;
            ++size_;
        } else {
            // locate insertion point w/ binary search
            int lb = 0, ub = size_ - 1, mid = (lb + ub) / 2;
            if( element < vector_[lb] ) {
                mid = lb - 1;
            } else if( vector_[ub] <= element ) {
                mid = ub;
            } else {
                assert(vector_[lb] <= element);
                assert(element < vector_[ub]);
                while( lb != mid ) {
                    if( vector_[mid] <= element ) {
                        lb = mid;
                    } else {
                        ub = mid;
                    }
                    mid = (lb + ub) / 2;
                }
                assert(vector_[mid] <= element);
                assert(element < vector_[mid+1]);
            }

            // make insertion
            if( (mid == -1) || (vector_[mid] != element) ) {
                if( size_ == capacity_ ) reserve(1 + capacity_);
                for( int i = size_ - 1; i > mid; --i ) {
                    vector_[1+i] = vector_[i];
                }
                vector_[mid+1] = element;
                ++size_;
            }
        }
    }

    const T& operator[](int i) const { return vector_[i]; }
    cow_vector_t& operator=(const cow_vector_t &vec) {
        reserve(vec.size_);
        for( int i = 0; i < vec.size_; ++i )
            vector_[i] = vec[i];
        size_ = vec.size_;
        return *this;
    }
    bool operator==(const cow_vector_t &vec) const {
        if( size_ != vec.size_ )
            return false;
        else {
            for( int i = 0; i < size_; ++i ) {
                if( vector_[i] != vec[i] )
                    return false;
            }
            return true;
        }
    }
    bool operator!=(const cow_vector_t &vec) const {
        return *this == vec ? false : true;
    }

    struct const_iterator {
        const T *ptr_;
        const_iterator(const T *ptr) : ptr_(ptr) { }
        ~const_iterator() { }
        const_iterator& operator++() {
            ++ptr_;
            return *this;
        }
        const T& operator*() const { return *ptr_; }
        bool operator==(const const_iterator &it) const {
            return ptr_ == it.ptr_;
        }
        bool operator!=(const const_iterator &it) const {
            return ptr_ != it.ptr_;
        }
    };
    const_iterator begin() const {
        return const_iterator(vector_);
    }
    const_iterator end() const {
        return const_iterator(&vector_[size_]);
    }

};

struct cow_belief_component_t {
    cow_vector_t<int> *cow_vector_;
   
    cow_belief_component_t()
      : cow_vector_(new cow_vector_t<int>) { }
    cow_belief_component_t(const cow_belief_component_t &bc)
      : cow_vector_(ref(bc.cow_vector_)) { }
    ~cow_belief_component_t() { deallocate(cow_vector_); }

    static void deallocate(cow_vector_t<int> *vec) {
        cow_vector_t<int>::deallocate(vec);
    }
    static cow_vector_t<int>* ref(const cow_vector_t<int> *vec) {
        return cow_vector_t<int>::ref(vec);
    }

    unsigned hash() const {
        unsigned value = 0;
        for( int i = 0; i < cow_vector_->size(); ++i ) {
            int sample = (*cow_vector_)[i];
            value = value ^ (i & 0x1 ? rotation(sample) : sample);
        }
        return value;
    }

    float probability_locked() const {
        float value = 0;
        for( int i = 0; i < cow_vector_->size(); ++i ) {
            int sample = (*cow_vector_)[i];
            int status = sample & 0x3;
            value += status == LOCKED ? 1 : 0;
        }
        return value / (float) cow_vector_->size();
    }

    float key_entropy(int dim) const {
        std::vector<int> key_pos_cnt = std::vector<int>(dim, 0);
        for( int i = 0; i < cow_vector_->size(); ++i ) {
            int sample = (*cow_vector_)[i];
            int key_pos = (sample >> 2) & 0xFF;
            if( key_pos != AGENT ) ++key_pos_cnt[key_pos];
        }
        float entropy = 0;
        for( int i = 0; i < dim; ++i ) {
            float p = (float)key_pos_cnt[i] / (float)cow_vector_->size();
            entropy += p > 0 ? p * logf(p) : 0;
        }
        return -entropy;
    }

    int nrefs() const {
        return cow_vector_->nrefs();
    }
    void reserve(int cap) {
        cow_vector_->reserve(cap);
    }
    void clear() {
        if( nrefs() > 1 ) {
            deallocate(cow_vector_);
            cow_vector_ = new cow_vector_t<int>;
        } else
            cow_vector_->clear();
    }
    bool empty() const {
        return cow_vector_->empty();
    }
    int size() const {
        return cow_vector_->size();
    }
    void insert(int e) {
        if( nrefs() > 1 ) {
            deallocate(cow_vector_);
            cow_vector_t<int> *nvec = new cow_vector_t<int>(*cow_vector_);
            cow_vector_ = nvec;
        }
        cow_vector_->insert(e);
    }

    int operator[](int i) const { return (*cow_vector_)[i]; }
    const cow_belief_component_t& operator=(const cow_belief_component_t &bc) {
        deallocate(cow_vector_);
        cow_vector_ = ref(bc.cow_vector_);
        return *this;
    }
    bool operator==(const cow_belief_component_t &bc) const {
        return *cow_vector_ == *bc.cow_vector_;
    }
    bool operator!=(const cow_belief_component_t &bc) const {
        return *cow_vector_ != *bc.cow_vector_;
    }

    int window_status() const {
        int rv = UNSET;
        for( int i = 0; i < cow_vector_->size(); ++i ) {
            int sample = (*cow_vector_)[i];
            int status = sample & 0x3;
            if( rv == UNSET )
                rv = status;
            else if( rv != status )
                return UNKNOWN;
        }
        assert(rv != UNSET);
        return rv;
    }

    int key_position() const {
        int rv = UNSET;
        for( int i = 0; i < cow_vector_->size(); ++i ) {
            int sample = (*cow_vector_)[i];
            int key_pos = (sample >> 2) & 0xFF;
            if( rv == UNSET )
                rv = key_pos;
            else if( rv != key_pos )
                return UNKNOWN;
        }
        assert(rv != UNSET);
        return rv;
    }

    void apply_close(int window_pos, cow_belief_component_t &result, bool non_det = false) const {
        result.reserve(size());
        for( int i = 0; i < cow_vector_->size(); ++i ) {
            int sample = (*cow_vector_)[i];
            int status = sample & 0x3;
            int key_pos = (sample >> 2) & 0xFF;
            int agent_pos = (sample >> 10) & 0xFF;
            if( (status != LOCKED) && (agent_pos == window_pos) ) {
                result.insert(CLOSED | (key_pos << 2) | (agent_pos << 10));
            } else if( non_det && (status != LOCKED) ) {
                result.insert(OPEN | (key_pos << 2) | (agent_pos << 10));
                result.insert(CLOSED | (key_pos << 2) | (agent_pos << 10));
            } else {
                result.insert(sample);
            }
        }
    }
    void apply_lock(int window_pos, cow_belief_component_t &result, bool non_det = false) const {
        result.reserve(size());
        for( int i = 0; i < cow_vector_->size(); ++i ) {
            int sample = (*cow_vector_)[i];
            int status = sample & 0x3;
            int key_pos = (sample >> 2) & 0xFF;
            int agent_pos = (sample >> 10) & 0xFF;
            if( (status == CLOSED) && (key_pos == AGENT) && (agent_pos == window_pos) ) {
                result.insert(LOCKED | (AGENT << 2) | (agent_pos << 10));
            } else if( non_det && (status != LOCKED) ) {
                result.insert(OPEN | (key_pos << 2) | (agent_pos << 10));
                result.insert(CLOSED | (key_pos << 2) | (agent_pos << 10));
            } else {
                result.insert(sample);
            }
        }
    }
    void apply_move(int dim, int steps, cow_belief_component_t &result, bool non_det = false) const {
        result.reserve(size());
        for( int i = 0; i < cow_vector_->size(); ++i ) {
            int sample = (*cow_vector_)[i];
            int status = sample & 0x3;
            int key_pos = (sample >> 2) & 0xFF;
            int agent_pos = (sample >> 10) & 0xFF;
            int new_pos = (dim + agent_pos + steps) % dim;
            assert(new_pos >= 0);
            if( non_det && (status != LOCKED) ) {
                result.insert(OPEN | (key_pos << 2) | (new_pos << 10));
                result.insert(CLOSED | (key_pos << 2) | (new_pos << 10));
            } else {
                result.insert(status | (key_pos << 2) | (new_pos << 10));
            }
        }
    }

    // The precondition is that the agent is in the same room of the key.
    // Thus the effect has to be to hold the key.
    void apply_grab_key(cow_belief_component_t &result, bool non_det = false) const {
        result.reserve(size());
        for( int i = 0; i < cow_vector_->size(); ++i ) {
            int sample = (*cow_vector_)[i];
            int status = sample & 0x3;
            int key_pos = (sample >> 2) & 0xFF;
            int agent_pos = (sample >> 10) & 0xFF;
            assert(key_pos == agent_pos);
            if( non_det && (status != LOCKED) ) {
                result.insert(OPEN | (AGENT << 2) | (agent_pos << 10));
                result.insert(CLOSED | (AGENT << 2) | (agent_pos << 10));
            } else {
                result.insert(status | (AGENT << 2) | (agent_pos << 10));
            }
        }
    }

    void filter(bool key_in_agent_pos, cow_belief_component_t &result) const {
        result.reserve(size());
        for( int i = 0; i < cow_vector_->size(); ++i ) {
            int sample = (*cow_vector_)[i];
            int key_pos = (sample >> 2) & 0xFF;
            int agent_pos = (sample >> 10) & 0xFF;
            if( key_in_agent_pos ) {
                if( key_pos == agent_pos )
                    result.insert(sample);
            } else {
                if( key_pos != agent_pos )
                    result.insert(sample);
            }
        }
    }

    bool key_in_agent_position() const {
        for( int i = 0; i < cow_vector_->size(); ++i ) {
            int sample = (*cow_vector_)[i];
            int key_pos = (sample >> 2) & 0xFF;
            int agent_pos = (sample >> 10) & 0xFF;
            if( key_pos != agent_pos ) {
                return false;
            }
        }
        return true;
    }

    float probability_key_in_agent_position() const {
        float p = 0;
        for( int i = 0; i < cow_vector_->size(); ++i ) {
            int sample = (*cow_vector_)[i];
            int key_pos = (sample >> 2) & 0xFF;
	    int agent_pos = (sample >> 10) & 0xFF;
	    if( key_pos == agent_pos ) {
                ++p;
            }
        }
        return p / cow_vector_->size();
    }

    void print(std::ostream &os) const {
        os << "{";
        for( int i = 0; i < cow_vector_->size(); ++i ) {
            int sample = (*cow_vector_)[i];
            int status = sample & 0x3;
            int key_pos = (sample >> 2) & 0xFF;
            int agent_pos = (sample >> 10) & 0xFF;
            os << "(" << status << "," << key_pos << "," << agent_pos << "),";
        }
        os << "}";
    }
};

inline std::ostream& operator<<(std::ostream &os, const cow_belief_component_t &bc) {
    bc.print(os);
    return os;
}


struct ordered_vector_t {
    int *set_;
    int capacity_;
    int size_;
    ordered_vector_t() : set_(0), capacity_(0), size_(0) { }
    ordered_vector_t(const ordered_vector_t &vec)
      : set_(0), capacity_(0), size_(0) {
        *this = vec;
    }
    ~ordered_vector_t() { delete[] set_; }
    void reserve(int new_capacity) {
        if( capacity_ < new_capacity ) {
            capacity_ = new_capacity;
            int *nset = new int[capacity_];
            memcpy(nset, set_, size_ * sizeof(int));
            delete[] set_;
            set_ = nset;
        }
    }
    void clear() { size_ = 0; }
    bool empty() const { return size_ == 0; }
    int size() const { return size_; }
    void push_back(int e) {
        if( size_ == capacity_ ) reserve(1 + capacity_);
        set_[size_++] = e;
    }
    void insert(int e) {
        if( size_ == 0 ) {
            reserve(1);
            set_[0] = e;
            ++size_;
        } else {
            int lb = 0, ub = size_ - 1, mid = (lb + ub) / 2;
            if( e < set_[lb] ) {
                mid = lb - 1;
            } else if( set_[ub] <= e ) {
                mid = ub;
            } else {
                assert(set_[lb] <= e);
                assert(e < set_[ub]);
                while( lb != mid ) {
                    if( set_[mid] <= e ) {
                        lb = mid;
                    } else {
                        ub = mid;
                    }
                    mid = (lb + ub) / 2;
                }
                assert(set_[mid] <= e);
                assert(e < set_[mid+1]);
            }

            if( (mid == -1) || (set_[mid] != e) ) {
                if( size_ == capacity_ ) reserve(1 + capacity_);
                for( int i = size_ - 1; i > mid; --i ) {
                    set_[1+i] = set_[i];
                }
                set_[mid+1] = e;
                ++size_;
            }
        }
    }

    int operator[](int i) const { return set_[i]; }
    const ordered_vector_t& operator=(const ordered_vector_t &vec) {
        reserve(vec.size_);
        memcpy(set_, vec.set_, vec.size_ * sizeof(int));
        size_ = vec.size_;
        return *this;
    }
    bool operator==(const ordered_vector_t &vec) const {
        return size_ != vec.size_ ? false : memcmp(set_, vec.set_, size_ * sizeof(int)) == 0;
    }
    bool operator!=(const ordered_vector_t &vec) const {
        return *this == vec ? false : true;
    }

    struct const_iterator {
        const int *ptr_;
        const_iterator(const int *ptr) : ptr_(ptr) { }
        ~const_iterator() { }
        const_iterator& operator++() {
            ++ptr_;
            return *this;
        }
        int operator*() const { return *ptr_; }
        bool operator==(const const_iterator &it) const {
            return ptr_ == it.ptr_;
        }
        bool operator!=(const const_iterator &it) const {
            return ptr_ != it.ptr_;
        }
    };
    const_iterator begin() const {
        return const_iterator(set_);
    }
    const_iterator end() const {
        return const_iterator(&set_[size_]);
    }

};

struct belief_component_t : public ordered_vector_t {
   
    belief_component_t() { }
    ~belief_component_t() { }

    unsigned hash() const {
        int i = 0;
        unsigned value = 0;
        for( const_iterator it = begin(); it != end(); ++it, ++i ) {
            value = value ^ (i & 0x1 ? rotation(*it) : *it);
        }
        return value;
    }

    float probability_locked() const {
        float value = 0;
        for( const_iterator it = begin(); it != end(); ++it ) {
            int status = *it & 0x3;
            value += status == LOCKED ? 1 : 0;
        }
        return value / (float) size();
    }

    int window_status() const {
        int rv = -1;
        for( const_iterator it = begin(); it != end(); ++it ) {
            int status = *it & 0x3;
            if( rv == -1 )
                rv = status;
            else if( rv != status )
                return UNKNOWN;
        }
        assert(rv != -1);
        return rv;
    }

    void apply_close(int window_pos, belief_component_t &result, bool non_det = false) const {
        result.reserve(size());
        for( const_iterator it = begin(); it != end(); ++it ) {
            int status = *it & 0x3;
            int key_pos = ((*it) >> 2) & 0xFF;
            int agent_pos = ((*it) >> 10) & 0xFF;
            if( (status != LOCKED) && (agent_pos == window_pos) ) {
                result.insert(CLOSED | (key_pos << 2) | (agent_pos << 10));
            } else if( non_det && (status != LOCKED) ) {
                result.insert(OPEN | (key_pos << 2) | (agent_pos << 10));
                result.insert(CLOSED | (key_pos << 2) | (agent_pos << 10));
            } else {
                result.insert(*it);
            }
        }
    }
    void apply_lock(int window_pos, belief_component_t &result, bool non_det = false) const {
        result.reserve(size());
        for( const_iterator it = begin(); it != end(); ++it ) {
            int status = *it & 0x3;
            int key_pos = ((*it) >> 2) & 0xFF;
            int agent_pos = ((*it) >> 10) & 0xFF;
            if( (status == CLOSED) && (key_pos == AGENT) && (agent_pos == window_pos) ) {
                result.insert(LOCKED | (AGENT << 2) | (agent_pos << 10));
            } else if( non_det && (status != LOCKED) ) {
                result.insert(OPEN | (key_pos << 2) | (agent_pos << 10));
                result.insert(CLOSED | (key_pos << 2) | (agent_pos << 10));
            } else {
                result.insert(*it);
            }
        }
    }
    void apply_move(int dim, int steps, belief_component_t &result, bool non_det = false) const {
        result.reserve(size());
        for( const_iterator it = begin(); it != end(); ++it ) {
            int status = *it & 0x3;
            int key_pos = ((*it) >> 2) & 0xFF;
            int agent_pos = ((*it) >> 10) & 0xFF;
            int new_pos = (dim + agent_pos + steps) % dim;
            assert(new_pos >= 0);
            if( non_det && (status != LOCKED) ) {
                result.insert(OPEN | (key_pos << 2) | (new_pos << 10));
                result.insert(CLOSED | (key_pos << 2) | (new_pos << 10));
            } else {
                result.insert(status | (key_pos << 2) | (new_pos << 10));
            }
        }
    }
    void apply_grab_key(belief_component_t &result, bool non_det = false) const {
        result.reserve(size());
        for( const_iterator it = begin(); it != end(); ++it ) {
            int status = *it & 0x3;
            int key_pos = ((*it) >> 2) & 0xFF;
            int agent_pos = ((*it) >> 10) & 0xFF;
            if( key_pos == agent_pos ) {
                if( non_det && (status != LOCKED) ) {
                    result.insert(OPEN | (AGENT << 2) | (agent_pos << 10));
                    result.insert(CLOSED | (AGENT << 2) | (agent_pos << 10));
                } else {
                    result.insert(status | (AGENT << 2) | (agent_pos << 10));
                }
            } else {
                if( non_det && (status != LOCKED) ) {
                    result.insert(OPEN | (key_pos << 2) | (agent_pos << 10));
                    result.insert(CLOSED | (key_pos << 2) | (agent_pos << 10));
                } else {
                    result.insert(*it);
                }
            }
        }
    }

    void print(std::ostream &os) const {
        os << "{";
        for( const_iterator it = begin(); it != end(); ++it ) {
            int sample = *it;
            int status = sample & 0x3;
            int key_pos = (sample >> 2) & 0xFF;
            int agent_pos = (sample >> 10) & 0xFF;
            os << "(" << status << "," << key_pos << "," << agent_pos << "),";
        }
        os << "}";
    }
};

inline std::ostream& operator<<(std::ostream &os, const belief_component_t &bc) {
    bc.print(os);
    return os;
}

struct state_t {
#ifdef USE_COW_COMPONENTS
    std::vector<cow_belief_component_t> components_;
#else
    std::vector<belief_component_t> components_;
#endif
    static int dim_;

    state_t() : components_(dim_) { }
    state_t(const state_t &bel) : components_(bel.components_) { }
    ~state_t() { }

    static void initialize(int dim) {
        dim_ = dim;
    }

    unsigned hash() const {
        unsigned value = 0;
        for( int i = 0; i < dim_; ++i )
            value = value ^ components_[i].hash();
        return value;
    }

    void clear() {
        for( int i = 0; i < dim_; ++i )
            components_[i].clear();
    }

    bool empty() const {
        return components_[0].empty();
    }

    bool have_key() const {
        int key_pos = components_[0].key_position();
        return key_pos == AGENT;
    }

    bool key_in_agent_position() const {
        return components_[0].key_in_agent_position();
    }

    float probability_key_in_agent_position() const {
        return components_[0].probability_key_in_agent_position();
    }

    bool all_locked() const {
        for( int i = 0; i < dim_; ++i ) {
            if( components_[i].window_status() != LOCKED ) {
                return false;
            }
        }
        return true;
    }
    bool goal() const {
        return all_locked();
    }

    void apply(int action, state_t &result, bool non_det = false) const {
        result.clear();
        for( int i = 0; i < dim_; ++i ) {
            if( action == MOVE_BWD ) {
                components_[i].apply_move(dim_, -1, result.components_[i], non_det);
            } else if( action == MOVE_FWD ) {
                components_[i].apply_move(dim_, 1, result.components_[i], non_det);
            } else if( action == CLOSE ) {
                components_[i].apply_close(i, result.components_[i], non_det);
            } else if( action == LOCK ) {
                components_[i].apply_lock(i, result.components_[i], non_det);
            } else if( action == GRAB_KEY ) {
                components_[i].apply_grab_key(result.components_[i], non_det);
            }
        }
    }
    void filter(bool key_in_agent_pos, state_t &result) const {
        result.clear();
        for( int i = 0; i < dim_; ++i ) {
            components_[i].filter(key_in_agent_pos, result.components_[i]);
            if( result.components_[i].empty() ) {
                result.clear();
                break;
            }
        }
    }

    const state_t& operator=(const state_t &bel) {
        for( int i = 0; i < dim_; ++i ) {
            components_[i] = bel.components_[i];
        }
        return *this;
    }
    bool operator==(const state_t &bel) const {
        for( int i = 0; i < dim_; ++i ) {
            if( components_[i] != bel.components_[i] )
                return false;
        }
        return true;
    }
    bool operator!=(const state_t &bel) const {
        return *this == bel ? false : true;
    }

    void print(std::ostream &os) const {
        for( int i = 0; i < dim_; ++i ) {
            os << "comp[" << i << "]=" << components_[i] << std::endl;
        }
    }
};

int state_t::dim_ = 0;

inline std::ostream& operator<<(std::ostream &os, const state_t &b) {
    b.print(os);
    return os;
}

struct problem_t : public Problem::problem_t<state_t> {
    int dim_;
    bool non_det_;
    bool contingent_;
    state_t init_;

    problem_t(int dim, bool non_det = false, bool contingent = false)
      : Problem::problem_t<state_t>(DISCOUNT),
        dim_(dim), non_det_(non_det), contingent_(contingent) {
        // set initial belief
        for( int window = 0; window < dim_; ++window) {
            init_.components_[window].reserve(2 * dim_);
            for( int agent_pos = 0; agent_pos < dim_; ++agent_pos ) {
                for( int status = 0; status < 2; ++status ) {
                    for( int key_pos = 0; key_pos < dim_; ++key_pos ) {
                        int sample = status | (key_pos << 2) | (agent_pos << 10);
                        init_.components_[window].insert(sample);
                    }
                    //int sample = status | (AGENT << 2) | (agent_pos << 10);
                    //init_.components_[window].insert(sample);
                }
            }
        }
        //std::cout << "Initial Belief:" << std::endl << init_ << std::endl;
    }
    virtual ~problem_t() { }

    virtual Problem::action_t number_actions(const state_t &s) const {
        return 5;
    }
    virtual bool applicable(const state_t &s, Problem::action_t a) const {
        if( contingent_ ) {
            if( a == LOCK ) {
                return s.have_key();
            } else if( a == GRAB_KEY ) {
                return s.key_in_agent_position();
            } else {
                return true;
            }
        } else {
            return true;
        }
    }
    virtual const state_t& init() const { return init_; }
    virtual bool terminal(const state_t &s) const {
        return s.goal();
    }
    virtual bool dead_end(const state_t &s) const {
        return false;
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

        outcomes.reserve(1);
        state_t next;

        if( !contingent_ ) {
            s.apply(a, next, non_det_);
            outcomes.push_back(std::make_pair(next, 1));
            //std::cout << "    " << next << " w.p. " << 1 << std::endl;
        } else {
            if( (a == MOVE_FWD) || (a == MOVE_BWD) ) {
                outcomes.reserve(2);
                s.apply(a, next, non_det_);
                float p = next.probability_key_in_agent_position();
                if( p > 0 ) {
                    state_t filtered;
                    next.filter(true, filtered);
                    assert(!filtered.empty());
                    outcomes.push_back(std::make_pair(filtered, p));
                }
                if( p < 1 ) {
                    state_t filtered;
                    next.filter(false, filtered);
                    assert(!filtered.empty());
                    outcomes.push_back(std::make_pair(filtered, 1 - p));
                }
            } else {
                s.apply(a, next, non_det_);
                outcomes.push_back(std::make_pair(next, 1));
                //std::cout << "    " << next << " w.p. " << 1 << std::endl;
            }
        }
    }

    void sample_state(state_t &s) const {
        s.clear();
        int agent_pos = Random::uniform(dim_);
        int key_pos = Random::uniform(dim_);
        for( int i = 0; i < dim_; ++i ) {
            int status = Random::uniform(2);
            int sample = status | (key_pos << 2) | (agent_pos << 10);
            s.components_[i].insert(sample);
        }
    }

    void apply(state_t &s, state_t &hidden, Problem::action_t a) const {
        ++expansions_;
        state_t nstate, nstate_hidden;
        s.apply(a, nstate, non_det_);
        hidden.apply(a, nstate_hidden, non_det_);

        if( !contingent_ || ((a != MOVE_FWD) && (a != MOVE_BWD)) ) {
            s = nstate;
            hidden = nstate_hidden;
        } else {
            float p = nstate_hidden.probability_key_in_agent_position();
            assert((p == 0) || (p == 1));
            if( p == 0 ) {
                nstate.filter(false, s);
            } else {
                nstate.filter(true, s);
            }
            assert(!s.empty());
            hidden = nstate_hidden;
        }
    }

    virtual void print(std::ostream &os) const { }

};

struct cardinality_heuristic_t : public Heuristic::heuristic_t<state_t> {
  public:
    cardinality_heuristic_t() { }
    virtual ~cardinality_heuristic_t() { }
    virtual float value(const state_t &s) const {
        int value = 0;
        for( int i = 0; i < s.dim_; ++i ) {
            int compsz = s.components_[i].size();
            value = compsz > value ? compsz : value;
        }
        return value - 1;
    }
    virtual void reset_stats() const { }
    virtual float setup_time() const { return 0; }
    virtual float eval_time() const { return 0; }
    virtual size_t size() const { return 0; }
    virtual void dump(std::ostream &os) const { }
    float operator()(const state_t &s) const { return value(s); }
};

struct window_heuristic_t : public Heuristic::heuristic_t<state_t> {
  public:
    window_heuristic_t() { }
    virtual ~window_heuristic_t() { }
    virtual float value(const state_t &s) const {
        int value = 0;
        for( int i = 0; i < s.dim_; ++i ) {
            int status = s.components_[i].window_status();
            value += status != LOCKED ? 2 : 0;
        }
        return value;
    }
    virtual void reset_stats() const { }
    virtual float setup_time() const { return 0; }
    virtual float eval_time() const { return 0; }
    virtual size_t size() const { return 0; }
    virtual void dump(std::ostream &os) const { }
    float operator()(const state_t &s) const { return value(s); }
};

struct probability_heuristic_t : public Heuristic::heuristic_t<state_t> {
  public:
    probability_heuristic_t() { }
    virtual ~probability_heuristic_t() { }
    virtual float value(const state_t &s) const {
        if( s.goal() ) {
            return 0;
        } else {
            float value = 0;
            for( int i = 0; i < s.dim_; ++i ) {
                value += (1.0 - s.components_[i].probability_locked());
            }
            return value / s.dim_;
        }
    }
    virtual void reset_stats() const { }
    virtual float setup_time() const { return 0; }
    virtual float eval_time() const { return 0; }
    virtual size_t size() const { return 0; }
    virtual void dump(std::ostream &os) const { }
    float operator()(const state_t &s) const { return value(s); }
};

class fwd_random_policy_t : public Policy::policy_t<state_t> {
  public:
    fwd_random_policy_t(const Problem::problem_t<state_t> &problem)
      : Policy::policy_t<state_t>(problem) { }
    virtual ~fwd_random_policy_t() { }
    virtual Problem::action_t operator()(const state_t &s) const {
        return 1 + Random::uniform(3);
    }
    virtual const Policy::policy_t<state_t>* clone() const {
        return new fwd_random_policy_t(problem());
    }
    virtual void print_stats(std::ostream &os) const { }
};



