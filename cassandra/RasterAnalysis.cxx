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

#include <RasterAnalysis.hxx>
#include <boost/filesystem.hpp>
#include <QFileDialog>
#include <QMessageBox>
#include <SimulationRecord.hxx>
#include <QCheckBox>

#include <RasterAnalysisSelection.hxx>
#include <RunAnalysis.hxx>
#include <AnalysisControlThread.hxx>

#include <RasterMean.hxx>
#include <RasterSum.hxx>
//#include <RasterStdDev.hxx>
/*
#include <IndividualStats.hxx>
#include <AgentHDFtoSHP.hxx>
*/

namespace GUI
{

RasterAnalysis::RasterAnalysis(QWidget * parent ) : QDialog(parent), _sampleRecord(0), _groups(0)
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
	/*
	_analysis.analysisTypes->addItem("Individual agent analysis", eIndividual);
	_analysis.analysisTypes->addItem("Histogram", eHistogram);
	_analysis.analysisTypes->addItem("Geospatial reference", eGeospatial);
	*/

	_analysis.globalStatistics->hide();
	_analysis.groupParams->hide();
	_analysis.individualStats->hide();
}

RasterAnalysis::~RasterAnalysis()
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

void RasterAnalysis::selectOutput()
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

void RasterAnalysis::selectBaseDir()
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

void RasterAnalysis::selectBaseConfig()
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

void RasterAnalysis::fillParamsTree()
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

void RasterAnalysis::updateNumberOfPermutations()
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

void RasterAnalysis::computePermutations(TiXmlElement * element, QTreeWidgetItem * item)
{
	for(int i=0; i<item->childCount(); i++)
	{
		QTreeWidgetItem * attribute = item->child(i);
		// element , not attribute
		if(attribute->childCount()!=0)
		{
			continue;
		}
		std::string value(element->Attribute(attribute->text(RasterAnalysis::eName).toStdString().c_str()));

		ValuesMap & values = _permutations.find(attribute)->second;
		ValuesMap::iterator it = values.find(value);
		if(it==values.end())
		{
			int permutations = attribute->text(RasterAnalysis::ePermutations).toInt();
			attribute->setText(RasterAnalysis::ePermutations, QString::number(permutations+1));
			// insert
			values.insert(std::make_pair(value, 1));
		}
	}
}

void RasterAnalysis::parseAttributes(TiXmlElement * parent, QTreeWidgetItem * parentItem )
{
	TiXmlAttribute * attribute = 0;
	for(attribute=parent->FirstAttribute(); attribute!=0; attribute=attribute->Next())
	{
		QTreeWidgetItem * item = new QTreeWidgetItem(0);
		item->setText(RasterAnalysis::eName, attribute->Name());
		item->setText(RasterAnalysis::ePermutations, "0");
		item->setCheckState(RasterAnalysis::eUse, Qt::Unchecked);
		item->setText(RasterAnalysis::eField, attribute->Name());
		_permutations.insert(std::make_pair(item, ValuesMap()));
		parentItem->addChild(item);
		item->setExpanded(true);
		item->setFlags(item->flags() | Qt::ItemIsEditable);
	}
}

void RasterAnalysis::parseLevelPermutations( TiXmlElement * element, QTreeWidgetItem * item)
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
void RasterAnalysis::parseLevel( TiXmlNode * parent, QTreeWidgetItem * parentItem)
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

bool RasterAnalysis::loadConfig( const std::string & configDir )
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
	_analysis.rasterTypes->clear();
	_sampleRecord = new Engine::SimulationRecord(1);

	// TODO load simulations too expensive, it could be faster just checking every data file for raster types
	if(!_sampleRecord->loadHDF5(dataFile, true, false))
	{	
		QMessageBox msgBox;
		msgBox.setText("Unable to open data file for experiment: "+QString(dataFile.c_str()));
		msgBox.exec();
		delete _sampleRecord;
		_sampleRecord = 0;
		return false;
	}

	QStringList rasters;
	for(Engine::SimulationRecord::RasterMap::const_iterator it=_sampleRecord->beginRasters(); it!=_sampleRecord->endRasters(); it++)
	{
		rasters<< QString(it->first.c_str());
	}
	_analysis.rasterTypes->addItems(rasters);

	if(_analysis.rasterTypes->count()>0)
	{
		_analysis.baseConfigEdit->setText(oss.str().c_str());
		return true;
	}
	return false;
}
	
void RasterAnalysis::loadConfigs()
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

void RasterAnalysis::removeAnalysis( QWidget * analysis )
{
	_analysis.globalStatistics->layout()->removeWidget(analysis);
	delete analysis;

	if(_analysis.globalStatistics->layout()->count()==0)
	{
		_analysis.exploreConfig->widget(2)->setEnabled(false);	
	}
}

void RasterAnalysis::newAnalysis()
{
	RasterAnalysisSelection * selection = new RasterAnalysisSelection(this);
	_analysis.globalStatistics->layout()->addWidget(selection);
	connect(selection, SIGNAL(removeAnalysis(QWidget *)), this, SLOT(removeAnalysis(QWidget *)));
}

void RasterAnalysis::addGlobalAnalysis( AnalysisControlThread* thread )
{
	PostProcess::GlobalRasterStats * global = new PostProcess::GlobalRasterStats();
	global->setAnalysisOwnership(false);
	for(int i=0; i<_analysis.globalStatistics->layout()->count(); i++)
	{
		QWidget * aWidget = _analysis.globalStatistics->layout()->itemAt(i)->widget();
		if(aWidget && aWidget->objectName().compare("RasterAnalysisSelection")==0)
		{
			RasterAnalysisSelection * widget = (RasterAnalysisSelection*)aWidget;
			RasterAnalysisSelection::GlobalAnalysis type = widget->getAnalysis();
			switch(type)
			{
				case RasterAnalysisSelection::eMean:
					global->addAnalysis(std::shared_ptr<PostProcess::RasterAnalysis>(new PostProcess::RasterMean()));
					break;
				
				case RasterAnalysisSelection::eSum:
					global->addAnalysis(std::shared_ptr<PostProcess::RasterAnalysis>(new PostProcess::RasterSum()));
					break;
		
					/*
				case RasterAnalysisSelection::eStandardDeviation:
					global->addAnalysis(new PostProcess::RasterStdDev(rasterName));
					break;
					*/

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

void RasterAnalysis::groupGlobalStats( PostProcess::GlobalRasterStats * global )
{
	std::list<QTreeWidgetItem * > params;
	QTreeWidgetItem * item = _analysis.paramsTree->topLevelItem(0);
	while(item!=0)
	{
		int checked = item->checkState(RasterAnalysis::eUse);
		if(checked==Qt::Checked)
		{
			std::cout << "group by param: " << item->text(RasterAnalysis::eName).toStdString() << std::endl;
			params.push_back(item);
		}
		item = _analysis.paramsTree->itemBelow(item);
	}

	if(_groups)
	{
		delete _groups;
	}

	_groups = new PostProcess::GlobalRasterStats::Params();

	for(std::list<QTreeWidgetItem * >::iterator it=params.begin(); it!=params.end(); it++)
	{
		QTreeWidgetItem * item = *it;
		std::list< std::string > aGrouping;
		while(item!=0)
		{			
			aGrouping.push_back(item->text(RasterAnalysis::eName).toStdString());
			item = item->parent();
		}
		_groups->push_back(aGrouping);
	}
	global->setParams(_groups, _outputDir+"/groupResults.csv", _baseDir, _sampleRecord->getNumSteps(), _sampleRecord->getFinalResolution());
}
		
/*
void RasterAnalysis::addIndividualStats(AnalysisControlThread* thread )
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

void RasterAnalysis::addHistogram( AnalysisControlThread* thread )
{
	int numStep = _sampleRecord->getNumSteps();
	if(!_analysis.lastStep->isChecked())
	{
		numStep = _analysis.step->value();
	}
	PostProcess::RasterHistogram * histogram = new PostProcess::RasterHistogram(_analysis.attributes->currentText().toStdString(), _analysis.interval->value(), numStep);
	thread->setOutput(histogram);
}

void RasterAnalysis::addGeoreference( AnalysisControlThread* thread )
{
	PostProcess::RasterHDFtoSHP * shp = new PostProcess::RasterHDFtoSHP(Engine::Point2D<int>(_analysis.west->value(), _analysis.north->value()), _analysis.resolution->value(), _analysis.datum->text().toStdString());

	thread->setOutput(shp);
}
*/

void RasterAnalysis::run()
{
	AnalysisControlThread * thread = new AnalysisControlThread(_baseDir, _analysis.rasterTypes->currentText().toStdString(), _outputDir, _analysis.resolution->value(), true);
	
	RunAnalysis * runAnalysis = new RunAnalysis(0);
	
	connect(thread, SIGNAL(nextSimulation()), runAnalysis, SLOT(updateSimulationAnalysis()));
	connect(runAnalysis, SIGNAL(rejected()), thread, SLOT(cancelExecution()));

	AnalysisType analysis = (AnalysisType)_analysis.analysisTypes->itemData(_analysis.analysisTypes->currentIndex()).toInt();
	switch(analysis)
	{	
		case eGlobal:
			addGlobalAnalysis(thread);
			break;
			/*
		case eIndividual:
			addIndividualStats(thread);
			break;
		case eHistogram:
			addHistogram(thread);
			break;
		case eGeospatial:
			addGeoreference(thread);
			break;
			*/
		default:
			return;
	}
	runAnalysis->init(thread->getNumberOfSimulations());
	runAnalysis->show();
	thread->start();
}
	
/*
void RasterAnalysis::fillIndividualStats()
{	
	std::string type = _analysis.agentTypes->currentText().toStdString();
	Engine::SimulationRecord::RasterRecordsMap::const_iterator it=_sampleRecord->beginRasters(type);

	QVBoxLayout * layout = new QVBoxLayout;
	Engine::RasterRecord * agentRecord = it->second; 
	for(Engine::RasterRecord::StatesMap::const_iterator itS=agentRecord->beginStates(); itS!=agentRecord->endStates(); itS++)
	{
		QCheckBox * box = new QCheckBox(QString(itS->first.c_str()));
		layout->addWidget(box);
	}
	layout->addStretch(1);
	delete _analysis.individualStats->layout();
    _analysis.individualStats->setLayout(layout);
}

void RasterAnalysis::fillHistogram()
{
	std::string type = _analysis.agentTypes->currentText().toStdString();
	Engine::SimulationRecord::RasterRecordsMap::const_iterator it=_sampleRecord->beginRasters(type);
	Engine::RasterRecord * agentRecord = it->second;
	
	QStringList traits;
	for(Engine::RasterRecord::StatesMap::const_iterator itS=agentRecord->beginStates(); itS!=agentRecord->endStates(); itS++)
	{
		traits << QString(itS->first.c_str());
	}
	_analysis.attributes->clear();
	_analysis.attributes->addItems(traits);
	_analysis.step->setMaximum(_sampleRecord->getNumSteps()-1);
}
*/

void RasterAnalysis::analysisTypeChosen( int index )
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
			/*
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
			*/
		default:
			return;
	}
	_analysis.exploreConfig->setCurrentIndex(2);
}

void RasterAnalysis::lastStepChanged( int checked )
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

