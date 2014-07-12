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

#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <limits>

namespace Algorithm {

struct parameters_t {
    float epsilon_;
    unsigned long seed_;

    struct vi_parameters_t {
        unsigned max_number_iterations_;
        vi_parameters_t()
          : max_number_iterations_(std::numeric_limits<unsigned>::max()) { }
    } vi;

    struct rtdp_parameters_t {
        unsigned bound_;
        unsigned max_number_steps_;
        float epsilon_greedy_;
        rtdp_parameters_t()
          : bound_(std::numeric_limits<unsigned>::max()),
            max_number_steps_(std::numeric_limits<unsigned>::max()),
            epsilon_greedy_(0) { }
    } rtdp;

    struct simple_bfs_parameters_t {
        bool pure_heuristic_search_;
        simple_bfs_parameters_t() : pure_heuristic_search_(false) { }
    } simple_bfs;

    struct eval_parameters_t {
        unsigned number_trials_;
        unsigned max_number_steps_;
        bool verbose_;
        eval_parameters_t()
          : number_trials_(1000),
            max_number_steps_(std::numeric_limits<unsigned>::max()),
            verbose_(false) { }
    } eval;

    parameters_t() : epsilon_(0), seed_(0) { }
};

}; // end of namespace Algorithm


namespace Online {

namespace Evaluation {

struct parameters_t {
    unsigned evaluation_trials_;
    unsigned evaluation_depth_;
    unsigned width_;
    unsigned depth_;
    float par1_;
    unsigned par2_;
    unsigned labeling_;
    parameters_t()
      : evaluation_trials_(1000), evaluation_depth_(100),
        width_(0), depth_(0), par1_(0), par2_(0), labeling_(false) { }
};

}; // namespace Evaluation

}; // namespace Online

#endif

