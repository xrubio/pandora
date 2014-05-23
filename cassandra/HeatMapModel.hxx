
#ifndef __HeatMapModel_hxx__
#define __HeatMapModel_hxx__

#include <vector>
#include <string>

namespace GUI
{

// data used in HeatMap MVC
class HeatMapModel
{
    std::vector<float> _xTicks;
    std::vector<float> _yTicks;
    std::vector<std::string> _variables;
    std::vector< std::vector<float> > _values;

    std::vector< std::vector<float> > _rows;

    int _xIndex;
    std::string _xParam;
    int _yIndex;
    std::string _yParam;

    int _variableIndex;
    std::string _variable;

    float _minValue;
    float _maxValue;

public:
    HeatMapModel();
    virtual ~HeatMapModel();


    const std::string & xParam() const;
    const std::string & yParam() const;
    
    const std::vector<float> & xTicks() const;
    const std::vector<float> & yTicks() const; 
    const std::vector<std::string> & variables() const;

    float getValue( size_t i, size_t j) const;
    float getNormalisedValue( size_t i, size_t j) const;

    void updateValues();
    bool loadFile( const std::string & file );
    void updateXParam( int index, const std::string & param );
    void updateYParam( int index, const std::string & param );
    void updateVariable( int index, const std::string & variable );
    
    void updateMinMax();
    bool isEmpty() const;
};

} // namespace GUI

#endif // __HeatMapModel_hxx__


