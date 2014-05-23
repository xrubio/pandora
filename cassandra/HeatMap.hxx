
#ifndef __HeatMap_hxx__
#define __HeatMap_hxx__

#include <QWidget>
#include <QMouseEvent>

namespace GUI
{

class HeatMapModel;

class HeatMap : public QWidget
{
    Q_OBJECT

    QSize _cellSize;
    QPoint _selectedCell;

    QImage _gradientImage;

    const HeatMapModel & _model;
public:
    HeatMap( QWidget * parent, const HeatMapModel & model );
    virtual ~HeatMap();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

public slots:    
    void updateView();

protected:
    void paintEvent( QPaintEvent * );
    void mouseMoveEvent( QMouseEvent * event);

};

} // namespace GUI

#endif // __HeatMap_hxx__


