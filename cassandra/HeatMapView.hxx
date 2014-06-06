
#ifndef __HeatMapView_hxx__
#define __HeatMapView_hxx__

#include <QWidget>
#include <QMouseEvent>

namespace GUI
{

class HeatMapModel;

class HeatMapView : public QWidget
{
    Q_OBJECT

    QSize _cellSize;
    QPoint _selectedCell;

    QImage _gradientImage;

    const HeatMapModel & _model;
    bool _showText;
public:
    HeatMapView( QWidget * parent, const HeatMapModel & model );
    virtual ~HeatMapView();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

public slots:    
    void updateView();
    void switchText();

protected:
    void paintEvent( QPaintEvent * );
    void mouseMoveEvent( QMouseEvent * event);
	void resizeEvent( QResizeEvent * event);

};

} // namespace GUI

#endif // __HeatMapView_hxx__


