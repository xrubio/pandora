/*
 * Copyright (c) 2013
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

#include <Laboratory.hxx>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextEdit>
#include <QTextStream>

namespace GUI
{

Laboratory::Laboratory(QWidget * parent) : QDialog(parent)
{
	setModal(false);
	_lab.setupUi(this);
	
	connect(_lab.simulationButton, SIGNAL(clicked()), this, SLOT(selectSimulation()));
	connect(_lab.configButton, SIGNAL(clicked()), this, SLOT(selectConfig()));
	connect(_lab.outputButton, SIGNAL(clicked()), this, SLOT(selectOutput()));
	connect(_lab.showButton, SIGNAL(clicked()), this, SLOT(showConfig()));
	connect(_lab.parseButton, SIGNAL(clicked()), this, SLOT(parseConfig()));
	connect(_lab.paramsTree, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(paramChanged(QTreeWidgetItem*, int)));
	connect(_lab.paramsTree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(doubleClick(QTreeWidgetItem*, int)));

	_lab.buttonBox->addButton("Generate", QDialogButtonBox::ApplyRole);
	_lab.buttonBox->addButton("Run", QDialogButtonBox::ApplyRole);
	connect(_lab.buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(buttonClicked(QAbstractButton*)));
}

Laboratory::~Laboratory()
{
}

void Laboratory::buttonClicked( QAbstractButton * button )
{
	if(button->text()=="Generate")
	{
		generateConfigs();
	}
	else if(button->text()=="Run")
	{
		runSimulations();
	}
}

void Laboratory::generateConfigs()
{
}

void Laboratory::runSimulations()
{
}

void Laboratory::selectSimulation()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Select Simulation Binary"), "", tr("All Files (*)"));
	if (fileName.isEmpty())
	{
		return;
	}
	else
	{
		_simulationBinary = fileName.toStdString();
		_lab.simulationEdit->setText(fileName);
	}
}

void Laboratory::selectConfig()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Select Simulation Config"), "", tr("Config file (*.xml);;All Files (*)"));
	if (fileName.isEmpty())
	{
		return;
	}
	else
	{
		_simulationConfig = fileName.toStdString();
		_lab.configEdit ->setText(fileName);
	}
}

void Laboratory::selectOutput()
{
	QString fileName = QFileDialog::getExistingDirectory(this, tr("Select Output dir"), "");
	if (fileName.isEmpty())
	{
		return;
	}
	else
	{
		_outputDir = fileName.toStdString();
		_lab.outputEdit->setText(fileName);
	}
}

void Laboratory::showConfig()
{
	QFile file(_simulationConfig.c_str());
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QMessageBox msgBox;
		msgBox.setText("Unable to open config file");
		msgBox.exec();
		return;
	}

	QTextEdit * text = new QTextEdit(_simulationConfig.c_str(), 0);
	text->clear();
	text->setReadOnly(true);
	QString line;

	QTextStream stream(&file);
	while(!stream.atEnd())
	{
		line = stream.readLine();
		text->append(line);
	}
	file.close();
	text->show();
}

void Laboratory::parseConfig()
{	
	TiXmlDocument doc(_simulationConfig.c_str());
	if (!doc.LoadFile())
	{
		QMessageBox msgBox;
		msgBox.setText("Unable to open config file");
		msgBox.exec();
		return;
	}
	TiXmlHandle hDoc(&doc);
	TiXmlHandle hRoot(0);
    
	TiXmlElement * root = doc.FirstChildElement( "config" );
	if(!root)
	{
		QMessageBox msgBox;
		msgBox.setText("Bad formatted Pandora config file");
		msgBox.exec();
		return;
	}
	_lab.paramsTree->clear();
	parseLevel(root, 0);
}

void Laboratory::parseAttributes(TiXmlElement * parent, QTreeWidgetItem * parentItem )
{
	TiXmlAttribute * attribute = 0;
	for(attribute=parent->FirstAttribute(); attribute!=0; attribute=attribute->Next())
	{
		QTreeWidgetItem * item = new QTreeWidgetItem();
		item->setText(eName, attribute->Name());
		int value = 0;
		float valueF = 0.0f;
		if(parent->QueryIntAttribute(attribute->Name(), &value)==TIXML_SUCCESS)
		{
			item->setText(eMin, attribute->Value());
			item->setText(eMax, attribute->Value());
			item->setText(eStep, "1");
		}
		else if(parent->QueryFloatAttribute(attribute->Name(), &valueF)==TIXML_SUCCESS)
		{
			item->setText(eMin, attribute->Value());
			item->setText(eMax, attribute->Value());
			item->setText(eStep, "0.1");
		}
		else
		{
			item->setText(eDefault, attribute->Value());
		}
		parentItem->addChild(item);
		item->setExpanded(true);
		item->setFlags(item->flags() | Qt::ItemIsEditable);
	}
}

void Laboratory::parseLevel( TiXmlNode * parent, QTreeWidgetItem * parentItem)
{
	TiXmlNode * child;
	TiXmlText * text;
	int type = parent->Type();
	QTreeWidgetItem * newItem = 0;

	switch(type)
	{
	case TiXmlNode::TINYXML_DOCUMENT:
		break;

	case TiXmlNode::TINYXML_ELEMENT:
		newItem = new QTreeWidgetItem();
		newItem->setText(0, parent->Value());
		if(!parentItem)
		{
			_lab.paramsTree->insertTopLevelItem(0, newItem);
		}
		else
		{
			parentItem->addChild(newItem);
		}
		newItem->setExpanded(true);
		parseAttributes(parent->ToElement(), newItem);
		break;		

	case TiXmlNode::TINYXML_COMMENT:
		break;

	case TiXmlNode::TINYXML_UNKNOWN:
		break;

	case TiXmlNode::TINYXML_TEXT:
		break;

	case TiXmlNode::TINYXML_DECLARATION:
		break;
	default:
		break;
	}
	
	for(child=parent->FirstChild(); child!=0; child=child->NextSibling())
	{
		parseLevel(child, newItem);
	}
}
	
void Laboratory::paramChanged( QTreeWidgetItem * item, int column )
{
	if(column==eDefault)
	{
		return;
	}
	// check more than minimum
	if(column==eMax || column==eMin)
	{
		float valueMin = item->text(eMin).toFloat();
		float valueMax = item->text(eMax).toFloat();
		if(valueMin>valueMax)
		{
			item->setBackground(eMin, QBrush(QColor(255,0,0)));
			item->setBackground(eMax, QBrush(QColor(255,0,0)));
		}
		else
		{
			item->setBackground(eMin, QBrush(QColor(255,255,255)));
			item->setBackground(eMax, QBrush(QColor(255,255,255)));
		}
	}
}

void Laboratory::doubleClick(QTreeWidgetItem * item, int column)
{
	if(column==eName)
	{
		return;
	}
	// a number
	if(item->text(eDefault).isEmpty() && column==eDefault)
	{
		return;
	}
	// not a number
	if(column!=eDefault)
	{
		return;
	}
	_lab.paramsTree->editItem(item, column);
}

} // namespace GUI

