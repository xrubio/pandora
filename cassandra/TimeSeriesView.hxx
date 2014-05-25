
#ifndef __TimeSeriesView_hxx__
#define __TimeSeriesView_hxx__

#include <QWidget>
#include <QMouseEvent>

namespace GUI
{

class TimeSeriesModel;

class TimeSeriesView : public QWidget
{

    Q_OBJECT

    const TimeSeriesModel & _model;

public:
    TimeSeriesView( QWidget * parent, const TimeSeriesModel & model );
    virtual ~TimeSeriesView();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

public slots:    
    void updateView();

protected:
    void paintEvent( QPaintEvent * );
    void mouseMoveEvent( QMouseEvent * event);


};

} // namespace GUI

#endif // __TimeSeriesView_hxx__

