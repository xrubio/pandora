
#include <ColorInterval.hxx>
#include <QColorDialog>
#include <iostream>
 
namespace GUI
{

ColorInterval::ColorInterval(QWidget * parent ) : QWidget(parent)
{
	_colorInterval.setupUi(this);
	connect(_colorInterval.colorChooserButton, SIGNAL(clicked()), this, SLOT(selectColor()));
	connect(_colorInterval.splitButton, SIGNAL(clicked()), this, SLOT(split()));
	connect(_colorInterval.removeButton, SIGNAL(clicked()), this, SLOT(remove()));
}

void ColorInterval::split()
{
	emit splitColorInterval();

}

void ColorInterval::remove()
{
	emit removeColorInterval();
}

void ColorInterval::selectColor()
{
	QColorDialog dialog(this);
	dialog.setCurrentColor(_color);
	connect(&dialog, SIGNAL(colorSelected(const QColor &)), this, SLOT(colorSelected(const QColor &)));
	dialog.exec();
}

void ColorInterval::colorSelected( const QColor & color )
{
	_color = color;
	QPalette palette(_colorInterval.colorChooserButton->palette());
	palette.setColor(QPalette::Button, _color);
	_colorInterval.colorChooserButton->setPalette(palette);
}

ColorInterval::~ColorInterval()
{
}

void ColorInterval::setMinValue( const int & value )
{
	_colorInterval.minValue->setValue(value);
}

void ColorInterval::setMaxValue( const int & value )
{
	_colorInterval.minValue->setMaximum(value);
	if(value>_colorInterval.maxValue->maximum())
	{
		_colorInterval.maxValue->setMaximum(value);
	}
	if(value<_colorInterval.maxValue->maximum())
	{
		_colorInterval.maxValue->setMinimum(value);
	}
	_colorInterval.maxValue->setValue(value);
}

void ColorInterval::setColor( const QColor & color )
{
	colorSelected(color);
}

int ColorInterval::getMinValue()
{
	return _colorInterval.minValue->value();
}

int ColorInterval::getMaxValue()
{
	return _colorInterval.maxValue->value();
}

const QColor & ColorInterval::getColor()
{
	return _color;
}

void ColorInterval::connectToNext( ColorInterval * next )
{
	connect(next->_colorInterval.minValue, SIGNAL(valueChanged(int)), this, SLOT(updateMaxValue(int)));
	connect(_colorInterval.minValue, SIGNAL(valueChanged(int)), next, SLOT(updateMinValue(int)));
}

void ColorInterval::updateMaxValue( int value )
{
	setMaxValue(value-1);
}

void ColorInterval::updateMinValue( int value )
{
	_colorInterval.minValue->setMinimum(value+1);
}

void ColorInterval::deactivateMinValue()
{
	_colorInterval.minValue->setReadOnly(true);
	_colorInterval.minValue->setEnabled(false);
}

void ColorInterval::checkMinimum( int value)
{
	_colorInterval.maxValue->setMinimum(value+1);
	emit adjustMin(value+1);
}

void ColorInterval::checkMaximum( int value)
{
	_colorInterval.minValue->setMaximum(value-1);	
	emit adjustMax(value-1);
}

void ColorInterval::adjustMaxValue(int value)
{
	_colorInterval.maxValue->setMaximum(value);
}

void ColorInterval::adjustMinValue(int value)	
{
	_colorInterval.minValue->setMinimum(value);
}

} // namespace GUI

