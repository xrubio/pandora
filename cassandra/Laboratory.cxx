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
#include <boost/filesystem.hpp>
#include <iomanip>
#include <Exception.hxx>
#include <RunSimulations.hxx>
#include <SimulationControlThread.hxx>
#include <limits>

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
	connect(_lab.paramsTree, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(paramChanged(QTreeWidgetItem*, int)));
	connect(_lab.paramsTree, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(doubleClick(QTreeWidgetItem*, int)));

	_generateButton = _lab.buttonBox->addButton("Generate", QDialogButtonBox::ApplyRole);
	_runButton = _lab.buttonBox->addButton("Run", QDialogButtonBox::ApplyRole);
	connect(_lab.buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(buttonClicked(QAbstractButton*)));
	connect(_lab.numRepeats, SIGNAL(valueChanged(int)), this, SLOT(numRepeatsChanged(int)));

	_lab.paramGroup->setEnabled(false);
	_lab.experimentsGroup->setEnabled(false);
	_generateButton->setEnabled(false);
	_runButton->setEnabled(false);

	_lab.statusLabel->setText("Select simulation binary");

	_lab.outputEdit->setText(QString(boost::filesystem::current_path().c_str()).append("/experiments"));
	_outputDir = _lab.outputEdit->text().toStdString();

	_runSimulations = new RunSimulations(0);
	connect(this, SIGNAL(nextSimulation()), _runSimulations, SLOT(updateSimulationRun()));

	_lab.paramsTree->header()->setMovable(false);
	_lab.paramsTree->header()->setResizeMode(0, QHeaderView::ResizeToContents);
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

void Laboratory::numRepeatsChanged(int )
{
	updateNumberOfExecutions();
}

void Laboratory::computeExperiments()
{
	_runs.clear();
	_runs.resize(_numExperiments*_lab.numRepeats->value());

	QTreeWidgetItem * item = _lab.paramsTree->topLevelItem(0);

	int totalRuns = _lab.numRepeats->value()*_numExperiments;
	int iterations = _numExperiments;
	while(item)
	{
		// number params are the only ones to be recorded in _runs
		if(!item->text(eMin).isEmpty())
		{
			float minValue = item->text(eMin).toFloat();
			float maxValue = item->text(eMax).toFloat();
			float stepValue = item->text(eStep).toFloat();
			int index = 0;
			// repetition
			if(minValue==maxValue)
			{
				for(int i=0; i<totalRuns; i++)
				{
					ParamsMap & params = _runs.at(index);
					params.insert(std::make_pair(item, item->text(eMin).toStdString()));
					index++;
				}
			}
			// parameter sweep
			else
			{
				int series = _numExperiments/iterations;
				int numCombinations = 1+(maxValue-minValue)/stepValue;
				iterations /= numCombinations;
				
				// for the number of repetitions
				for(int r=0; r<_lab.numRepeats->value(); r++)
				{
					for(int z=0; z<series; z++)
					{
						// iterate through float values (use transformed integer to avoir iterating through floats)						
						for(int j=int(1000000.0f*minValue); j<=int(1000000.0f*maxValue); j+=(1000000.0f*stepValue))
						{
							// a number of times equal to iterations
							for(int i=0; i<iterations; i++)
							{
								ParamsMap & params = _runs.at(index);
								params.insert(std::make_pair(item, QString::number(float(j)/1000000.0).toStdString()));
								index++;
							}
						}
					}
				}
			}
			if(index!=_runs.size())
			{	
				std::stringstream oss;
				oss << "Laboratory::computeExperiments - number of experiments: " << _runs.size() << " does not match with computed number: " << index << " for attribute: " << item->text(eName).toStdString() << " of element: " << item->parent()->text(eName).toStdString();
				throw Engine::Exception(oss.str());
				return;
			}
		}
		item = _lab.paramsTree->itemBelow(item);
	}
}

void Laboratory::storeChildren(TiXmlElement * parentElement, QTreeWidgetItem * parentItem, int index)
{
	for(int i=0; i<parentItem->childCount(); i++)
	{
		QTreeWidgetItem * item = parentItem->child(i);
		// attribute
		if(item->childCount()==0)
		{
			// string
			if(!item->text(eText).isEmpty())
			{
				parentElement->SetAttribute(item->text(eName).toStdString(), item->text(eText).toStdString());
			}
			// number
			else if(!item->text(eMin).isEmpty())
			{
				ParamsMap & params = _runs.at(index);
				ParamsMap::iterator it = params.find(item);
				if(it==params.end())
				{	
					std::stringstream oss;
					oss << "Laboratory::storeChildren - trying to get unknown number attribute: " << item->text(eName).toStdString() << " for element: " << parentItem->text(eName).toStdString();
					throw Engine::Exception(oss.str());
					return;
				}
				parentElement->SetAttribute(item->text(eName).toStdString(), it->second);
			}
		}
		else
		{
			TiXmlElement * element = new TiXmlElement(item->text(eName).toStdString());
			storeChildren(element, item, index);
			parentElement->LinkEndChild(element);
		}
	}
}

void Laboratory::createConfigFile( const std::string & dir, int index )
{
	TiXmlDocument doc;
	doc.LinkEndChild(new TiXmlDeclaration( "1.0", "", "" ));
	

	for(int i=0; i<_lab.paramsTree->topLevelItemCount(); i++)
	{
		QTreeWidgetItem * item = _lab.paramsTree->topLevelItem(i);
		TiXmlElement * element = new TiXmlElement(item->text(eName).toStdString());
		storeChildren(element, item, index);
		doc.LinkEndChild( element );
	}
	doc.SaveFile(dir+"/config.xml" );
}

void Laboratory::generateConfigs()
{
	boost::filesystem::path dataDir(_outputDir);
	if(boost::filesystem::exists(_outputDir) && !boost::filesystem::is_empty(_outputDir))
	{
		QMessageBox::StandardButton button = QMessageBox::question(0, "Directory exists", QString("Directory \"").append(QString(_outputDir.c_str()).append("\" is not empty; do you want to overwrite it?")), QMessageBox::Yes | QMessageBox::No);
		if(button==QMessageBox::No)
		{
			return;
		}
		boost::filesystem::remove_all(_outputDir);
	}
	boost::filesystem::create_directory(_outputDir);
	int totalExperiments = _numExperiments*_lab.numRepeats->value();
	computeExperiments();


	for(int i=0; i<totalExperiments; i++)
	{
		std::stringstream oss;
		oss << _outputDir << "/run_" << std::setfill('0') << std::setw(4) << i;
		boost::filesystem::create_directory(oss.str());
		createConfigFile(oss.str(), i);
	}
	_lab.statusLabel->setText("Experiments generated, ready to run");
	_runButton->setEnabled(true);
}

void Laboratory::runSimulations()
{
	int totalExperiments = _numExperiments*_lab.numRepeats->value();
	_runSimulations->init(totalExperiments);
	_runSimulations->show();
	SimulationControlThread * thread = new SimulationControlThread(_simulationBinary, _outputDir, totalExperiments);

	connect(_runSimulations, SIGNAL(rejected()), thread, SLOT(cancelExecution()));
	connect(thread, SIGNAL(nextSimulation()), _runSimulations, SLOT(updateSimulationRun()));
	thread->start();
}

void Laboratory::selectSimulation()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Select Simulation binary"), "", tr("All Files (*)"));
	if (fileName.isEmpty())
	{
		return;
	}
	
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly))
	{
		_lab.paramGroup->setEnabled(false);
		_lab.experimentsGroup->setEnabled(false);
		_generateButton->setEnabled(false);
		_runButton->setEnabled(false);

		QMessageBox msgBox;
		msgBox.setText("Unable to open simulation binary");
		msgBox.exec();
		return;
	}
	_simulationBinary = fileName.toStdString();
	_lab.simulationEdit->setText(fileName);
	_lab.paramGroup->setEnabled(true);
	_lab.showButton->setEnabled(false);
	_lab.experimentsGroup->setEnabled(false);
	_generateButton->setEnabled(false);
	_runButton->setEnabled(false);
	_lab.statusLabel->setText("Select simulation config file");

}

void Laboratory::selectConfig()
{
	QString fileName = QFileDialog::getOpenFileName(this, tr("Select Simulation Config"), "", tr("Config file (*.xml);;All Files (*)"));
	if (fileName.isEmpty())
	{
		return;
	}
	
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly))
	{
		_lab.experimentsGroup->setEnabled(false);
		_generateButton->setEnabled(false);
		_runButton->setEnabled(false);

		QMessageBox msgBox;
		msgBox.setText("Unable to open config file");
		msgBox.exec();
		return;
	}

	_simulationConfig = fileName.toStdString();
	if(!parseConfig())
	{	
		_lab.experimentsGroup->setEnabled(false);
		_generateButton->setEnabled(false);
		_runButton->setEnabled(false);
	}

	_lab.configEdit ->setText(fileName);
	_lab.experimentsGroup->setEnabled(true);
	_lab.showButton->setEnabled(true);

	updateNumberOfExecutions();
	_runButton->setEnabled(false);
}

void Laboratory::updateNumberOfExecutions()
{
	_numExperiments = 1;

	QTreeWidgetItem * item = _lab.paramsTree->topLevelItem(0);
	_generateButton->setEnabled(false);
	while(item)
	{
		float minValue = 1.0f;
		float maxValue = 1.0f;
		float stepValue = 1.0f;

		// number
		if(!item->text(eMin).isEmpty())
		{
			minValue = item->text(eMin).toFloat();
			maxValue = item->text(eMax).toFloat();
			stepValue = item->text(eStep).toFloat();
			if(minValue>maxValue)
			{
				_lab.statusLabel->setText("Incorrect value range for parameter"+item->text(eName));
				_numExperiments = 1;
				return;
			}
			_numExperiments *= 1+((maxValue-minValue)/stepValue);
		}
		item = _lab.paramsTree->itemBelow(item);
	}
	_lab.statusLabel->setText("Ready to generate: "+QString::number(_numExperiments*_lab.numRepeats->value())+" experiments");
	_generateButton->setEnabled(true);
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

bool Laboratory::parseConfig()
{	
	TiXmlDocument doc(_simulationConfig.c_str());
	if (!doc.LoadFile())
	{
		QMessageBox msgBox;
		msgBox.setText("Unable to open config file");
		msgBox.exec();
		return false;
	}
	TiXmlHandle hDoc(&doc);
	TiXmlHandle hRoot(0);
    
	TiXmlElement * root = doc.FirstChildElement( "config" );
	if(!root)
	{
		QMessageBox msgBox;
		msgBox.setText("Bad formatted Pandora config file");
		msgBox.exec();
		return false;
	}
	_lab.paramsTree->clear();
	parseLevel(root, 0);
	return true;
}

void Laboratory::parseAttributes(TiXmlElement * parent, QTreeWidgetItem * parentItem )
{
	TiXmlAttribute * attribute = 0;
	for(attribute=parent->FirstAttribute(); attribute!=0; attribute=attribute->Next())
	{
		QTreeWidgetItem * item = new QTreeWidgetItem();
		item->setText(eName, attribute->Name());

		bool ok = false;
		QString minValue(attribute->Value());
		float valueF = minValue.toFloat(&ok);
		if(ok)
		{
			item->setText(eMin, QString::number(valueF));
			item->setText(eMax, QString::number(valueF));
			item->setText(eStep, QString::number(1));
		}
		else
		{
			item->setText(eText, attribute->Value());
		}
		parentItem->addChild(item);
		item->setExpanded(true);
		item->setFlags(item->flags() | Qt::ItemIsEditable);
	}
}

void Laboratory::parseLevel( TiXmlNode * parent, QTreeWidgetItem * parentItem)
{
	TiXmlNode * child;
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
	if(column==eText)
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
	updateNumberOfExecutions();
}

void Laboratory::doubleClick(QTreeWidgetItem * item, int column)
{
	if(column==eName)
	{
		return;
	}
	// a number
	if(item->text(eText).isEmpty() && column==eText)
	{
		return;
	}
	// not a number
	if(column!=eText)
	{
		return;
	}
	_lab.paramsTree->editItem(item, column);
}

} // namespace GUI

