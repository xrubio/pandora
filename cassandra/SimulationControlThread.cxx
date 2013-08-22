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

#include <SimulationControlThread.hxx>
#include <sstream>
#include <cstdlib>
#include <wait.h>
#include <iomanip>
#include <boost/filesystem.hpp>

namespace GUI
{

SimulationControlThread::SimulationControlThread( const std::string & simulationBinary, const std::string & outputDir, int totalExperiments ) : _simulationBinary(simulationBinary), _outputDir(outputDir), _totalExperiments(totalExperiments)
{
}

SimulationControlThread::~SimulationControlThread()
{

}

void SimulationControlThread::run()
{
	_cancelExecution = false;
	int status = 0;
	bool finished = false;

	for(int i=0; i<_totalExperiments; i++)
	{
		std::stringstream workingDirectory;
		workingDirectory << _outputDir << "/run_" << std::setfill('0') << std::setw(4) << i;
		boost::filesystem::current_path(workingDirectory.str());

		int pid_ps = fork();
		if(pid_ps == 0)
		{
			execv(_simulationBinary.c_str(), 0);
		}
		else
		{
			int status = 0;
			bool finished = false;
			while(!finished)
			{
				if(_cancelExecution)
				{
					finished = true;
					kill(pid_ps, SIGKILL);
				}
				else
				{
					pid_t result = waitpid(pid_ps, &status, WNOHANG|WUNTRACED);
					if(result==pid_ps)
					{
						finished = true;
					}
					else
					{
						sleep(100);
					}
				}
			}
		}
		if(_cancelExecution)
		{
			quit();
			return;
		}
		emit nextSimulation();
	}
}

void SimulationControlThread::cancelExecution()
{
	_cancelExecution = true;
}
	
} // namespace GUI

