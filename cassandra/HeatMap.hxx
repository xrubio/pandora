
#ifndef __HeatMap_hxx__
#define __HeatMap_hxx__

#include <QWidget>

namespace GUI
{

class HeatMap : public QWidget
{
    Q_OBJECT

    std::vector<float> _xTicks;    
    std::vector<float> _yTicks;
    std::vector< std::vector<float> > _values;
    float _maxValue;
    float _minValue;
public:
    HeatMap( QWidget * parent );
    virtual ~HeatMap();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

public slots:    
    void setXValues( std::vector<float> values );
    void setYValues( std::vector<float> values );
    void setVariables( std::vector< std::vector<float> > values );

protected:
    void paintEvent( QPaintEvent * event );

};

} // namespace GUI

#endif // __HeatMap_hxx__


