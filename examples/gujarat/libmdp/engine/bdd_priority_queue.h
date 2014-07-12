/*
 *  Copyright (C) 2011 Universidad Simon Bolivar
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

#ifndef BDD_PRIORITY_QUEUE_H
#define BDD_PRIORITY_QUEUE_H

#include <iostream>
#include <cassert>
#include <cstring>
#include <vector>

//#define DEBUG

namespace std {

template<typename T, typename MAX_CMP_FN, typename MIN_CMP_FN> class bdd_priority_queue {
  protected:
    mutable MAX_CMP_FN max_cmpfn_;
    mutable MIN_CMP_FN min_cmpfn_;

    struct container_t {
        T element_;
        unsigned xref_;
        container_t *next_;
        container_t() : xref_(0), next_(0) { }
        container_t(const T &element) : element_(element), xref_(0), next_(0) { }
    };
    typedef bdd_priority_queue<T, MAX_CMP_FN, MIN_CMP_FN> base;

    const unsigned capacity_;
    mutable container_t *pool_;
    mutable T removed_element_;

    unsigned max_size_;
    vector<container_t*> max_array_;

    unsigned min_size_;
    vector<unsigned> min_array_;

    container_t* allocate_container(const T &element) const {
        if( pool_ == 0 ) {
            return new container_t(element);
        } else {
            container_t *p = pool_;
            pool_ = pool_->next_;
            p->element_ = element;
            p->next_ = 0;
            p->xref_ = 0;
            return p;
        }
    }

    bool is_leaf_in_max(unsigned index) const { return (index << 1) > max_size_; }
    bool is_leaf_in_min(unsigned index) const { return (index << 1) > min_size_; }

    bool check_max_heap_property(unsigned index) const {
        unsigned left = index << 1, right = 1 + left;
        if( (left <= max_size_) && max_cmpfn_(max_array_[index]->element_, max_array_[left]->element_) ) {
            return false;
        }
        if( (right <= max_size_) && max_cmpfn_(max_array_[index]->element_, max_array_[right]->element_) ) {
            return false;
        }
        return true;
    }
    bool check_max_invariant(unsigned index) const { return index == min_array_[max_array_[index]->xref_]; }
    bool check_max() const {
        // check heap property
        for( unsigned index = 1; index <= max_size_; ++index ) {
            if( !check_min_heap_property(index) ) {
                std::cout << "heap property violated at max_array[index=" << index << "]" << std::endl;
                return false;
            }
        }

        // check invariants for xrefs
        for( unsigned index = 1; index <= max_size_; ++index ) {
            if( !check_max_invariant(index) ) {
                std::cout << "invariant violated for max_array[index=" << index << "]" << std::endl;
                return false;
            }
        }

        // all tests passed, declare it sound
        return true;
    }

    bool check_min_heap_property(unsigned index) const {
        unsigned left = index << 1, right = 1 + left;
        if( (left <= min_size_) &&
            min_cmpfn_(max_array_[min_array_[index]]->element_, max_array_[min_array_[left]]->element_) ) {
            return false;
        }
        if( (right <= min_size_) &&
            min_cmpfn_(max_array_[min_array_[index]]->element_, max_array_[min_array_[right]]->element_) ) {
            return false;
        }
        return true;
    }
    bool check_min_invariant(unsigned index) const { return index == max_array_[min_array_[index]]->xref_; }
    bool check_min() const {
        // check heap property in min_array
        for( unsigned index = 1; index <= min_size_; ++index ) {
            if( !check_min_heap_property(index) ) {
                std::cout << "heap property violated at min_array[index=" << index << "]" << std::endl;
                return false;
            }
        }

        // check invariants for xrefs in min_array
        for( unsigned index = 1; index <= min_size_; ++index ) {
            if( !check_min_invariant(index) ) {
                std::cout << "invariant violated for min_array[index=" << index << "]" << std::endl;
                return false;
            }
        }

        // all tests passed, declare it sound
        return true;
    }

    bool check() const { return check_max() && check_min(); }

    void push_max(unsigned index, container_t *container) {
        unsigned parent = index >> 1;
        while( (index > 1) && max_cmpfn_(max_array_[parent]->element_, container->element_) ) {
            max_array_[index] = max_array_[parent];
            min_array_[max_array_[index]->xref_] = index;
            index = parent;
            parent = parent >> 1;
        }
        max_array_[index] = container;

        if( container->xref_ != 0 ) {
            // container is not new; it is changing location in max_array
            min_array_[container->xref_] = index;
        } else {
            push_min(1 + min_size_, index);
            ++min_size_;
        }
    }

    unsigned max_subtree_leaf(unsigned index) const {
        while( !is_leaf_in_max(index) ) index = index << 1;
        return index;
    }
    void remove_from_max(unsigned index) {
        if( !is_leaf_in_max(index) ) {
            unsigned leaf = max_subtree_leaf(index);
            assert(is_leaf_in_max(leaf));
            assert(!max_cmpfn_(max_array_[index]->element_, max_array_[leaf]->element_));
            assert(!min_cmpfn_(max_array_[index]->element_, max_array_[leaf]->element_));
            assert(max_array_[index]->xref_ == 1);
            max_array_[index]->xref_ = max_array_[leaf]->xref_;
            min_array_[max_array_[index]->xref_] = index;
            index = leaf;
        }

        // link pool of available containers
        max_array_[index]->next_ = pool_;
        pool_ = max_array_[index];

        // remove container from heap
        if( index != max_size_ ) {
            push_max(index, max_array_[max_size_]);
        }
        --max_size_;
        assert(check());
    }

    void heapify_max(unsigned index) {
        assert((index > 0) && (index <= max_size_));
        unsigned left = index << 1, right = 1 + left, largest = 0;

        if( (left <= max_size_) && max_cmpfn_(max_array_[index]->element_, max_array_[left]->element_) ) {
            largest = left;
        } else {
            largest = index;
        }
        if( (right <= max_size_) && max_cmpfn_(max_array_[largest]->element_, max_array_[right]->element_) ) {
            largest = right;
        }

        if( largest != index ) {
            container_t *tmp = max_array_[largest];
            max_array_[largest] = max_array_[index];
            max_array_[index] = tmp;
            min_array_[max_array_[largest]->xref_] = largest;
            min_array_[max_array_[index]->xref_] = index;
            heapify_max(largest);
        }
    }

    void push_min(unsigned index, unsigned element) {
        unsigned parent = index >> 1;
        while( (index > 1) && min_cmpfn_(max_array_[min_array_[parent]]->element_, max_array_[element]->element_) ) {
            min_array_[index] = min_array_[parent];
            max_array_[min_array_[index]]->xref_ = index;
            index = parent;
            parent = parent >> 1;
        }
        min_array_[index] = element;
        max_array_[element]->xref_ = index;
    }

    unsigned min_subtree_leaf(unsigned index) const {
        while( !is_leaf_in_min(index) ) index = index << 1;
        return index;
    }
    void remove_from_min(unsigned index) {
        if( !is_leaf_in_min(index) ) {
            unsigned leaf = min_subtree_leaf(index);
            assert(is_leaf_in_min(leaf));
            assert(!max_cmpfn_(max_array_[min_array_[index]]->element_, max_array_[min_array_[leaf]]->element_));
            assert(!min_cmpfn_(max_array_[min_array_[index]]->element_, max_array_[min_array_[leaf]]->element_));
            max_array_[min_array_[index]]->xref_ = leaf;
            max_array_[min_array_[leaf]]->xref_ = index;
            unsigned aux_index = min_array_[index];
            min_array_[index] = min_array_[leaf];
            min_array_[leaf] = aux_index;
            index = leaf;
        }

        if( index != min_size_ ) {
            push_min(index, min_array_[min_size_]);
        }
        --min_size_;
        assert(check_min());
    }

    void heapify_min(unsigned index) {
        assert((index > 0) && (index <= min_size_));
        unsigned left = index << 1, right = 1 + left, largest = 0;

        if( (left <= min_size_) &&
            min_cmpfn_(max_array_[min_array_[index]]->element_, max_array_[min_array_[left]]->element_) ) {
            largest = left;
        } else {
            largest = index;
        }
        if( (right <= min_size_) &&
            min_cmpfn_(max_array_[min_array_[largest]]->element_, max_array_[min_array_[right]]->element_) ) {
            largest = right;
        }

        if( largest != index ) {
            unsigned tmp = min_array_[largest];
            min_array_[largest] = min_array_[index];
            min_array_[index] = tmp;
            max_array_[min_array_[largest]]->xref_ = largest;
            max_array_[min_array_[index]]->xref_ = index;
            heapify_min(largest);
        }
    }

  public:
    bdd_priority_queue(unsigned capacity)
      : capacity_(capacity), pool_(0), max_size_(0), min_size_(0) {
        max_array_ = vector<container_t*>(1+capacity_, 0);
        min_array_ = vector<unsigned>(1+capacity_, 0);
        clear();
    }
    ~bdd_priority_queue() {
        clear();
        while( pool_ != 0 ) {
            container_t *p = pool_->next_;
            delete pool_;
            pool_ = p;
        }
    }

    unsigned capacity() const { return capacity_; }
    unsigned size() const { return max_size_; }
    bool empty() const { return max_size_ == 0; }
    T removed_element() const { return removed_element_; }

    void clear() {
        for( unsigned index = 1; index <= max_size_; ++index ) {
            max_array_[index]->next_ = pool_;
            pool_ = max_array_[index];
            max_array_[index] = 0;
        }
        max_size_ = 0;

        memset(&min_array_[0], 0, (1 + capacity_) * sizeof(unsigned));
        min_size_ = 0;

        assert(check());
    }

    const T top() const {
        return max_array_[1]->element_;
    }

    std::pair<bool, bool> push(T &element) {
        if( max_size_ < capacity_ ) {
            container_t *container = allocate_container(element);
            push_max(1 + max_size_, container);
            ++max_size_;
            assert(check());
            return std::make_pair(true, false);
        } else {
            unsigned min = min_array_[1];
            if( max_cmpfn_(max_array_[min]->element_, element) ) {
                min_array_[1] = min_array_[min_size_];
                max_array_[min_array_[1]]->xref_ = 1;
                --min_size_;
                if( min_size_ > 0 ) {
                    heapify_min(1);
                }

                remove_from_max(min);
                assert(pool_ != 0);
                removed_element_ = pool_->element_;

                assert(max_size_ < capacity_);
                push(element);
                return std::make_pair(true, true);
            } else {
                return std::make_pair(false, false);
            }
        }
    }

    void pop() {
        assert(max_size_ > 0);
        remove_from_min(max_array_[1]->xref_);
        max_array_[1]->next_ = pool_;
        pool_ = max_array_[1];
        max_array_[1] = max_array_[max_size_];
        min_array_[max_array_[1]->xref_] = 1;
        --max_size_;

        if( max_size_ > 0 ) {
            heapify_max(1);
        }

        assert(check());
    }
};

}; // end of namespace

#undef DEBUG

#endif

