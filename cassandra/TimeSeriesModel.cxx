
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

void TimeSeriesModel::fillParams( const std::string & groupFile )
{
    std::cout << "fill params from: " << groupFile << std::endl;
    setlocale ( LC_NUMERIC, "C" );
    std::ifstream infile(groupFile.c_str());
    std::string line;

    // header
    std::getline(infile, line);

    std::vector<std::string> tokens;
    std::istringstream iss(line);
    std::string token;        
    
    // iterate through first line
    _params.clear();
    // first is run name
    std::getline(iss, token, ';');
    while(std::getline(iss, token, ';'))
    {    
        bool isParam = true;
        // remove params that are in results (they are NOT params)
        for(auto result : _results)
        {
            if(result==token)
            {
                isParam = false;
                break;
            }
        }
        if(isParam)
        {
            _params.push_back(token);
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

void TimeSeriesModel::fillResults( const std::string & file )
{
    std::cout << "fill results from: " << file << std::endl;
    setlocale ( LC_NUMERIC, "C" );
    std::ifstream infile(file.c_str());
    std::string line;

    // header
    std::getline(infile, line);

    std::vector<std::string> tokens;
    std::istringstream iss(line);
    std::string token;        
    
    _results.clear();
    // timeStep is always the first param
    std::getline(iss, token, ';');
    while(std::getline(iss, token, ';'))
    {
        _results.push_back(token);
    }
}

void TimeSeriesModel::loadGroupFile( const std::string & groupFile )
{
    std::cout << "loading group file: " << groupFile << std::endl;

    size_t pos = groupFile.rfind("/");
    std::string dir = groupFile.substr(0, pos);

    _params.clear();
    _results.clear();
    _values.clear();

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
                fillResults(oss.str());
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
    fillParams(groupFile);
}

const std::vector<std::string> & TimeSeriesModel::params() const
{
    return _params;
}
 
const std::vector<std::string> & TimeSeriesModel::results() const
{
    return _results;
}
 
const std::vector<int> & TimeSeriesModel::timeSteps() const
{
    return _timeSteps;
}


} // namespace GUI

