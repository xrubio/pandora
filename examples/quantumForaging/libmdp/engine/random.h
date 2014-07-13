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

#ifndef RANDOM_H
#define RANDOM_H

#include <iostream>
#include <cassert>
#include <stdlib.h>

//#define DEBUG

namespace Random {

inline void set_seed(int seed) {
    unsigned short useed[3];
    useed[0] = useed[1] = useed[2] = seed;
    srand48((long int)seed);
    seed48(useed);
}

inline float _random_float() {
    float d = drand48();
#ifdef DEBUG
    std::cerr << "_random_float: " << d << std::endl;
#endif
    return d;
}

inline unsigned _random_unsigned() {
    int r = lrand48();
#ifdef DEBUG
    std::cerr << "_random_unsigned: " << r << std::endl;
#endif
    return r;
}

inline float real() {
    return _random_float();
}

inline unsigned uniform(unsigned max) {
    assert(max > 0);
    return max == 1 ? 0 : _random_unsigned() % max;
}

inline unsigned uniform(unsigned min, unsigned max) {
    assert(max - min > 0);
    return min + uniform(max - min);
}

};

#undef DEBUG

#endif

