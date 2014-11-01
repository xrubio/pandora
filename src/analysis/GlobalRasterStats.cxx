/*
 * Copyright (c) 2013
 * COMPUTER APPLICATIONS IN SCIENCE & ENGINEERING
 * BARCELONA SUPERCOMPUTING CENTRE - CENTRO NACIONAL DE SUPERCOMPUTACIÓN
 * http://www.bsc.es

 * This file is part of Pandora Library. This library is free software; 
 * you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation;
 * either version 3.0 of the License, or (at your option) any later version.
 * 
 * Pandora is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public 
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 * 
 */

#include <analysis/GlobalRasterStats.hxx>
#include <analysis/Analysis.hxx>
#include <mpi.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <tinyxml.h>
#include <boost/algorithm/string/find.hpp>

namespace PostProcess
{

GlobalRasterStats::GlobalRasterStats( const std::string & separator ) : Output(separator), _analysisOwnership(true), _params(0)
{
}

GlobalRasterStats::~GlobalRasterStats()
{
	if(!_analysisOwnership)
	{
		return;
	}

	if(_params)
	{
		delete _params;
	}
}

void GlobalRasterStats::setAnalysisOwnership( bool analysisOwnership )
{
	_analysisOwnership = analysisOwnership;
}

void GlobalRasterStats::apply( const Engine::SimulationRecord & simRecord, const std::string & outputFile, const std::string & type )
{	
	std::cout << "Executing postprocess: " << getName() << " ...";

	std::ofstream file;
	file.open(outputFile.c_str());
  
	std::stringstream header;
	header << "timeStep";
	for(RasterAnalysisList::const_iterator it=_analysisList.begin(); it!=_analysisList.end(); it++)
	{
		if((*it)->writeResults())
		{
			header << _separator << (*it)->getName();
		}
	}
	file << header.str() << std::endl;;

	for(RasterAnalysisList::const_iterator itL=_analysisList.begin(); itL!=_analysisList.end(); itL++)
	{
		std::cout << "Preprocessing analysis: " << (*itL)->getName() << "...";
		(*itL)->setNumTimeSteps(1+(simRecord.getNumSteps()/simRecord.getFinalResolution()));	
		(*itL)->preProcess();
		std::cout << "done" << std::endl;
		std::cout << "Computing analysis: " << (*itL)->getName() << "...";
		// all agents
		if(type.compare("all")==0)
		{	
			for(Engine::SimulationRecord::RasterMap::const_iterator it=simRecord.beginRasters(); it!=simRecord.endRasters(); it++)
			{
				const Engine::SimulationRecord::RasterHistory & rasterHistory = it->second;
				(*itL)->computeRaster(rasterHistory);
			}
		}
		else
		{
			const Engine::SimulationRecord::RasterHistory & rasterHistory = simRecord.getRasterHistory(type);
			(*itL)->computeRaster(rasterHistory);
		}
		std::cout << "done" << std::endl;
		std::cout << "Postprocessing analysis: " << (*itL)->getName() << "...";
		(*itL)->postProcess();
		std::cout << "done" << std::endl;
	}

	for(int i=0; i<=simRecord.getNumSteps(); i=i+simRecord.getFinalResolution())
	{
		std::stringstream newLine;
		newLine << i;
		for(RasterAnalysisList::const_iterator itL=_analysisList.begin(); itL!=_analysisList.end(); itL++)
		{
			if((*itL)->writeResults())
			{
				newLine  << _separator << std::setprecision(2) << std::fixed << (*itL)->getResult(i/simRecord.getFinalResolution());				
			}
		}
		file << newLine.str() << std::endl;
	}
	file.close();
	std::cout << "done!" << std::endl;
	if(_params)
	{
		std::ofstream groupFile;
		std::cout << "grouping by params" << std::endl;
		groupFile.open(_groupFile.c_str(), std::ios_base::app);
		std::stringstream line;
		// get the text of the folder (between third and second last '/')
		// i.e. foo/run_001/data/data.h5 would return 'run_001'
		boost::iterator_range<std::string::const_iterator> initName = boost::algorithm::find_nth(simRecord.getName(), "/", -3);
		boost::iterator_range<std::string::const_iterator> endName = boost::algorithm::find_nth(simRecord.getName(), "/", -2);
		std::string fileName = std::string(initName.begin()+1, endName.begin());
		// only possible combination that could be wrong
		if(fileName.compare(".")==0)
		{
			initName = boost::algorithm::find_nth(simRecord.getName(), "/", -4);
			endName = boost::algorithm::find_nth(simRecord.getName(), "/", -3);
			fileName = std::string(initName.begin()+1, endName.begin());
		}
		line << fileName;

		writeParams(line, fileName);

		// time series for one attribute
		if(_analysisList.size()==1)
		{
            std::shared_ptr<RasterAnalysis > analysis = *(_analysisList.begin());
			for(int i=0; i<simRecord.getNumSteps()/simRecord.getFinalResolution(); i++)
			{
				line << _separator << std::setprecision(2) << std::fixed << analysis->getResult(i);
			}
		}
		// outcome at the end of simulation for several attributes
		else
		{
			for(RasterAnalysisList::const_iterator itL=_analysisList.begin(); itL!=_analysisList.end(); itL++)
			{
				line  << _separator << std::setprecision(2) << std::fixed << (*itL)->getResult(simRecord.getNumSteps()/simRecord.getFinalResolution());				
			}
		}
		groupFile << line.str() << std::endl;
		groupFile.close();
	}
}

void GlobalRasterStats::writeParams( std::stringstream & line, const std::string & fileName )
{
		std::stringstream configFile;
		unsigned pos = fileName.find_last_of(".");
		configFile << _inputDir << "/" << fileName.substr(0,pos) << "/config.xml";

		TiXmlDocument doc(configFile.str().c_str());
		if (!doc.LoadFile())
		{
			return;
		}
		TiXmlHandle hDoc(&doc);
		TiXmlHandle hRoot(0);

		TiXmlElement * element = 0;
		for(Params::iterator it=_params->begin(); it!=_params->end(); it++)
		{
			std::list<std::string> & paramsList = *it;
			// backwards iteration, the first element is the attribute
			std::list<std::string>::reverse_iterator itL=paramsList.rbegin();
			for(size_t i=0; i<paramsList.size()-1; i++)
			{
				if(!element)
				{
					element = doc.FirstChildElement(*itL);
				}
				else
				{
					element = element->FirstChildElement(*itL);
				}
				itL++;
			}
			TiXmlElement * finalElement = element->ToElement();
			std::string & attributeName = *(paramsList.begin());
			line  << _separator << finalElement->Attribute(attributeName.c_str());
			element = 0;
		}

}

void GlobalRasterStats::addAnalysis( RasterAnalysis * analysis )
{
	_analysisList.push_back(std::shared_ptr<RasterAnalysis>(analysis));
}

void GlobalRasterStats::addAnalysis( std::shared_ptr<RasterAnalysis> analysis )
{
	_analysisList.push_back(analysis);
}

void GlobalRasterStats::setParams( Params * params, const std::string & groupFile, const std::string & inputDir, int numSteps, int resolution)
{
	_params = params;
	_inputDir = inputDir;
	_groupFile = groupFile;
	
	std::ofstream file;
	file.open(_groupFile.c_str());
  
	std::stringstream header;
	header << "run";

	// header will have the name of the field + name of parent (if exists)
	for(Params::iterator it=params->begin(); it!=params->end(); it++)
	{
		header << _separator;
		std::list<std::string> & paramsList = *it;
		std::list<std::string>::iterator itL=paramsList.begin();
		int numEntries = 0;
		while(itL!=paramsList.end())
		{
			header << *itL;

			numEntries++;
			if(numEntries>=2)
			{
				break;
			}
			itL++;
			if(itL!=paramsList.end())
			{
				header << "_";
			}
			
		}
	}

	// results, time series if one attribute
	if(_analysisList.size()==1)
	{	
		for(int i=0; i<=numSteps; i+=resolution)
		{
			header << _separator << "step" << i;
		}
	}
	// outcome at the end of simulation for several attributes
	else
	{
		for(RasterAnalysisList::const_iterator it=_analysisList.begin(); it!=_analysisList.end(); it++)
		{
			if((*it)->writeResults())
			{
				header << _separator << (*it)->getName();
			}
		}
	}
	file << header.str() << std::endl;;
	file.close();
}

std::string GlobalRasterStats::getName() const
{
	return "Global DynamicRaster Stats";
}

} // namespace PostProcess

