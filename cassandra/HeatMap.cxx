
#include <HeatMap.hxx>
#include <QPainter>
#include <QLayout>
#include <QImage>
#include <QLinearGradient>
#include <iostream>
#include <HeatMapModel.hxx>

namespace GUI
{

HeatMap::HeatMap( QWidget * parent, const HeatMapModel & model ) : QWidget(parent), _model(model)
{
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setSizePolicy(sizePolicy);
    setMouseTracking(true); 
    
    QLinearGradient linearGrad(0,0,100,100);
    linearGrad.setColorAt(0.0f, QColor("#FDECA6"));
    linearGrad.setColorAt(0.2f, QColor("#E6D298"));
    linearGrad.setColorAt(0.4f, QColor("#CFB98B"));
    linearGrad.setColorAt(0.6f, QColor("#B89F7D"));
    linearGrad.setColorAt(0.8f, QColor("#A1856F"));

    QPixmap gradientValues(QSize(100,100));
    QPainter paintGrad(&gradientValues);
    QBrush brush(linearGrad);
    paintGrad.setBrush(brush);
    paintGrad.drawRect(QRect(0,0,100,100));

    _gradientImage = gradientValues.toImage();
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

void HeatMap::paintEvent( QPaintEvent * )
{
    if(_model.isEmpty())
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


    for(size_t i=0; i<_model.xTicks().size(); i++)
    {
        for(size_t j=0; j<_model.yTicks().size(); j++)
        {
            QRect rect(50+i*_cellSize.width(),50+j*_cellSize.height(),_cellSize.width(), _cellSize.height());

            int gradientCoordinate = 100.0f*_model.getNormalisedValue(i,j);
            // 0 is out of range due to QGradient code
            if(gradientCoordinate<1)
            {
                gradientCoordinate = 1;
            }
            if(gradientCoordinate>99)
            {
                gradientCoordinate= 99;
            }
            brush.setColor(QColor(_gradientImage.pixel(gradientCoordinate,gradientCoordinate)));
            painter.setBrush(brush);
            if(i==size_t(_selectedCell.x()) || j==size_t(_selectedCell.y()))
            {
                pen.setColor(QColor("#000000"));
                pen.setWidth(2.0f);
                painter.setPen(pen);
            }
            else
            {
                pen.setColor(QColor("#585858"));
                pen.setWidth(1.0f);
                painter.setPen(pen);
            }   
            painter.drawRect(rect); 
            if(i==size_t(_selectedCell.x()) && j==size_t(_selectedCell.y()))
            {   
                QFont previousFont = painter.font();  
                QFont font = painter.font();
                int pixelSize = _cellSize.height()/2;

                QFontMetrics metrics(font);
                font.setPixelSize(pixelSize);
                while(pixelSize>1 && QFontMetrics(font).width(QString::number(_model.getValue(i,j)>_cellSize.width())))
                {
                    pixelSize--;
                    font.setPixelSize(pixelSize);
                }
                painter.drawText(QRect(50+i*_cellSize.width(),50+j*_cellSize.height(), _cellSize.width(), _cellSize.height()), Qt::AlignCenter, QString::number(_model.getValue(i,j)));
                painter.setFont(previousFont);
            }
        }
    } 
    
    // text
    painter.drawText(size().width()/2, 20,_model.xParam().c_str()); 
    for(size_t i=0; i<_model.xTicks().size(); i++)
    {
        painter.drawText(QRect(50+i*_cellSize.width() + _cellSize.width()/2, 30, 100, _cellSize.height()), QString::number(_model.xTicks().at(i)));
    }

    painter.drawText(20, 20, _model.yParam().c_str());
    for(size_t i=0; i<_model.yTicks().size(); i++)
    {
        painter.drawText(QRect(20, 50+i*_cellSize.height() + _cellSize.height()/2, 100, _cellSize.height()), QString::number(_model.yTicks().at(i)));
    }


	painter.end();

    QPainter screenPainter(this);	
	screenPainter.save();
	screenPainter.drawPixmap(0,0,imageToDraw);
	screenPainter.restore();

}
	
void HeatMap::updateView()
{
    if(_model.isEmpty())
    {
        return;
    }
    QSize size(std::max(600, int(_model.xTicks().size())), std::max(600, int(_model.yTicks().size())));
    _cellSize.setWidth(std::max(1, int((size.width()-50)/_model.xTicks().size())));
    _cellSize.setHeight(std::max(1, int((size.height()-50)/_model.yTicks().size())));
//    resize(size);
    update();
}

void HeatMap::mouseMoveEvent( QMouseEvent * event)
{
    if(_model.isEmpty())
    {
        return;
    }

    _selectedCell.setX(-1);
    _selectedCell.setY(-1);
    if(event->pos().x()<50 || event->pos().y()<50)
    {
        return;
    }
    _selectedCell.setX((event->pos().x()-50)/_cellSize.width());
    _selectedCell.setY((event->pos().y()-50)/_cellSize.height());
    update();
}

} // namespace GUI

