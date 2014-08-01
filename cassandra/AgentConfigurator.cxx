/*
 * Copyright (c) 2012
 * COMPUTER APPLICATIONS IN SCIENCE & ENGINEERING
 * BARCELONA SUPERCOMPUTING CENTRE - CENTRO NACIONAL DE SUPERCOMPUTACIÓN
 * http://www.bsc.es
 *
 * This file is part of Cassandra.
 * Cassandra is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Cassandra is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *  
 * You should have received a copy of the GNU General Public 
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#include <AgentConfigurator.hxx>
#include <ProjectConfiguration.hxx>
#include <sstream>
#include <iostream>
#include <QTextStream>

#include <QApplication>
#include <QVBoxLayout>
#include <QPushButton>
#include <QColorDialog>
#include <QFileDialog>
#include <QIcon>
#include <QTextStream>
#include <QLabel>
#include <QCloseEvent>
#include <QMessageBox>

#include <QImageReader>

namespace GUI
{

AgentConfigurator::AgentConfigurator( QWidget * parent, const std::string & type ) : QDialog(parent), _type(type), _configuration(*ProjectConfiguration::instance()->getAgentConfig(type))
{
	setModal(true);
	_agentConfig.setupUi(this);

	colorSelected(_configuration.getColor());
	if(!_configuration.getFileName2D().empty())
	{
		_agentConfig.iconChooserButton->setIcon(_configuration.getIcon());
	}
	else
	{
		_agentConfig.iconButton->setEnabled(false);
		_agentConfig.colorButton->setEnabled(false);
	}

	_agentConfig.size2D->setValue(_configuration.getSize());
	_agentConfig.model3DFilename->setText(_configuration.getFileName3D().c_str());

	_agentConfig.xSize->setValue(_configuration.getSize3D()._x);
	_agentConfig.ySize->setValue(_configuration.getSize3D()._y);
	_agentConfig.zSize->setValue(_configuration.getSize3D()._z);

	if(_configuration.useIcon())
	{
		_agentConfig.iconButton->setChecked(true);
		_agentConfig.colorButton->setChecked(false);
	}
	else
	{
		_agentConfig.iconButton->setChecked(false);
		_agentConfig.colorButton->setChecked(true);
	}

    if(_configuration.showValue())
    {
        _agentConfig.showValues->setChecked(true);
    }
    else
    {
        _agentConfig.showValues->setChecked(false);
    }
	connect(_agentConfig.colorChooserButton, SIGNAL(clicked()), this, SLOT(selectColor()));
	connect(_agentConfig.iconChooserButton, SIGNAL(clicked()), this, SLOT(selectIcon()));
	connect(_agentConfig.model3DButton, SIGNAL(clicked()), this, SLOT(selectModel3D()));


	/*
	//!Declaració de la finestra amb les formes
	QWidget * shapeChooser = new QWidget;

	//FINESTRA PRINCIPAL
	//!Modifiquem característiques de la finestra principal
	setWindowTitle("Select shape and color");
	resize(500, 100);
	move(250, 250);
	//!Declarem botons dins la finestra principal per Colors i per a Formes
	QPushButton*buttonColors = new QPushButton("Colors");
	QPushButton*buttonShapes = new QPushButton("Shape");
	QPushButton*buttonConfirmMain = new QPushButton("Accept");
	QPushButton*buttonCancelMain = new QPushButton("Cancel");
	
	//!S'enllaça cada botó a la señal y al procès que ha d'executar
	connect(buttonColors, SIGNAL(clicked()), this, SLOT(selectColor()));
	connect(buttonShapes, SIGNAL(clicked()), shapeChooser, SLOT(show()));
	connect(buttonConfirmMain, SIGNAL(clicked()), this, SLOT(closeConfirm()));
	connect(buttonCancelMain, SIGNAL(clicked()), this, SLOT(close()));
	
	
	//!Es compacta tots els widgets dins d'una mateixa finestra MAIN
	QVBoxLayout * layoutMain = new QVBoxLayout;
	layoutMain->addWidget(buttonColors);
	layoutMain->addWidget(buttonShapes);
	layoutMain->addWidget(buttonConfirmMain);
	layoutMain->addWidget(buttonCancelMain);
	
	setLayout(layoutMain);
	*/
	show();

	/*
	//FINESTRA DE FORMES
	//!Modifiquem característiques de la finestra de formes dels agents
	shapeChooser->setWindowTitle("Select shape");
	shapeChooser->resize(100, 100);
	shapeChooser->move(250, 250);
	//!Declarem els botons del widget de formes per a cada forma corresponent
	//RECTANGLE
	QPushButton * buttonRectangle = new QPushButton();
	buttonRectangle->setIcon(QIcon(":/resources/images/rectangle.png"));
	connect(buttonRectangle, SIGNAL(clicked()), this, SLOT(rectangleClicked()));
	
	//CIRCLE
	QPushButton * buttonCircle = new QPushButton();
	buttonCircle->setIcon(QIcon(":/resources/images/circle.png"));
	connect(buttonCircle, SIGNAL(clicked()), this, SLOT(circleClicked()));
	
	//TRIANGLE
	QPushButton * buttonTriangle = new QPushButton();
	buttonTriangle->setIcon(QIcon(":/resources/images/triangle.png"));
	connect(buttonTriangle, SIGNAL(clicked()), this, SLOT(triangleClicked()));
	
	//SQUARE
	QPushButton * buttonSquare = new QPushButton();
	buttonSquare->setIcon(QIcon(":/resources/images/square.png"));
	connect(buttonSquare, SIGNAL(clicked()), this, SLOT(squareClicked()));
	
	//STAR
	QPushButton * buttonStar = new QPushButton();
	buttonStar->setIcon(QIcon(":/resources/images/star.png"));
	connect(buttonStar, SIGNAL(clicked()), this, SLOT(starClicked()));
	
	QPushButton * buttonConfirm = new QPushButton("Accept");
	connect(buttonConfirm, SIGNAL(clicked()), shapeChooser, SLOT(close()));
	
	//!Es compacten els botons al dialeg de formes a una finestra FORMS
	QVBoxLayout * layoutShapeChooser = new QVBoxLayout;
	layoutShapeChooser->addWidget(buttonRectangle);
	layoutShapeChooser->addWidget(buttonCircle);
	layoutShapeChooser->addWidget(buttonTriangle);
	layoutShapeChooser->addWidget(buttonSquare);
	layoutShapeChooser->addWidget(buttonStar);
	layoutShapeChooser->addWidget(buttonConfirm);
	shapeChooser->setLayout(layoutShapeChooser);
	*/
}

AgentConfigurator::~AgentConfigurator()
{
}

/*
void AgentConfigurator::rectangleClicked()
{	
	_selectedShape = "rectangle";
}

void AgentConfigurator::circleClicked()
{	
	_selectedShape = "circle";
}

void AgentConfigurator::squareClicked()
{	
	_selectedShape = "square";
}

void AgentConfigurator::triangleClicked()
{	
	_selectedShape = "triangle";
}

void AgentConfigurator::starClicked()
{	
	_selectedShape = "star";
}
*/
void AgentConfigurator::selectColor()
{	
	QColorDialog dialog(this);
	dialog.setCurrentColor(_configuration.getColor());
	connect(&dialog, SIGNAL(colorSelected(const QColor &)), this, SLOT(colorSelected(const QColor &)));
	dialog.exec();
}

void AgentConfigurator::selectIcon()
{
	std::size_t rightSlash = _configuration.getFileName2D().find_last_of("/");
	std::string path = _configuration.getFileName2D().substr(0,rightSlash);

	std::cout << "path: " << path << std::endl;
	QString formats = "(";
	for(int i=0; i<QImageReader::supportedImageFormats().count(); i++)
	{
		formats += "*.";
		formats += QString(QImageReader::supportedImageFormats().at(i)).toLower();
		formats += " ";
	}
	formats += ")";

	QString fileName = QFileDialog::getOpenFileName(this, tr("Select Icon"), path.c_str(), tr("Images ")+formats);
	if(fileName.isEmpty())
	{
		_agentConfig.iconButton->setEnabled(false);
		_agentConfig.colorButton->setEnabled(false);
		return;
	}
	else
	{
		//_configuration.setIcon(fileName.toStdString());
		_configuration.setFileName2D(fileName.toStdString());
		_agentConfig.iconChooserButton->setIcon(_configuration.getIcon());	
		_agentConfig.iconButton->setEnabled(true);
		_agentConfig.colorButton->setEnabled(true);
	}
}

void AgentConfigurator::selectModel3D()
{
	std::size_t rightSlash = _configuration.getFileName3D().find_last_of("/");
	std::string path = _configuration.getFileName3D().substr(0,rightSlash);

	QString fileName = QFileDialog::getOpenFileName(this, tr("Select Model"), path.c_str(), tr("3D Studio Max (*.3ds);;All Files (*)"));
	if(fileName.isEmpty())
	{
		return;
	}
	else
	{
		_configuration.setFileName3D(fileName.toStdString());
		_agentConfig.model3DFilename->setText(fileName);
	}
}

void AgentConfigurator::colorSelected(const QColor & color)
{
	_configuration.setColor(color);
	QPalette palette(_agentConfig.colorChooserButton->palette());
	palette.setColor(QPalette::Button, _configuration.getColor());
	_agentConfig.colorChooserButton->setPalette(palette);
}

void AgentConfigurator::accept()
{
    _configuration.setUseIcon(_agentConfig.iconButton->isChecked());
    _configuration.showValue(_agentConfig.showValues->isChecked());

	_configuration.setSize(_agentConfig.size2D->value());
	Engine::Point3D<float> scale(_agentConfig.xSize->value(), _agentConfig.ySize->value(), _agentConfig.zSize->value());
	_configuration.setSize3D(scale);

	emit agentConfigured(_type, _configuration);
	close();
}

} // namespace GUI

