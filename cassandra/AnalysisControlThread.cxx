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

#include <AnalysisControlThread.hxx>
#include <Analysis.hxx>

#include <boost/filesystem.hpp>
#include <tinyxml.h>
#include <QMessageBox>
#include <iomanip>
#include <Output.hxx>

#include <iostream>

namespace GUI
{

AnalysisControlThread::AnalysisControlThread( const std::string & baseDir, const std::string & type, const std::string & outputDir, int resolution, bool isRaster ) : _baseDir(baseDir), _type(type), _outputDir(outputDir), _numberOfSimulations(0), _resolution(resolution), _isRaster(isRaster)
{
	// compute number of simulations to analyse
	for( boost::filesystem::directory_iterator it(_baseDir); it!=boost::filesystem::directory_iterator(); it++ )
	{
		if(!boost::filesystem::is_directory(it->status()))
		{
			continue;
		}
		std::stringstream oss;
		oss << (*it).path().native() << "/config.xml";
		if(boost::filesystem::exists(oss.str()))
		{
			_numberOfSimulations++;
		}
	}
	std::cout << "num sims: " << _numberOfSimulations << std::endl;
}

AnalysisControlThread::~AnalysisControlThread()
{
	if(_output)
	{
		delete _output;
	}
}

void AnalysisControlThread::run()
{
	_cancelExecution = false;
	
	for( boost::filesystem::directory_iterator it(_baseDir); it!=boost::filesystem::directory_iterator(); it++ )
	{
		std::cout << "next sim" << std::endl;
		if(!boost::filesystem::is_directory(it->status()))
		{
			continue;
		}
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
		TiXmlElement * output = root->FirstChildElement("output");
		std::string dataFile = (*it).path().native()+"/"+output->Attribute("resultsFile");

		boost::filesystem::path pathToDir = *it;
		analyseSimulation(dataFile, pathToDir.filename().native());
		std::cout << "simulation analysis done for: " << pathToDir.filename().native() << std::endl;
		emit nextSimulation();
		
		if(_cancelExecution)
		{
			quit();
			return;
		}
	}
}

void AnalysisControlThread::analyseSimulation( const std::string & dataFile, const std::string & fileName)
{
	std::cout << "analysing file: " << dataFile << std::endl;
	Engine::SimulationRecord record(_resolution, true);
	record.loadHDF5(dataFile, _isRaster, !_isRaster);

	std::stringstream oss;
	oss << _outputDir << "/" << fileName << ".csv";
	std::cout << "output to: " << oss.str() << std::endl;

	_output->apply(record, oss.str(), _type);
}

void AnalysisControlThread::setOutput( PostProcess::Output * output )
{
	_output = output;
}

void AnalysisControlThread::cancelExecution()
{
	_cancelExecution = true;
}

int AnalysisControlThread::getNumberOfSimulations()
{
	return _numberOfSimulations;
}

} // namespace GUI

