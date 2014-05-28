
#include <TimeSeriesView.hxx>
#include <TimeSeriesModel.hxx>
#include <QPainter>
#include <iostream>

namespace GUI
{

TimeSeriesView::TimeSeriesView( QWidget * parent, const TimeSeriesModel & model ) : QWidget(parent), _model(model)
{
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setSizePolicy(sizePolicy);
    setMouseTracking(true); 

}

TimeSeriesView::~TimeSeriesView()
{
}

QSize TimeSeriesView::minimumSizeHint() const
{
	return QSize(150, 150);
}

QSize TimeSeriesView::sizeHint() const
{
	return QSize(800, 600);
}

void TimeSeriesView::updateView()
{
    update();
}

void TimeSeriesView::paintEvent( QPaintEvent * )
{
    if(_model.isEmpty())
    {
        return;
    }

    QSizeF cellSize;
    cellSize.setWidth((size().width()-50)/_model.timeSteps().size());
    cellSize.setHeight((size().height()-50)/(_model.maxResultValue()-_model.minResultValue()));

    QVector<QPointF> generalMean;
    QVector<QPointF> selectedMean;
    for(size_t i=0; i<_model.timeSteps().size()-1; i++)
    {
        float value = _model.mean(_model.timeSteps().at(i));
        float nextValue = _model.mean(_model.timeSteps().at(i+1));
        
        float selectedValue = _model.selectedMean(_model.timeSteps().at(i));
        float selectedNextValue = _model.selectedMean(_model.timeSteps().at(i+1));
        
        generalMean.push_back(QPointF(50+i*cellSize.width(),size().height()-50-(value-_model.minResultValue())*cellSize.height()));
        generalMean.push_back(QPointF(50+(i+1)*cellSize.width(),size().height()-50-(nextValue-_model.minResultValue())*cellSize.height()));
        
        selectedMean.push_back(QPointF(50+i*cellSize.width(),size().height()-50-(selectedValue-_model.minResultValue())*cellSize.height()));
        selectedMean.push_back(QPointF(50+(i+1)*cellSize.width(),size().height()-50-(selectedNextValue-_model.minResultValue())*cellSize.height()));
    }
    
    QPainter screenPainter(this);
    QPen oldPen = screenPainter.pen();
    QPen pen(QColor("#6C6C6C"));
    pen.setWidth(6);
    screenPainter.setPen(pen);
    screenPainter.drawLines(generalMean);
    
    QPen selectedPen(QColor("#FF404D"));
    selectedPen.setWidth(3);
    screenPainter.setPen(selectedPen);
    screenPainter.drawLines(selectedMean);

    screenPainter.setPen(oldPen);  
    // text
    for(size_t i=0; i<_model.timeSteps().size(); i++)
    {
        screenPainter.drawText(QRect(50+i*cellSize.width(), size().height()-30, 100, 20), QString::number(_model.timeSteps().at(i)));
    }
    // 10 steps in y axis
    float yStep = (_model.maxResultValue()-_model.minResultValue())/10.0f;
    float iValue = _model.minResultValue();
    for(float i=size().height(); i>=0; i=i-((size().height()-50)/10.0f))
    {
        screenPainter.drawText(QRect(0, i-50, 100, 20), QString::number(iValue, 'f', 2));
        iValue += yStep;
    }
}

void TimeSeriesView::mouseMoveEvent( QMouseEvent * event)
{
}

} // namespace GUI

