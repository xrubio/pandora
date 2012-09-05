/*
 *  Copyright (C) 2012 Universidad Simon Bolivar
 * 
 *  Permission is hereby granted to distribute this software for
 *  non-commercial research purposes, provided that this copyright
 *  notice is included with any such distribution.
 *  
 *  THIS SOFTWARE IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 *  EITHER EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE.  THE ENTIRE RISK AS TO THE QUALITY AND PERFORMANCE OF THE
 *  SOFTWARE IS WITH YOU.  SHOULD THE PROGRAM PROVE DEFECTIVE, YOU
 *  ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR CORRECTION.
 *  
 *  Blai Bonet, bonet@ldc.usb.ve
 *
 */

#include <cassert>
#include <iostream>

namespace COW {

inline unsigned rotation(unsigned x) {
    return (x << 16) | (x >> 16);
}

template<typename T> class vector_t {
  private:
    mutable int nrefs_;
    int capacity_;
    int size_;
    T *vector_;

  public:
    vector_t() : nrefs_(1), capacity_(0), size_(0), vector_(0) { }
    vector_t(const vector_t &vec)
      : nrefs_(1), capacity_(0), size_(0), vector_(0) {
        *this = vec;
    }
    virtual ~vector_t() {
        delete[] vector_;
    }

    static void deallocate(const vector_t *vec) {
        assert(vec != 0);
        if( --vec->nrefs_ == 0 ) delete vec;
    }
    static vector_t* ref(const vector_t *vec) {
        assert(vec != 0);
        ++vec->nrefs_;
        return const_cast<vector_t*>(vec);
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

    unsigned hash() const {
        unsigned value = 0;
        for( int i = 0; i < size_; ++i ) {
            value = value ^ (i & 0x1 ? rotation(vector_[i]) : vector_[i]);
        }
        return value;
    }

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
    vector_t& operator=(const vector_t &vec) {
        reserve(vec.size_);
        for( int i = 0; i < vec.size_; ++i )
            vector_[i] = vec[i];
        size_ = vec.size_;
        return *this;
    }
    bool operator==(const vector_t &vec) const {
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
    bool operator!=(const vector_t &vec) const {
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

    void print(std::ostream &os) const {
        os << "{";
        for( int i = 0; i < size_; ++i ) {
            os << vector_[i] << ",";
        }
        os << "}";
    }

};

}; // namespace

template<typename T> inline
std::ostream& operator<<(std::ostream &os, const COW::vector_t<T> &vec) {
    vec.print(os);
    return os;
}

