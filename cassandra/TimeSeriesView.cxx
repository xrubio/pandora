
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
	return QSize(600, 600);
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
    cellSize.setWidth(size().width()/_model.timeSteps().size());
    cellSize.setHeight(size().height()/(_model.maxResultValue()-_model.minResultValue()));

    /*
    std::cout << "size: " << size().width() << "/" << size().height() << std::endl;
    std::cout << "min: " << _model.minResultValue() << " max: " << _model.maxResultValue() << " diff: " << _model.maxResultValue()-_model.minResultValue() << std::endl;
    std::cout << "resolution: " << cellSize.width() << "/" << cellSize.height() << std::endl;
    */

    QVector<QPointF> generalMean;
    QVector<QPointF> selectedMean;
    for(size_t i=0; i<_model.timeSteps().size()-1; i++)
    {
        float value = _model.mean(_model.timeSteps().at(i));
        float nextValue = _model.mean(_model.timeSteps().at(i+1));
        
        float selectedValue = _model.selectedMean(_model.timeSteps().at(i));
        float selectedNextValue = _model.selectedMean(_model.timeSteps().at(i+1));

        int step = _model.timeSteps().at(i);
        int nextStep = _model.timeSteps().at(i+1);

        generalMean.push_back(QPointF(step*cellSize.width(),size().height()-(value-_model.minResultValue())*cellSize.height()));
        generalMean.push_back(QPointF(nextStep*cellSize.width(),size().height()-(nextValue-_model.minResultValue())*cellSize.height()));
        
        selectedMean.push_back(QPointF(step*cellSize.width(),size().height()-(selectedValue-_model.minResultValue())*cellSize.height()));
        selectedMean.push_back(QPointF(nextStep*cellSize.width(),size().height()-(selectedNextValue-_model.minResultValue())*cellSize.height()));

    }
    QPainter screenPainter(this);
    QPen oldPen = screenPainter.pen();

    QPen pen(QColor("#6C6C6C"));
    pen.setWidth(5);
    screenPainter.setPen(pen);
    screenPainter.drawLines(generalMean);
    
    QPen selectedPen(QColor("#FF0000"));
    selectedPen.setWidth(4);
    screenPainter.setPen(selectedPen);
    screenPainter.drawLines(selectedMean);

    screenPainter.setPen(oldPen);
}

void TimeSeriesView::mouseMoveEvent( QMouseEvent * event)
{
}

} // namespace GUI

