
#include <TimeSeriesModel.hxx>
#include <boost/filesystem.hpp>
#include <sstream>
#include <fstream>
#include <iomanip>

namespace GUI
{

TimeSeriesModel::TimeSeriesModel()
{
}

TimeSeriesModel::~TimeSeriesModel()
{
}

void TimeSeriesModel::fillParamNames( const std::string & groupFile )
{
    setlocale ( LC_NUMERIC, "C" );
    std::ifstream infile(groupFile.c_str());
    std::string line;

    // header
    std::getline(infile, line);

    std::vector<std::string> tokens;
    std::istringstream iss(line);
    std::string token;        
    
    // iterate through first line
    _paramNames.clear();
    // first is run name
    std::getline(iss, token, ';');
    while(std::getline(iss, token, ';'))
    {    
        bool isParamName = true;
        // remove params that are in results (they are NOT params)
        for(auto resultName : _resultNames)
        {
            if(resultName==token)
            {
                isParamName = false;
                break;
            }
        }
        if(isParamName)
        {
            _paramNames.push_back(token);
        }
    }
}

void TimeSeriesModel::fillTimeSteps( const std::string & file )
{   
    _timeSteps.clear();
    setlocale ( LC_NUMERIC, "C" );
    std::ifstream infile(file.c_str());
    std::string line;

    // header
    std::getline(infile, line);
    while(std::getline(infile, line))
    {
        std::istringstream iss(line);
        std::string token;
        // timeStep is always the first param
        std::getline(iss, token, ';');
        _timeSteps.push_back(atoi(token.c_str()));
        continue;
    }
}

void TimeSeriesModel::fillResultNames( const std::string & file )
{
    setlocale ( LC_NUMERIC, "C" );
    std::ifstream infile(file.c_str());
    std::string line;

    // header
    std::getline(infile, line);

    std::vector<std::string> tokens;
    std::istringstream iss(line);
    std::string token;        
    
    _resultNames.clear();
    // timeStep is always the first param
    std::getline(iss, token, ';');
    while(std::getline(iss, token, ';'))
    {
        _resultNames.push_back(token);
    }
}

void TimeSeriesModel::loadGroupFile( const std::string & groupFile )
{
    std::cout << "loading group file: " << groupFile << std::endl;

    size_t pos = groupFile.rfind("/");
    std::string dir = groupFile.substr(0, pos);

    _paramNames.clear();
    _resultNames.clear();
    _results.clear();

    int i=0;
    bool finished = false;
    while(!finished)
    {
        std::stringstream oss;
		oss << dir << "/run_"  << std::setfill('0') << std::setw(4) << i <<  ".csv";
        if(boost::filesystem::exists(oss.str()))
        {
            if(_results.empty())
            {
                fillResultNames(oss.str());
                fillTimeSteps(oss.str());
            }
        }
        else
        {
            finished = true;
        }
        i++;
    }
    std::cout << i << " results files parsed" << std::endl; 
    fillParamNames(groupFile);
}

const std::vector<std::string> & TimeSeriesModel::paramNames() const
{
    return _paramNames;
}
 
const std::vector<std::string> & TimeSeriesModel::resultNames() const
{
    return _resultNames;
}
 
const std::vector<int> & TimeSeriesModel::timeSteps() const
{
    return _timeSteps;
}

size_t TimeSeriesModel::numRuns() const
{
    return 0;
}
    
float TimeSeriesModel::params( const size_t & run, const size_t & paramIndex ) const
{
    return 0;
}

float TimeSeriesModel::results( const size_t & run, const size_t & resultIndex, const size_t & timeStep ) const
{
    return 0;
}

} // namespace GUI

