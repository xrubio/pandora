
#include <HeatMap.hxx>
#include <QPainter>
#include <QLayout>
#include <QImage>
#include <QLinearGradient>
#include <iostream>
#include <cstdio>

namespace GUI
{

HeatMap::HeatMap( QWidget * parent )
{
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setSizePolicy(sizePolicy);
  
    /*
    _values.resize(_xTicks.size());
   for(int i=0; i<_xTicks.size(); i++)
   {
       _values.at(i).resize(_yTicks.size());
       for(int j=0; j<_yTicks.size(); j++)
       {
            _values.at(i).at(j) = j;
       }
   }
   */
}

HeatMap::~HeatMap()
{
}

QSize HeatMap::minimumSizeHint() const
{
	return QSize(600, 600);
}


QSize HeatMap::sizeHint() const
{
	return QSize(600, 600);
}

void HeatMap::paintEvent( QPaintEvent * event )
{
    if(_xTicks.size()==0 || _yTicks.size()==0)
    {
        return;
    }
	QPixmap imageToDraw(size());
	QPainter painter(&imageToDraw);
	QPen pen;
	pen.setWidth(1.0f);
    pen.setColor(QColor(100,100,100));
    painter.setPen(pen);
    
    QBrush brush(Qt::SolidPattern);
	
    imageToDraw.fill(QColor(255,255,255));

    QLinearGradient linearGrad(0,0,100,100);
    linearGrad.setColorAt(0.0f, QColor("#FDECA6"));
    linearGrad.setColorAt(0.2f, QColor("#E6D298"));
    linearGrad.setColorAt(0.4f, QColor("#CFB98B"));
    linearGrad.setColorAt(0.6f, QColor("#B89F7D"));
    linearGrad.setColorAt(0.8f, QColor("#A1856F"));
    QBrush brush2(linearGrad);

    QPixmap gradientP(QSize(100,100));
    QPainter paintGrad(&gradientP);
    paintGrad.setBrush(brush2);
    paintGrad.drawRect(QRect(0,0,100,100));

    QImage foo2 = gradientP.toImage();

    QSize cellSize(std::max(1, int((size().width()-50)/_xTicks.size())), std::max(1,int((size().height()-50)/_yTicks.size())));
    for(size_t i=0; i<_xTicks.size(); i++)
    {
        for(size_t j=0; j<_yTicks.size(); j++)
        {
            QRect rect(50+i*cellSize.width(),50+j*cellSize.height(),cellSize.width(), cellSize.height());
            float normalisedValue = float(_values.at(i).at(j)-_minValue)/(_maxValue-_minValue);
            int bar = 100.0f*normalisedValue;
            // 0 is out of range due to QGradient code
            if(bar<1)
            {
                bar = 1;
            }
            if(bar>99)
            {
                bar = 99;
            }
            //std::cout << "i: " << i << " j: " << j << " value: " << _values.at(i).at(j) << " bar: " << bar << std::endl;
            brush.setColor(QColor(foo2.pixel(bar,bar)));
            painter.setBrush(brush);
            painter.drawRect(rect);
        }
    } 
    
    // text
    painter.drawText(size().width()/2, 20, "Parameter 1");    
    for(size_t i=0; i<_xTicks.size(); i++)
    {
        painter.drawText(QRect(50+i*cellSize.width() + cellSize.width()/2, 40, 100, cellSize.height()), QString::number(_xTicks.at(i)));
    }

    painter.drawText(20, 20, "Parameter 2");
    for(size_t i=0; i<_yTicks.size(); i++)
    {
        painter.drawText(QRect(20, 50+i*cellSize.height() + cellSize.height()/2, 100, cellSize.height()), QString::number(_yTicks.at(i)));
    }


	painter.end();

    QPainter screenPainter(this);	
	screenPainter.save();
	screenPainter.drawPixmap(0,0,imageToDraw);
	screenPainter.restore();
}
	
void HeatMap::setXValues( std::vector<float> values )
{
    _xTicks.clear();
    _xTicks.resize(values.size());
    std::copy(values.begin(), values.end(), _xTicks.begin());
    QSize size(std::max(600, int(_xTicks.size())), std::max(600, int(_xTicks.size())));
    resize(size);
    update();
}

void HeatMap::setYValues( std::vector<float> values )
{
    _yTicks.clear();
    _yTicks.resize(values.size());
    std::copy(values.begin(), values.end(), _yTicks.begin());
    QSize size(std::max(600, int(_xTicks.size())), std::max(600, int(_yTicks.size())));
    resize(size);
    update();
}

void HeatMap::setVariables( std::vector< std::vector<float> > values )
{
    std::cout << "num values: " << values.size() << std::endl;
    _values.clear();
    _values.resize(values.size());
    _maxValue = std::numeric_limits<float>::min();
    _minValue = std::numeric_limits<float>::max();

    for(size_t i=0; i<_values.size(); i++)
    {
        _values.at(i).resize(values.at(i).size());
        std::copy(values.at(i).begin(), values.at(i).end(), _values.at(i).begin());
        float minValue = *std::min_element(_values.at(i).begin(), _values.at(i).end());
        float maxValue = *std::max_element(_values.at(i).begin(), _values.at(i).end());
        std::cout << "\tlocal min value: " << minValue << " max: " << maxValue << std::endl;
        if(_maxValue<maxValue)
        {
            _maxValue = maxValue;
        }
        if(_minValue>minValue)
        {
            _minValue = minValue;
        }
    }
    std::cout << "min value: " << _minValue << " max: " << _maxValue << std::endl;
}
	
} // namespace GUI

