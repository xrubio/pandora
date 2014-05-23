
#ifndef __TimeSeriesModel_hxx__
#define __TimeSeriesModel_hxx__

#include <vector>
#include <string>

namespace GUI
{

class TimeSeriesModel
{
    std::vector<std::string> _params;
    std::vector<std::string> _results;
    std::vector<int> _timeSteps;
    std::vector< std::vector<float> > _values;

    void fillParams( const std::string & groupFile );
    void fillResults( const std::string & file );
    void fillTimeSteps( const std::string & file );

public:
    TimeSeriesModel();
    virtual ~TimeSeriesModel();

    void loadGroupFile( const std::string & groupFile );
    const std::vector<std::string> & params() const;
    const std::vector<std::string> & results() const;
    const std::vector<int> & timeSteps() const;
};

}

#endif // __TimeSeriesModel_hxx__

