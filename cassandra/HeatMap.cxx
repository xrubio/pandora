
#include <HeatMap.hxx>
#include <QPainter>
#include <QLayout>
#include <QImage>
#include <QLinearGradient>
#include <iostream>

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
    QSize size(600,600);
	QPixmap imageToDraw(size);
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

    QSize cellSize((size.width()-50)/_xTicks.size(), (size.height()-50)/_yTicks.size());
    std::cout << "cellSize: " << cellSize.width() << "/" << cellSize.height() << std::endl;

    /*
    for(size_t i=0; i<_xTicks.size(); i++)
    {
        for(size_t j=0; j<_yTicks.size(); j++)
        {
            QRect rect(50+i*cellSize.width(),50+j*cellSize.height(),cellSize.width(), cellSize.height());
            int bar = 100.0f*float(_values.at(i).at(j))/10.0f;
            // 0 is out of range due to QGradient code
            if(bar<1)
            {
                bar = 1;
            }
            //std::cout << "i: " << i << " j: " << j << " value: " << _values.at(i).at(j) << " bar: " << bar << std::endl;
            brush.setColor(QColor(foo2.pixel(bar,bar)));
            painter.setBrush(brush);
            painter.drawRect(rect);
        }
    } 
    */
    
    // text
    painter.drawText(size.width()/2, 20, "Parameter 1");
    for(size_t i=0; i<_xTicks.size(); i++)
    {
        painter.drawText(50+i*cellSize.width() + cellSize.width()/2, 40, QString::number(_xTicks.at(i)));
    }

    painter.drawText(20, 20, "Parameter 2");
    for(size_t i=0; i<_yTicks.size(); i++)
    {
        painter.drawText(20, 50+i*cellSize.height() + cellSize.height()/2, QString::number(_yTicks.at(i)));
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
    std::unique_copy(values.begin(), values.end(), std::back_inserter(_xTicks));
    //std::unique_copy(myvector.begin(), myvector.end(), myfunction);   // (no changes)
    std::cout << "number of x ticks: " << _xTicks.size() << std::endl;
    update();
}

void HeatMap::setYValues( std::vector<float> values )
{
    _yTicks.clear();
    std::unique_copy(values.begin(), values.end(), std::back_inserter(_yTicks));
    //std::unique_copy(myvector.begin(), myvector.end(), myfunction);   // (no changes)
    std::cout << "number of y ticks: " << _yTicks.size() << std::endl;
    update();
}
	
} // namespace GUI

