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
#include <QCheckBox>

#include <TraitAnalysisSelection.hxx>
#include <RunAnalysis.hxx>
#include <AnalysisControlThread.hxx>

#include <AgentMean.hxx>
#include <AgentSum.hxx>
#include <AgentStdDev.hxx>
#include <IndividualStats.hxx>
#include <AgentHistogram.hxx>
#include <AgentHDFtoSHP.hxx>

namespace GUI
{

AgentAnalysis::AgentAnalysis(QWidget * parent ) : QDialog(parent), _sampleRecord(0), _groups(0)
{
	setModal(false);
	_analysis.setupUi(this);
	
	connect(_analysis.baseButton, SIGNAL(clicked()), this, SLOT(selectBaseDir()));
	connect(_analysis.baseConfigButton, SIGNAL(clicked()), this, SLOT(selectBaseConfig()));
	connect(_analysis.newTrait, SIGNAL(clicked()), this, SLOT(newAnalysis()));
	connect(_analysis.outputButton, SIGNAL(clicked()), this, SLOT(selectOutput()));

	_runButton = _analysis.buttonBox->addButton("Run", QDialogButtonBox::ApplyRole);
	connect(_runButton, SIGNAL(clicked()), this, SLOT(run()));
	_analysis.exploreConfig->setEnabled(false);

	_runButton->setEnabled(false);
	_analysis.exploreConfig->widget(1)->setEnabled(false);	
	_analysis.exploreConfig->widget(2)->setEnabled(false);	

	_analysis.paramsTree->header()->setMovable(false);
	_analysis.paramsTree->header()->setResizeMode(0, QHeaderView::ResizeToContents);

	_analysis.outputButton->setEnabled(false);
	_analysis.outputEdit->setEnabled(false);

	connect(_analysis.analysisTypes, SIGNAL(currentIndexChanged(int)), this, SLOT(analysisTypeChosen(int)));
	connect(_analysis.lastStep, SIGNAL(stateChanged(int)), this, SLOT(lastStepChanged(int)));
	// analysis types
	_analysis.analysisTypes->addItem("Global analysis", eGlobal);
	_analysis.analysisTypes->addItem("Individual agent analysis", eIndividual);
	_analysis.analysisTypes->addItem("Histogram", eHistogram);
	_analysis.analysisTypes->addItem("Geospatial reference", eGeospatial);

	_analysis.globalStatistics->hide();
	_analysis.groupParams->hide();
	_analysis.individualStats->hide();
}

AgentAnalysis::~AgentAnalysis()
{
	if(_sampleRecord)
	{
		delete _sampleRecord;
	}

	if(_groups)
	{
		delete _groups;
	}
}

void AgentAnalysis::selectOutput()
{
	QString fileName = QFileDialog::getExistingDirectory(this, tr("Select Output dir"), "");
	if (fileName.isEmpty())
	{
		return;
	}
	_outputDir = fileName.toStdString();

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

	_analysis.outputEdit->setText(fileName);
	_runButton->setEnabled(true);
}

void AgentAnalysis::selectBaseDir()
{
	QString fileName = QFileDialog::getExistingDirectory(this, tr("Select Base dir"), "");
	if (fileName.isEmpty())
	{
		_analysis.exploreConfig->setEnabled(false);
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
		_analysis.exploreConfig->widget(2)->setEnabled(true);

		fillParamsTree();
		analysisTypeChosen(eGlobal);
		
		_analysis.outputButton->setEnabled(true);
		_analysis.outputEdit->setEnabled(true);
		
		_analysis.exploreConfig->setEnabled(true);
		_analysis.exploreConfig->setCurrentWidget(_analysis.page_1);
	}
}

void AgentAnalysis::selectBaseConfig()
{
	QString fileName = QFileDialog::getExistingDirectory(this, tr("Select simulation to use as template"), "");
	if (fileName.isEmpty())
	{
		return;
	}
	else
	{
		loadConfig(fileName.toStdString());
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

bool AgentAnalysis::loadConfig( const std::string & configDir )
{		
	// look for data file
	std::stringstream oss;
	oss << configDir << "/config.xml";

	TiXmlDocument doc(oss.str().c_str());
	if (!doc.LoadFile())
	{
		QMessageBox msgBox;
		msgBox.setText("Unable to open config file in dir: "+QString(oss.str().c_str()));
		msgBox.exec();
		return false;
	}
	TiXmlHandle hDoc(&doc);
	TiXmlHandle hRoot(0);
   
	TiXmlElement * root = doc.FirstChildElement( "config" );
	TiXmlElement * output = root->FirstChildElement("output");
	std::string dataFile = configDir+"/"+output->Attribute("resultsFile");

	std::cout << "next experiment with data dir: " << dataFile << std::endl;
	if(_sampleRecord)
	{
		delete _sampleRecord;
	}
	_analysis.agentTypes->clear();
	_sampleRecord = new Engine::SimulationRecord(1);

	// TODO load simulations too expensive, it could be faster just checking every data file for agent types
	if(!_sampleRecord->loadHDF5(dataFile, false, true))
	{
		QMessageBox msgBox;
		msgBox.setText("Unable to open data file for experiment: "+QString(dataFile.c_str()));
		msgBox.exec();
		delete _sampleRecord;
		_sampleRecord = 0;
		return false;
	}
	for(Engine::SimulationRecord::AgentTypesMap::const_iterator itType = _sampleRecord->beginTypes(); itType!=_sampleRecord->endTypes(); itType++)
	{
		QString newType(itType->first.c_str());
		if(_analysis.agentTypes->findText(newType)==-1)
		{
			_analysis.agentTypes->addItem(newType, newType);
		}
	}


	if(_analysis.agentTypes->count()>0)
	{
		_analysis.baseConfigEdit->setText(oss.str().c_str());
		return true;
	}
	return false;
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


		if(loadConfig((*it).path().native()))
		{
			return;
		}

	}
}

void AgentAnalysis::removeAnalysis( QWidget * analysis )
{
	_analysis.globalStatistics->layout()->removeWidget(analysis);
	delete analysis;

	if(_analysis.globalStatistics->layout()->count()==0)
	{
		_analysis.exploreConfig->widget(2)->setEnabled(false);	
	}
}

void AgentAnalysis::newAnalysis()
{
	TraitAnalysisSelection * selection = new TraitAnalysisSelection(this, _sampleRecord, _analysis.agentTypes->currentText().toStdString());
	_analysis.globalStatistics->layout()->addWidget(selection);
	connect(selection, SIGNAL(removeAnalysis(QWidget *)), this, SLOT(removeAnalysis(QWidget *)));
}

void AgentAnalysis::addGlobalAnalysis( AnalysisControlThread* thread )
{
	PostProcess::GlobalAgentStats * global = new PostProcess::GlobalAgentStats();
	global->setAnalysisOwnership(false);
	for(int i=0; i<_analysis.globalStatistics->layout()->count(); i++)
	{
		QWidget * aWidget = _analysis.globalStatistics->layout()->itemAt(i)->widget();
		if(aWidget && aWidget->objectName().compare("TraitAnalysisSelection")==0)
		{
			TraitAnalysisSelection * widget = (TraitAnalysisSelection*)aWidget;
			TraitAnalysisSelection::GlobalAnalysis type = widget->getAnalysis();
			std::string trait = widget->getTrait();
			switch(type)
			{
				case TraitAnalysisSelection::eMean:
					global->addAnalysis(std::shared_ptr<PostProcess::AgentAnalysis>(new PostProcess::AgentMean(trait)));
					break;
				
				case TraitAnalysisSelection::eSum:
					global->addAnalysis(std::shared_ptr<PostProcess::AgentAnalysis>(new PostProcess::AgentSum(trait)));
					break;
			
				case TraitAnalysisSelection::eStandardDeviation:
					global->addAnalysis(std::shared_ptr<PostProcess::AgentAnalysis>(new PostProcess::AgentStdDev(trait)));
					break;

				default:
					return;
			}
		}
	}

	if(_analysis.groupParams->isChecked())
	{
		groupGlobalStats(global);
	}
	thread->setOutput(global);
}

void AgentAnalysis::groupGlobalStats( PostProcess::GlobalAgentStats * global )
{
	std::list<QTreeWidgetItem * > params;
	QTreeWidgetItem * item = _analysis.paramsTree->topLevelItem(0);
	while(item!=0)
	{
		int checked = item->checkState(AgentAnalysis::eUse);
		if(checked==Qt::Checked)
		{
			std::cout << "group by param: " << item->text(AgentAnalysis::eName).toStdString() << std::endl;
			params.push_back(item);
		}
		item = _analysis.paramsTree->itemBelow(item);
	}

	if(_groups)
	{
		delete _groups;
	}

	_groups = new PostProcess::GlobalAgentStats::Params();

	for(std::list<QTreeWidgetItem * >::iterator it=params.begin(); it!=params.end(); it++)
	{
		QTreeWidgetItem * item = *it;
		std::list< std::string > aGrouping;
		while(item!=0)
		{			
			aGrouping.push_back(item->text(AgentAnalysis::eName).toStdString());
			item = item->parent();
		}
		_groups->push_back(aGrouping);
	}
	global->setParams(_groups, _outputDir+"/groupResults.csv", _baseDir, _sampleRecord->getNumSteps(), _sampleRecord->getFinalResolution());
}
			
void AgentAnalysis::addIndividualStats(AnalysisControlThread* thread )
{
	PostProcess::IndividualStats * results = new PostProcess::IndividualStats(_sampleRecord->getNumSteps()/_sampleRecord->getSerializedResolution());
	for(int i=0; i<_analysis.individualStats->layout()->count(); i++)
	{
		QCheckBox * box = (QCheckBox*)_analysis.individualStats->layout()->itemAt(i)->widget();
		if(box->isChecked())
		{
			results->addAttribute(box->text().toStdString());
		}
	}
	thread->setOutput(results);
}

void AgentAnalysis::addHistogram( AnalysisControlThread* thread )
{
	int numStep = _sampleRecord->getNumSteps();
	if(!_analysis.lastStep->isChecked())
	{
		numStep = _analysis.step->value();
	}
	PostProcess::AgentHistogram * histogram = new PostProcess::AgentHistogram(_analysis.attributes->currentText().toStdString(), _analysis.interval->value(), numStep);
	thread->setOutput(histogram);
}

void AgentAnalysis::addGeoreference( AnalysisControlThread* thread )
{
	PostProcess::AgentHDFtoSHP * shp = new PostProcess::AgentHDFtoSHP(Engine::Point2D<int>(_analysis.west->value(), _analysis.north->value()), _analysis.resolution->value(), _analysis.datum->text().toStdString());

	thread->setOutput(shp);
}


void AgentAnalysis::run()
{
	AnalysisControlThread * thread = new AnalysisControlThread(_baseDir, _analysis.agentTypes->currentText().toStdString(), _outputDir, _analysis.resolution->value(), false);
	
	RunAnalysis * runAnalysis = new RunAnalysis(0);
	
	connect(thread, SIGNAL(nextSimulation()), runAnalysis, SLOT(updateSimulationAnalysis()));
	connect(runAnalysis, SIGNAL(rejected()), thread, SLOT(cancelExecution()));

	AnalysisType analysis = (AnalysisType)_analysis.analysisTypes->itemData(_analysis.analysisTypes->currentIndex()).toInt();
	switch(analysis)
	{	
		case eGlobal:
			addGlobalAnalysis(thread);
			break;
		case eIndividual:
			addIndividualStats(thread);
			break;
		case eHistogram:
			addHistogram(thread);
			break;
		case eGeospatial:
			addGeoreference(thread);
			break;
		default:
			return;
	}
	runAnalysis->init(thread->getNumberOfSimulations());
	runAnalysis->show();
	thread->start();
}
			
void AgentAnalysis::fillIndividualStats()
{	
	std::string type = _analysis.agentTypes->currentText().toStdString();
	Engine::SimulationRecord::AgentRecordsMap::const_iterator it=_sampleRecord->beginAgents(type);

	QVBoxLayout * layout = new QVBoxLayout;
	Engine::AgentRecord * agentRecord = it->second; 
	for(Engine::AgentRecord::IntAttributesMap::const_iterator itS=agentRecord->beginInt(); itS!=agentRecord->endInt(); itS++)
	{
		QCheckBox * box = new QCheckBox(QString(itS->first.c_str()));
		layout->addWidget(box);
	}
    for(Engine::AgentRecord::FloatAttributesMap::const_iterator itS=agentRecord->beginFloat(); itS!=agentRecord->endFloat(); itS++)
	{
		QCheckBox * box = new QCheckBox(QString(itS->first.c_str()));
		layout->addWidget(box);
	}
    for(Engine::AgentRecord::StrAttributesMap::const_iterator itS=agentRecord->beginStr(); itS!=agentRecord->endStr(); itS++)
	{
		QCheckBox * box = new QCheckBox(QString(itS->first.c_str()));
		layout->addWidget(box);
	}
	layout->addStretch(1);
	delete _analysis.individualStats->layout();
    _analysis.individualStats->setLayout(layout);
}

void AgentAnalysis::fillHistogram()
{
	std::string type = _analysis.agentTypes->currentText().toStdString();
	Engine::SimulationRecord::AgentRecordsMap::const_iterator it=_sampleRecord->beginAgents(type);
	Engine::AgentRecord * agentRecord = it->second;
	
	QStringList traits;
	for(Engine::AgentRecord::IntAttributesMap::const_iterator itS=agentRecord->beginInt(); itS!=agentRecord->endInt(); itS++)
	{
		traits << QString(itS->first.c_str());
	}
    for(Engine::AgentRecord::FloatAttributesMap::const_iterator itS=agentRecord->beginFloat(); itS!=agentRecord->endFloat(); itS++)
	{
		traits << QString(itS->first.c_str());
	}
    for(Engine::AgentRecord::StrAttributesMap::const_iterator itS=agentRecord->beginStr(); itS!=agentRecord->endStr(); itS++)
	{
		traits << QString(itS->first.c_str());
	}
	_analysis.attributes->clear();
	_analysis.attributes->addItems(traits);
	_analysis.step->setMaximum(_sampleRecord->getNumSteps()-1);
}

void AgentAnalysis::analysisTypeChosen( int index )
{
	AnalysisType analysis = (AnalysisType)_analysis.analysisTypes->itemData(index).toInt();
	switch(analysis)
	{
		case eGlobal:
			_analysis.globalStatistics->show();
			_analysis.groupParams->show();
			_analysis.individualStats->hide();
			_analysis.histogram->hide();
			_analysis.georeference->hide();
			break;
		case eIndividual:
			_analysis.globalStatistics->hide();
			_analysis.groupParams->hide();
			_analysis.individualStats->show();
			_analysis.histogram->hide();
			_analysis.georeference->hide();
			fillIndividualStats();
			break;
		case eHistogram:
			_analysis.globalStatistics->hide();
			_analysis.groupParams->hide();
			_analysis.individualStats->hide();
			_analysis.histogram->show();
			_analysis.georeference->hide();
			fillHistogram();
			break;
		case eGeospatial:
			_analysis.globalStatistics->hide();
			_analysis.groupParams->hide();
			_analysis.individualStats->hide();
			_analysis.histogram->hide();
			_analysis.georeference->show();
			break;
		default:
			return;
	}
	_analysis.exploreConfig->setCurrentIndex(2);
}

void AgentAnalysis::lastStepChanged( int checked )
{
	if(checked==Qt::Checked)
	{
		_analysis.step->setEnabled(false);
	}
	else
	{
		_analysis.step->setEnabled(true);
	}
}

} // namespace GUI

