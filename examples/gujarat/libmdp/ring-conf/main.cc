#include <iostream>
#include <fstream>
#include <vector>

#include "ring.h"

#include "../evaluation.h"

using namespace std;

void usage(ostream &os) {
    os << "usage: ring [-a <n>] [-b <n>] [-e <f>] [-f] [-h <n>] [-s <n>] <n>"
       << endl << endl
       << "  -a <n>    Algorithm bitmask: 1=vi, 2=slrtdp, 4=ulrtdp, 8=blrtdp, 16=ilao, 32=plain-check, 64=elrtdp, 128=hdp-i, 256=hdp, 512=ldfs+, 1024=ldfs."
       << endl
       << "  -b <n>    Visits bound for blrtdp. Default: inf."
       << endl
       << "  -e <f>    Epsilon. Default: 0."
       << endl
       << "  -f        Formatted output."
       << endl
       << "  -h <n>    Heuristics: 0=zero, 1=minmin. Default: 0."
       << endl
       << "  -s <n>    Random seed. Default: 0."
       << endl
       << "  <n>       Number of windows."
       << endl << endl;
}

int main(int argc, const char **argv) {
    int dim = 0;
    bool non_det = false;
    bool contingent = false;
    unsigned bitmap = 0;
    int h = 0;
    bool formatted = false;

    string base_name;
    string policy_type;
    Evaluation::parameters_t eval_pars;

    cout << fixed;
    Algorithm::parameters_t alg_pars;

    // parse arguments
    ++argv;
    --argc;
    while( argc > 1 ) {
        if( **argv != '-' ) break;
        switch( (*argv)[1] ) {
            case 'a':
                bitmap = strtoul(argv[1], 0, 0);
                argv += 2;
                argc -= 2;
                break;
            case 'b':
                alg_pars.rtdp.bound_ = strtol(argv[1], 0, 0);
                argv += 2;
                argc -= 2;
                break;
            case 'D':
                eval_pars.evaluation_depth_ = strtoul(argv[1], 0, 0);
                argv += 2;
                argc -= 2;
                break;
            case 'c':
                contingent = true;
                ++argv;
                --argc;
                break;
            case 'e':
                alg_pars.epsilon_ = strtod(argv[1], 0);
                argv += 2;
                argc -= 2;
                break;
            case 'f':
                formatted = true;
                ++argv;
                --argc;
                break;
            case 'h':
                h = strtol(argv[1], 0, 0);
                argv += 2;
                argc -= 2;
                break;
            case 'n':
                non_det = true;
                ++argv;
                --argc;
                break;
            case 's':
                alg_pars.seed_ = strtoul(argv[1], 0, 0);
                argv += 2;
                argc -= 2;
                break;
            case 't':
                eval_pars.evaluation_trials_ = strtoul(argv[1], 0, 0);
                argv += 2;
                argc -= 2;
                break;
            default:
                usage(cout);
                exit(-1);
        }
    }

    if( argc >= 3 ) {
        dim = strtoul(argv[0], 0, 0);
        base_name = argv[1];
        policy_type = argv[2];
        if( argc >= 4 ) eval_pars.width_ = strtoul(argv[3], 0, 0);
        if( argc >= 5 ) eval_pars.depth_ = strtoul(argv[4], 0, 0);
        if( argc >= 6 ) eval_pars.par1_ = strtod(argv[5], 0);
        if( argc >= 7 ) eval_pars.par2_ = strtoul(argv[6], 0, 0);
    } else {
        usage(cout);
        exit(-1);
    }

    // build problem instances
    cout << "seed=" << alg_pars.seed_ << endl;
    Random::seeds(alg_pars.seed_);
    state_t::initialize(dim);
    problem_t problem(dim, non_det, contingent);

    // create heuristic
    Heuristic::heuristic_t<state_t> *heuristic = 0;
    if( h == 1 ) {
        heuristic = new window_heuristic_t;
    } else if( h == 2 ) {
        heuristic = new cardinality_heuristic_t;
    } else if( h == 3 ) {
        heuristic = new probability_heuristic_t;
    }

    // solve problem with algorithms
    vector<Dispatcher::result_t<state_t> > results;
    Dispatcher::solve(problem, heuristic, problem.init(), bitmap, alg_pars, results);

    // print results
    if( !results.empty() ) {
        if( formatted ) Dispatcher::print_result<state_t>(cout, 0);
        for( unsigned i = 0; i < results.size(); ++i ) {
            Dispatcher::print_result(cout, &results[i]);
        }
    }

    // evaluate policies
    vector<pair<const Policy::policy_t<state_t>*, string> > bases;

    // fill base policies
    const Problem::hash_t<state_t> *hash = results.empty() ? 0 : results[0].hash_;
    if( hash != 0 ) {
        Policy::hash_policy_t<state_t> optimal(*hash);
        bases.push_back(make_pair(optimal.clone(), "optimal"));
    }
    if( heuristic != 0 ) {
        Policy::greedy_t<state_t> greedy(problem, *heuristic);
        bases.push_back(make_pair(greedy.clone(), "greedy"));
        Policy::random_greedy_t<state_t> random_greedy(problem, *heuristic);
        bases.push_back(make_pair(random_greedy.clone(), "random-greedy"));
    }
    Policy::random_t<state_t> random(problem);
    bases.push_back(make_pair(&random, "random"));
    fwd_random_policy_t fwd_random(problem);
    bases.push_back(make_pair(&fwd_random, "fwd-random"));

    // evaluate
    pair<const Policy::policy_t<state_t>*, std::string> policy = Evaluation::select_policy(base_name, policy_type, bases, eval_pars);
    if( policy.first != 0 ) {
        pair<pair<float, float>, float> eval = Evaluation::evaluate_policy(*policy.first, eval_pars, true);
        cout << policy.second
             << "= " << setprecision(5) << eval.first.first
             << " " << eval.first.second << setprecision(2)
             << " ( " << eval.second << " secs "
             << policy.first->decisions() << " decisions)" << std::endl;
        policy.first->print_stats(cout);
    } else {
        cout << "error: " << policy.second << endl;
    }

    // free resources
    delete policy.first;
    for( unsigned i = 0; i < results.size(); ++i ) {
        delete results[i].hash_;
    }
    delete heuristic;

    exit(0);
}

