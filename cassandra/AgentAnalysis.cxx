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

#include <AgentAnalysis.hxx>
#include <boost/filesystem.hpp>
#include <QFileDialog>
#include <QMessageBox>
#include <SimulationRecord.hxx>

#include <TraitAnalysis.hxx>
#include <RunAnalysis.hxx>
#include <AnalysisControlThread.hxx>

#include <AgentMean.hxx>
#include <AgentSum.hxx>
#include <AgentStdDev.hxx>

namespace GUI
{

AgentAnalysis::AgentAnalysis(QWidget * parent ) : QDialog(parent), _sampleRecord(0)
{
	setModal(false);
	_analysis.setupUi(this);
	
	connect(_analysis.baseButton, SIGNAL(clicked()), this, SLOT(selectBaseDir()));
	connect(_analysis.newTrait, SIGNAL(clicked()), this, SLOT(newAnalysis()));
	connect(_analysis.outputButton, SIGNAL(clicked()), this, SLOT(selectOutput()));

	_runButton = _analysis.buttonBox->addButton("Run", QDialogButtonBox::ApplyRole);
	connect(_runButton, SIGNAL(clicked()), this, SLOT(run()));

	_runButton->setEnabled(false);
	_analysis.exploreConfig->widget(1)->setEnabled(false);	
	_analysis.exploreConfig->widget(2)->setEnabled(false);	

	_analysis.paramsTree->header()->setMovable(false);
	_analysis.paramsTree->header()->setResizeMode(0, QHeaderView::ResizeToContents);

	_analysis.outputButton->setEnabled(false);
	_analysis.outputEdit->setEnabled(false);
}

AgentAnalysis::~AgentAnalysis()
{
	if(_sampleRecord)
	{
		delete _sampleRecord;
	}
}

void AgentAnalysis::selectOutput()
{
	QString fileName = QFileDialog::getExistingDirectory(this, tr("Select Output dir"), "");
	if (fileName.isEmpty())
	{
		return;
	}
	_analysis.outputEdit->setText(fileName);
	_runButton->setEnabled(true);
}

void AgentAnalysis::selectBaseDir()
{
	QString fileName = QFileDialog::getExistingDirectory(this, tr("Select Base dir"), "");
	if (fileName.isEmpty())
	{
		return;
	}
	else
	{
		_baseDir = fileName.toStdString();
		_analysis.baseEdit->setText(fileName);
	}

	loadConfigs();
	if(!_sampleRecord)
	{
		_runButton->setEnabled(false);
		_analysis.exploreConfig->widget(1)->setEnabled(false);	
		_analysis.exploreConfig->widget(2)->setEnabled(false);

		_analysis.outputButton->setEnabled(false);
		_analysis.outputEdit->setEnabled(false);
	}
	else
	{
		_runButton->setEnabled(false);
		_analysis.exploreConfig->widget(1)->setEnabled(true);	
		_analysis.exploreConfig->widget(2)->setEnabled(false);

		fillParamsTree();
		
		_analysis.outputButton->setEnabled(true);
		_analysis.outputEdit->setEnabled(true);

	}
}

void AgentAnalysis::fillParamsTree()
{
	_analysis.paramsTree->clear();
	if(!_sampleRecord)
	{
		return;
	}

	boost::filesystem::directory_iterator it(_baseDir);
	bool loaded = false;
	while(!loaded && it!=boost::filesystem::directory_iterator())
	{	
		if(!boost::filesystem::is_directory(it->status()))
		{
			it++;
			continue;
		}

		// look for data file
		std::stringstream oss;
		oss << (*it).path().native() << "/config.xml";
		TiXmlDocument doc(oss.str().c_str());
		if (doc.LoadFile())
		{
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
			parseLevel(root, _analysis.paramsTree->topLevelItem(0));
			loaded = true;
		}
		else
		{
			it++;
		}
	}
	updateNumberOfPermutations();
}

void AgentAnalysis::updateNumberOfPermutations()
{
	for( boost::filesystem::directory_iterator it(_baseDir); it!=boost::filesystem::directory_iterator(); it++ )
	{
		if(!boost::filesystem::is_directory(it->status()))
		{
			continue;
		}

		// look for data file
		std::stringstream oss;
		oss << (*it).path().native() << "/config.xml";
		std::cout << "file: " << oss.str()<< std::endl;
		TiXmlDocument doc(oss.str().c_str());
		if (!doc.LoadFile())
		{
			QMessageBox msgBox;
			msgBox.setText("Unable to open config file in dir: "+QString(oss.str().c_str()));
			msgBox.exec();
			continue;
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
		parseLevelPermutations(root, _analysis.paramsTree->topLevelItem(0));
	}
}

void AgentAnalysis::computePermutations(TiXmlElement * element, QTreeWidgetItem * item)
{
	for(int i=0; i<item->childCount(); i++)
	{
		QTreeWidgetItem * attribute = item->child(i);
		// element , not attribute
		if(attribute->childCount()!=0)
		{
			continue;
		}
		std::string value(element->Attribute(attribute->text(AgentAnalysis::eName).toStdString().c_str()));

		ValuesMap & values = _permutations.find(attribute)->second;
		ValuesMap::iterator it = values.find(value);
		if(it==values.end())
		{
			int permutations = attribute->text(AgentAnalysis::ePermutations).toInt();
			attribute->setText(AgentAnalysis::ePermutations, QString::number(permutations+1));
			// insert
			values.insert(std::make_pair(value, 1));
		}
	}
}

void AgentAnalysis::parseAttributes(TiXmlElement * parent, QTreeWidgetItem * parentItem )
{
	TiXmlAttribute * attribute = 0;
	for(attribute=parent->FirstAttribute(); attribute!=0; attribute=attribute->Next())
	{
		QTreeWidgetItem * item = new QTreeWidgetItem(0);
		item->setText(AgentAnalysis::eName, attribute->Name());
		item->setText(AgentAnalysis::ePermutations, "0");
		item->setCheckState(AgentAnalysis::eUse, Qt::Unchecked);
		item->setText(AgentAnalysis::eField, attribute->Name());
		_permutations.insert(std::make_pair(item, ValuesMap()));
		parentItem->addChild(item);
		item->setExpanded(true);
		item->setFlags(item->flags() | Qt::ItemIsEditable);
	}
}

void AgentAnalysis::parseLevelPermutations( TiXmlElement * element, QTreeWidgetItem * item)
{
	computePermutations(element, item);

	TiXmlElement * newElement = element->FirstChildElement();
	for(int i=0; i<item->childCount(); i++)
	{
		QTreeWidgetItem * newItem = item->child(i);
		if(newItem->childCount()>0)
		{
			parseLevelPermutations(newElement, newItem);
			newElement = newElement->NextSiblingElement();
		}
	}
}
void AgentAnalysis::parseLevel( TiXmlNode * parent, QTreeWidgetItem * parentItem)
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
			_analysis.paramsTree->insertTopLevelItem(0, newItem);
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
	
void AgentAnalysis::loadConfigs()
{
	// take the config of the first simulation
	boost::filesystem::path basePath(_baseDir);  
	for( boost::filesystem::directory_iterator it(_baseDir); it!=boost::filesystem::directory_iterator(); it++ )
	{
		if(!boost::filesystem::is_directory(it->status()))
		{
			continue;
		}

		// look for data file
		std::stringstream oss;
		oss << (*it).path().native() << "/config.xml";
		std::cout << "file: " << oss.str()<< std::endl;
		TiXmlDocument doc(oss.str().c_str());
		if (!doc.LoadFile())
		{
			QMessageBox msgBox;
			msgBox.setText("Unable to open config file in dir: "+QString(oss.str().c_str()));
			msgBox.exec();
			continue;
		}
		TiXmlHandle hDoc(&doc);
		TiXmlHandle hRoot(0);
    
		TiXmlElement * root = doc.FirstChildElement( "config" );
		TiXmlElement * output = root->FirstChildElement("output");
		std::string dataFile = (*it).path().native()+"/"+output->Attribute("resultsFile");

		std::cout << "next experiment with data dir: " << dataFile << std::endl;
		_sampleRecord = new Engine::SimulationRecord(1);

		// TODO load simulations too expensive, it could be faster just checking every data file for agent types
		if(!_sampleRecord->loadHDF5(dataFile))
		{
			QMessageBox msgBox;
			msgBox.setText("Unable to open data file for experiment: "+QString(dataFile.c_str()));
			msgBox.exec();
			delete _sampleRecord;
			return;
		}

		for(Engine::SimulationRecord::AgentTypesMap::const_iterator itType = _sampleRecord->beginTypes(); itType!=_sampleRecord->endTypes(); itType++)
		{
			QString newType(itType->first.c_str());
			if(_analysis.agentTypes->findText(newType)==-1)
			{
				_analysis.agentTypes->addItem(newType, newType);
			}
		}

		// something has been loaded, finish the process
		if(_analysis.agentTypes->count()>0)
		{
			return;
		}
	}

}

void AgentAnalysis::removeAnalysis( QWidget * analysis )
{
	_analysis.analysisLayout->removeWidget(analysis);
	delete analysis;

	if(_analysis.analysisLayout->count()==0)
	{
		_analysis.exploreConfig->widget(2)->setEnabled(false);	
	}
}

void AgentAnalysis::newAnalysis()
{
	TraitAnalysis * traitAnalysis = new TraitAnalysis(this, _sampleRecord, _analysis.agentTypes->currentText().toStdString());
	_analysis.analysisLayout->addWidget(traitAnalysis);
	connect(traitAnalysis, SIGNAL(removeAnalysis(QWidget *)), this, SLOT(removeAnalysis(QWidget *)));
	
	_analysis.exploreConfig->widget(2)->setEnabled(true);
}

void AgentAnalysis::run()
{
	AnalysisControlThread * thread = new AnalysisControlThread(_baseDir, _analysis.agentTypes->currentText().toStdString(), _analysis.outputEdit->text().toStdString());
	
	RunAnalysis * runAnalysis = new RunAnalysis(0);
	
	connect(thread, SIGNAL(nextSimulation()), runAnalysis, SLOT(updateSimulationAnalysis()));
	connect(runAnalysis, SIGNAL(rejected()), thread, SLOT(cancelExecution()));

	for(int i=0; i<_analysis.analysisLayout->count(); i++)
	{
		QWidget * aWidget = _analysis.analysisLayout->itemAt(i)->widget();
		if(aWidget && aWidget->objectName().compare("TraitAnalysis")==0)
		{
			TraitAnalysis * widget = (TraitAnalysis*)aWidget;
			TraitAnalysis::AnalysisType type = widget->getAnalysis();
			std::string trait = widget->getTrait();
			switch(type)
			{
				case TraitAnalysis::eMean:
					thread->addAnalysis(new Analysis::AgentMean(trait));
					break;
				
				case TraitAnalysis::eSum:
					thread->addAnalysis(new Analysis::AgentSum(trait));
					break;
			
				case TraitAnalysis::eStandardDeviation:
					thread->addAnalysis(new Analysis::AgentStdDev(trait));
					break;

				default:
					return;
			}
		}

	}
	std::cout << "compiled analysis to perform" << std::endl;
	
	runAnalysis->init(thread->getNumberOfSimulations());
	runAnalysis->show();
	thread->start();
}

} // namespace GUI

