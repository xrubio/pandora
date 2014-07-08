
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
 * 
 */

#ifndef __Logger_hxx__
#define __Logger_hxx__


#include <GeneralState.hxx>

// Extreme debug activated if pandora is compiled in edebug and edebug=1
#ifdef PANDORAEDEBUG 
#define log_EDEBUG(file, message) Engine::GeneralState::logger().log(file) << message << std::endl;
#else
#define log_EDEBUG(file, message)
#endif

// DEBUG activated if pandora is compiled in debug
#ifdef PANDORADEBUG 
#define log_DEBUG(file, message) Engine::GeneralState::logger().log(file) << message << std::endl;
#else
#define log_DEBUG(file, message)
#endif

#define log_INFO(file, message) Engine::GeneralState::logger().log(file) << message << std::endl;

#endif // __Logger_hxx__

