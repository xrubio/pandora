
#ifndef __TimeSeriesModel_hxx__
#define __TimeSeriesModel_hxx__

#include <vector>
#include <string>

namespace GUI
{

class TimeSeriesModel
{
    std::vector<std::string> _paramNames;
    std::vector<std::string> _resultNames;
    std::vector<int> _timeSteps;
    // vector of runs, each with a vector of param values
    std::vector< std::vector<float> > _params;
    // vector of runs, each with a vector of results for each time step
    std::vector< std::vector< std::vector<float> > > _results;

    void fillParamNames( const std::string & groupFile );
    void fillResultNames( const std::string & file );
    void fillTimeSteps( const std::string & file );

public:
    TimeSeriesModel();
    virtual ~TimeSeriesModel();

    void loadGroupFile( const std::string & groupFile );
    const std::vector<std::string> & paramNames() const;
    const std::vector<std::string> & resultNames() const;
    const std::vector<int> & timeSteps() const;

    size_t numRuns() const;
    // returns the value of a param for a given run
    float params( const size_t & run, const size_t & paramIndex ) const;
    float results( const size_t & run, const size_t & resultIndex, const size_t & timeStep ) const;
};

}

#endif // __TimeSeriesModel_hxx__

