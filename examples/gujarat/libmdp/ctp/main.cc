#include <iostream>
#include <fstream>
#include <vector>

#include "ctp.h"

#include "../evaluation.h"

using namespace std;

void usage(ostream &os) {
    os << "usage: ctp [-a <n>] [-b <n>] [-e <f>] [-f] [-g <f>] [-h <n>] [-s <n>] <file>"
       << endl << endl
       << "  -a <n>    Algorithm bitmask: 1=vi, 2=slrtdp, 4=ulrtdp, 8=blrtdp, 16=ilao, 32=plain-check, 64=elrtdp, 128=hdp-i, 256=hdp, 512=ldfs+, 1024=ldfs."
       << endl
       << "  -b <n>    Visits bound for blrtdp. Default: inf."
       << endl
       << "  -e <f>    Epsilon. Default: 0."
       << endl
       << "  -f        Formatted output."
       << endl
       << "  -g <f>    Parameter for epsilon-greedy. Default: 0."
       << endl
       << "  -h <n>    Heuristics: 0=zero, 1=minmin. Default: 0."
       << endl
#if 0
       << "  -k <n>    Kappa consistency level. Default: 0."
       << endl
       << "  -K <f>    Used to define kappa measures. Default: 2."
       << endl
#endif
       << "  -s <n>    Random seed. Default: 0."
       << endl
       << "  <file>    Racetrack file."
       << endl << endl;
}

int main(int argc, const char **argv) {
    unsigned bitmap = 0;
    int h = 0;
    bool formatted = false;
    int shortcut_cost = 200;

    string base_name;
    string policy_type;
    Evaluation::parameters_t par;

    cout << fixed;
    Algorithm::parameters_t parameters;

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
                parameters.rtdp.bound_ = strtol(argv[1], 0, 0);
                argv += 2;
                argc -= 2;
                break;
            case 'c':
                shortcut_cost = strtol(argv[1], 0, 0);
                argv += 2;
                argc -= 2;
                break;
            case 'e':
                parameters.epsilon_ = strtod(argv[1], 0);
                argv += 2;
                argc -= 2;
                break;
            case 'f':
                formatted = true;
                ++argv;
                --argc;
                break;
            case 'g':
                parameters.rtdp.epsilon_greedy_ = strtod(argv[1], 0);
                argv += 2;
                argc -= 2;
                break;
            case 'h':
                h = strtol(argv[1], 0, 0);
                argv += 2;
                argc -= 2;
                break;
            case 's':
                parameters.seed_ = strtoul(argv[1], 0, 0);
                argv += 2;
                argc -= 2;
                break;
            default:
                usage(cout);
                exit(-1);
        }
    }

    CTP::graph_t graph(false, shortcut_cost);
    if( argc >= 3 ) {
        ifstream is(argv[0], ifstream::in);
        if( !graph.parse(is) ) exit(-1);
        is.close();
        base_name = argv[1];
        policy_type = argv[2];
        if( argc >= 4 ) par.width_ = strtoul(argv[3], 0, 0);
        if( argc >= 5 ) par.depth_ = strtoul(argv[4], 0, 0);
        if( argc >= 6 ) par.par1_ = strtod(argv[5], 0);
        if( argc >= 7 ) par.par2_ = strtoul(argv[6], 0, 0);
    } else {
        usage(cout);
        exit(-1);
    }

    // build problem instances
    cout << "seed=" << parameters.seed_ << endl;
    Random::seeds(parameters.seed_);
    problem_t problem(graph);
    //cout << "P(bad weather)=" << probability_bad_weather(graph, 1e6) << endl;

    // create heuristic
    Heuristic::heuristic_t<state_t> *heuristic = 0;
    if( h == 1 ) {
        heuristic = new Heuristic::min_min_heuristic_t<state_t>(problem);
    } else if( h == 2 ) {
        //heuristic = new Heuristic::hdp_heuristic_t<state_t>(problem, eps, 0);
    }

    // solve problem with algorithms
    vector<Dispatcher::result_t<state_t> > results;
    Dispatcher::solve(problem, heuristic, problem.init(), bitmap, parameters, results);

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
    }
    Policy::random_t<state_t> random(problem);
    bases.push_back(make_pair(&random, "random"));

    // evaluate
    pair<const Policy::policy_t<state_t>*, std::string> policy = Evaluation::select_policy(base_name, policy_type, bases, par);
    cout << policy.second << "= " << flush;

    problem_with_hidden_state_t pwhs(graph);
    vector<int> distances;
    vector<float> values;
    values.reserve(par.evaluation_trials_);
    float start_time = Utils::read_time_in_seconds();
    for( unsigned i = 0; i < par.evaluation_trials_; ++i ) {
        // sample a good weather
        state_t hidden = sample_weather(graph);
        hidden.preprocess(graph);
        while( hidden.is_dead_end_ ) {
            hidden = sample_weather(graph);
            hidden.compute_distances(graph, distances);
            hidden.preprocess(graph);
        }
        if( graph.with_shortcut_ ) hidden.set(graph.num_edges_ - 1, 0);
        pwhs.set_hidden(hidden);

        // do evaluation from start node
        size_t steps = 0;
        float cost = 0;
        state_t state = pwhs.init();
        while( (steps < par.evaluation_depth_) && !pwhs.terminal(state) ) {
            Problem::action_t action = (*policy.first)(state);
            assert(action != Problem::noop);
            assert(problem.applicable(state, action));
            std::pair<state_t, bool> p = pwhs.sample(state, action);
#if 0
            if( pwhs.cost(state, action) == shortcut_cost ) {
                cout << "large cost" << endl;
            }
#endif
            cost += pwhs.cost(state, action);
            state = p.first;
            ++steps;
        }
        values.push_back(cost);
    }

    // compute avg
    float avg = 0;
    for( unsigned i = 0; i < par.evaluation_trials_; ++i ) {
        avg += values[i];
    }
    avg /= par.evaluation_trials_;

    // compute stdev
    float stdev = 0;
    for( unsigned i = 0; i < par.evaluation_trials_; ++i ) {
        stdev += (avg - values[i]) * (avg - values[i]);
    }
    stdev = sqrt(stdev) / (par.evaluation_trials_ - 1);

    cout << setprecision(5) << avg << " " << stdev << setprecision(2)
         << " ( " << Utils::read_time_in_seconds() - start_time << " secs)" << endl;

    // free resources
    delete policy.first;
    for( unsigned i = 0; i < results.size(); ++i ) {
        delete results[i].hash_;
    }
    delete heuristic;

    exit(0);
}

