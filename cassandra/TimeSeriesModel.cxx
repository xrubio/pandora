
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
    _selectedValues.resize(_paramNames.size());
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

    if(!std::getline(infile, line))
    {
        return;
    }

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

void TimeSeriesModel::fillParams( const std::string & groupFile )
{
    setlocale ( LC_NUMERIC, "C" );
    std::ifstream infile(groupFile.c_str());
    std::string line;

    if(!std::getline(infile, line))
    {
        return;
    }
    std::cout << "first line: " << line << std::endl;
    // each line is a run
    // the order is always 'run name;param 1;param2;param 3;final result 1; final result 2;...
    while(std::getline(infile, line))
    {
        if(line.empty())
        {
            continue;
        }
        std::vector<float> run;
        std::cout << "line: " << line << std::endl;

        std::istringstream iss(line);
        std::string token;        
        std::getline(iss, token, ';');
        for(size_t i=0; i<_paramNames.size(); i++)
        {
            std::getline(iss, token, ';');
            run.push_back(std::atof(token.c_str()));
        }
        _params.push_back(run);
    }
}

void TimeSeriesModel::fillResults( const std::string & file )
{   
    std::vector< std::vector<float> > run;
    run.resize(_resultNames.size());

    setlocale ( LC_NUMERIC, "C" );
    std::ifstream infile(file.c_str());
    std::string line;

    // header
    std::getline(infile, line);
    // each line is a time step
    while(std::getline(infile, line))
    {
        std::istringstream iss(line);
        std::string token;
        // time step ignored
        std::getline(iss, token, ';');
        for(size_t i=0; i<_resultNames.size(); i++)
        {
            std::getline(iss, token, ';');
            run.at(i).push_back(std::atof(token.c_str()));
        }
    }
    _results.push_back(run);
}

void TimeSeriesModel::loadGroupFile( const std::string & groupFile )
{
    std::cout << "loading group file: " << groupFile << std::endl;

    size_t pos = groupFile.rfind("/");
    std::string dir = groupFile.substr(0, pos);

    _paramNames.clear();
    _resultNames.clear();

    _params.clear();
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
            fillResults(oss.str());
        }
        else
        {
            finished = true;
        }
        i++;
    }
    fillParamNames(groupFile);
    fillParams(groupFile);
    std::cout << "finished loading: " << numRuns() << " runs" << std::endl;
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
    return _params.size();
}
    
float TimeSeriesModel::params( const size_t & run, const size_t & paramIndex ) const
{
    return _params.at(run).at(paramIndex);
}

float TimeSeriesModel::results( const size_t & run, const size_t & resultIndex, const int & timeStep ) const
{
    for(size_t i=0; i<_timeSteps.size(); i++)
    {
        if(timeStep != _timeSteps.at(i))
        {
            continue;
        }
        return _results.at(run).at(resultIndex).at(i);
    }
    return 0.0f;
}
    
float TimeSeriesModel::mean( int step ) const
{
    float value = 0.0f;
    for(size_t i=0; i<numRuns(); i++)
    {
        value += results(i, _selectedResult, step);
    }
    return value/float(numRuns());
}

float TimeSeriesModel::selectedMean( int step ) const
{
    float value = 0.0f;
    int selectedRuns = 0;
    for(size_t i=0; i<numRuns(); i++)
    {
        bool selected = true;
        for(size_t j=0; j<_selectedValues.size(); j++)
        {
            if(!isParamSelected(j, params(i,j)))
            {
                selected = false;
                break;
            }
        }
        if(selected)
        {
            selectedRuns++;
            value += results(i, _selectedResult, step);
        }
    }
    return value/float(selectedRuns);
}

void TimeSeriesModel::selectResult( int index )
{
    _selectedResult = size_t(index);
}

bool TimeSeriesModel::isParamSelected( int paramIndex, float value ) const
{
    // none selected, all selected
    if(_selectedValues.at(paramIndex).size()==0)
    {
        return true;
    }

    for(size_t i=0; i<_selectedValues.at(paramIndex).size(); i++)
    {
        if(_selectedValues.at(paramIndex).at(i)==value)
        {
            return true;
        }
    }
    return false;
}

float TimeSeriesModel::maxResultValue() const
{
    float maxValue = std::numeric_limits<float>::min();
    for(auto step : _timeSteps)
    {
        float value = std::max(mean(step), selectedMean(step));
        if(value>maxValue)
        {
            maxValue = value;
        }
    }
    return maxValue;
}

float TimeSeriesModel::minResultValue() const
{   
    float minValue = std::numeric_limits<float>::max();
    for(auto step : _timeSteps)
    {
        float value = std::min(mean(step), selectedMean(step));
        if(value<minValue)
        {
            minValue = value;
        }
    }
    return minValue;
}
    
bool TimeSeriesModel::isEmpty() const
{
    if(_results.size()==0)
    {
        return true;
    }
    return false;
}
    
void TimeSeriesModel::setSelectedValues( int paramIndex, const std::vector<float> & selectedValues )
{
    _selectedValues.at(paramIndex).clear();
    _selectedValues.at(paramIndex).resize(selectedValues.size());
    std::copy(selectedValues.begin(), selectedValues.end(), _selectedValues.at(paramIndex).begin());
}

} // namespace GUI

