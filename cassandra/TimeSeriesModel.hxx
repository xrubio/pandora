
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

    // _params is filled with a set of runs, each with a vector of floats (one for each param)
    void fillParams( const std::string & groupFile );
    // _results is filled with a set of runs, each with a vector of floats (each one having the list of temporal values for each result)
    void fillResults( const std::string & file );

    void fillParamNames( const std::string & groupFile );
    void fillResultNames( const std::string & file );
    void fillTimeSteps( const std::string & file );

    size_t _selectedResult;
    // for each param the list of selected values (used for means)
    std::vector< std::vector<float> > _selectedValues;

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
    // returns the value of param for a given run and timeStep
    float results( const size_t & run, const size_t & resultIndex, const int & timeStep) const;

    float mean( int step ) const;
    float selectedMean( int step ) const;

    void selectResult( int index );
    float maxResultValue() const;
    float minResultValue() const;
    bool isEmpty() const;
    void setSelectedValues( int paramIndex, const std::vector<float> & selectedValues );
    bool isParamSelected( int paramIndex, float value ) const;
};

}

#endif // __TimeSeriesModel_hxx__

