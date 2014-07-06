
#include <HeatMapModel.hxx>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <limits>
#include <algorithm>

#include <iostream>

namespace GUI
{

HeatMapModel::HeatMapModel() : _xParam("none"), _yParam("none")
{
}

HeatMapModel::~HeatMapModel()
{
}

float HeatMapModel::getValue( size_t i, size_t j) const
{
    return _values.at(i).at(j);
}

float HeatMapModel::getNormalisedValue( size_t i, size_t j) const
{
    float value = getValue(i,j);
    return (value-_minValue)/(_maxValue-_minValue);
}

const std::string & HeatMapModel::xParam() const
{
    return _xParam;
}
const std::string & HeatMapModel::yParam() const
{
    return _yParam;
}

const std::vector<float> & HeatMapModel::xTicks() const
{
    return _xTicks;
}

const std::vector<float> & HeatMapModel::yTicks() const
{
    return _yTicks;
}

const std::vector<std::string> & HeatMapModel::variables() const
{
    return _variables;
}

void HeatMapModel::updateValues()
{
    if(isEmpty())
    {
        return;
    }
    _values.clear();
    _values.resize(_xTicks.size());
    for(size_t i=0; i<_xTicks.size(); i++)
    {
        _values.at(i).resize(_yTicks.size());
    }

    for(size_t i=0; i<_xTicks.size(); i++)
    {
        for(size_t j=0; j<_yTicks.size(); j++)
        {
            int numRuns = 0;
            float value = 0.0f;
            // collect all data and divide by mean of all the rows that have this value
            for(auto row : _rows)
            {
                if(row.at(_xIndex)==_xTicks.at(i) && row.at(_yIndex)==_yTicks.at(j))
                {
                    numRuns++;
                    value += row.at(_variableIndex);
                }
            }
            if(numRuns==0)
            {
                _values.at(i).at(j) = 0;
            }
            else
            {
                _values.at(i).at(j) = value/float(numRuns);
            }
        }
    }
    updateMinMax();
}

bool HeatMapModel::loadFile( const std::string & file )
{ 
    setlocale ( LC_NUMERIC, "C" );
    std::ifstream infile(file.c_str());
    std::string line;

    // header
    std::getline(infile, line);

    std::vector<std::string> tokens;
    std::istringstream iss(line);
    std::string token;        
    
    // first token is run
    _variables.clear();
    std::getline(iss, token, ';');
    while(std::getline(iss, token, ';'))
    {
        _variables.push_back(token);
    }

    _rows.clear();
    while(std::getline(infile, line))
    {
        std::vector<float> row;
        std::istringstream iss(line);
        std::string token;

        // first token is run
        if(!std::getline(iss, token, ';'))
        {
            continue;
        }

        while(std::getline(iss, token, ';'))
        {
            row.push_back(std::atof(token.c_str()));
        }
        _rows.push_back(row);
    }
    updateVariable(0, _variables.at(0));
    updateXParam(0, _variables.at(0));
    updateYParam(0, _variables.at(0));
    
    return true;
}

void HeatMapModel::updateXParam( int index, const std::string & param )
{
    _xTicks.clear();
    _xIndex = index;
    _xParam = param;

    for(size_t i=0; i<_rows.size(); i++)
    {
        _xTicks.push_back(_rows.at(i).at(_xIndex));
    }
    std::sort(_xTicks.begin(), _xTicks.end());
    std::vector<float>::iterator it = std::unique(_xTicks.begin(), _xTicks.end());
    _xTicks.resize(std::distance(_xTicks.begin(), it));
    updateValues();
}

void HeatMapModel::updateYParam( int index, const std::string & param ) 
{
    _yTicks.clear();
    _yIndex = index;
    _yParam = param;

    for(size_t i=0; i<_rows.size(); i++)
    {
        _yTicks.push_back(_rows.at(i).at(_yIndex));
    }
    std::sort(_yTicks.begin(), _yTicks.end());
    std::vector<float>::iterator it = std::unique(_yTicks.begin(), _yTicks.end());
    _yTicks.resize(std::distance(_yTicks.begin(), it));
    updateValues();
}

void HeatMapModel::updateVariable( int index, const std::string & variable)
{
    _variableIndex = index;
    _variable = variable;
    updateValues();
}

void HeatMapModel::updateMinMax()
{
    _maxValue = std::numeric_limits<float>::min();
    _minValue = std::numeric_limits<float>::max();

    for(size_t i=0; i<_values.size(); i++)
    {
        float minValue = *std::min_element(_values.at(i).begin(), _values.at(i).end());
        float maxValue = *std::max_element(_values.at(i).begin(), _values.at(i).end());
        if(_maxValue<maxValue)
        {
            _maxValue = maxValue;
        }
        if(_minValue>minValue)
        {
            _minValue = minValue;
        }
    }
}

bool HeatMapModel::isEmpty() const
{
    if(_xTicks.size()==0 || _yTicks.size()==0)
    {
        return true;
    }
    return false;
}

} // namespace GUI

