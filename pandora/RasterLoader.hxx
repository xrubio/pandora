
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

#ifndef __RasterLoader_hxx__
#define __RasterLoader_hxx__

#include <string>

namespace Engine
{

class StaticRaster;
class World;

class RasterLoader
{
public:
	RasterLoader();
	virtual ~RasterLoader();

	// load a GDAL file conforming World position
	void fillGDALRaster( StaticRaster & raster, const std::string & fileName, World * world );
	// load an HDF5 conforming adjusting raster to data, or to World position if not null
	void fillHDF5Raster( StaticRaster & raster, const std::string & fileName, const std::string & rasterName, World * world = 0);
	// load a raster file from a GRASS database conforming adjusting raster to data, or to World position if not null
	// Simulation MUST be executed from a Grass instance, and raster must exist in current location/mapset
	void fillGrassCellRaster( StaticRaster & raster, const std::string & rasterName, World * world = 0);
}; 

} // namespace Engine

#endif // __RasterLoader_hxx__

