/*
 * Copyright (c) 2012
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
 */

#ifndef __LoggerBase_hxx__
#define __LoggerBase_hxx__

#include <map>
#include <fstream>

namespace Engine
{

class Logger
{
	typedef std::map <std::string, std::ofstream *> FilesMap;

	FilesMap _files;
	std::string _logsDir;
public:
	Logger();
	virtual ~Logger();
	// message to file
	std::ofstream & log( const std::string & fileName );
	void setLogsDir( const std::string & logsDir );
};

} // namespace Engine

#endif // __LoggerBase_hxx__

