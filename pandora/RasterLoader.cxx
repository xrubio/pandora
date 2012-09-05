
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

#include <RasterLoader.hxx>
#include <StaticRaster.hxx>
#include <Raster.hxx>
#include <World.hxx>
#include <Exceptions.hxx>

#include <GeneralState.hxx>
#include <Logger.hxx>

#include <vector>
#include <gdal_priv.h>
#include <hdf5.h>

extern "C" 
{
#include <grass/gis.h>
}

namespace Engine
{

RasterLoader::RasterLoader()
{
}

RasterLoader::~RasterLoader()
{
}

void RasterLoader::fillGDALRaster( StaticRaster & raster, const std::string & fileName, World * world )
{
	std::stringstream logName;
	logName << "RasterLoader_world_" << world->getSimulation().getId();
	log_DEBUG(logName.str(), "loading file: " << fileName);

	Simulation & simulation(world->getSimulation());
	GDALAllRegister();
	GDALDataset * dataset = (GDALDataset *)GDALOpen(fileName.c_str(), GA_ReadOnly );
	
	if(!dataset)
	{
		std::stringstream oss;
		oss << "StaticRaster::loadGDALFile - file: " << fileName << " not found";
		throw Engine::Exception(oss.str());
	}

	int size = dataset->GetRasterXSize();
	if(size!=dataset->GetRasterYSize())
	{
		std::stringstream oss;
		oss << "StaticRaster::loadFile - file: " << fileName << " does not contain an squared raster. width: " << size << " and height: " << dataset->GetRasterYSize();
		throw Engine::Exception(oss.str());
	}
	if(size!=simulation.getSize())
	{
		std::stringstream oss;
		oss << "StaticRaster::loadFile - file: " << fileName << " with size: " << size << " different from defined size: " << simulation.getSize() << std::endl;
		throw Engine::Exception(oss.str());
	}
	log_DEBUG(logName.str(), "size of raster: " << size);

	raster.resize(world->getOverlapBoundaries()._size);
	log_DEBUG(logName.str(), "resize done with value: " << world->getOverlapBoundaries()._size);

	GDALRasterBand * band = dataset->GetRasterBand(1);
	double minMaxValues[2];
	int hasMin, hasMax;
	minMaxValues[0] = band->GetMinimum(&hasMin);
	minMaxValues[1] = band->GetMaximum(&hasMax);
	if(!(hasMin && hasMax))
	{
		GDALComputeRasterMinMax((GDALRasterBandH)band, TRUE, minMaxValues);
	}
	
	float * pafScanline = (float *)CPLMalloc(sizeof(float)*(world->getOverlapBoundaries()._size._x*world->getOverlapBoundaries()._size._y));

	band->RasterIO( GF_Read, world->getOverlapBoundaries()._origin._x, world->getOverlapBoundaries()._origin._y, world->getOverlapBoundaries()._size._x, world->getOverlapBoundaries()._size._y, pafScanline, world->getOverlapBoundaries()._size._x, world->getOverlapBoundaries()._size._y, GDT_Float32, 0, 0 );

	log_DEBUG(logName.str(), "raster IO done");
	const Rectangle<int> & overlapBoundaries = world->getOverlapBoundaries();
	log_DEBUG(logName.str(), "overlap boundaries of world: " << overlapBoundaries);

	Point2D<int> index;

	for(index._x=overlapBoundaries._origin._x; index._x<overlapBoundaries._origin._x+overlapBoundaries._size._x; index._x++)
	{
		for(index._y=overlapBoundaries._origin._y; index._y<overlapBoundaries._origin._y+overlapBoundaries._size._y; index._y++)
		{
			Point2D<int> index2(index - overlapBoundaries._origin);
			log_EDEBUG(logName.str(), "index: " << index << " and index2: " << index2 << " accessing to: " << overlapBoundaries._size._x*index2._y+index2._x);
			int value = (int)(pafScanline[overlapBoundaries._size._x*index2._y+index2._x]);
			log_EDEBUG(logName.str(), "value: " << value);

			raster._values[index2._x][index2._y] = (int)(pafScanline[overlapBoundaries._size._x*index2._y+index2._x]);
			log_EDEBUG(logName.str(), "value in index2: " << index2 << " is: " << value);
		}
	}

	CPLFree(pafScanline);
	log_DEBUG(logName.str(), "done, update minmax values");	
	raster.updateMinMaxValues();
	
	// if dynamic, copy to maxValues
	Raster * dynamicRaster = dynamic_cast<Raster*>(&raster);
	if(dynamicRaster)
	{
		std::copy(dynamicRaster->_values.begin(), dynamicRaster->_values.end(), dynamicRaster->_maxValues.begin());
		dynamicRaster->updateCurrentMinMaxValues();
	}
	log_DEBUG(logName.str(), "finished, closing");	
	GDALClose(dataset);
	log_DEBUG(logName.str(), "done!");	
}

void RasterLoader::fillHDF5Raster( StaticRaster & raster, const std::string & fileName, const std::string & rasterName, World * world )
{
	std::stringstream logName;
	if(world)
	{
		logName << "RasterLoader_world_" << world->getSimulation().getId();
	}
	else
	{
		logName << "RasterLoader";
	}
	log_DEBUG(logName.str(), "loading file: " << fileName << " rasterName: " << rasterName);

	std::ostringstream oss;
	oss << "/" << rasterName << "/values";
	hid_t fileId = H5Fopen(fileName.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
	hid_t dset_id = H5Dopen(fileId, oss.str().c_str(), H5P_DEFAULT);
	hid_t dataspaceId = H5Dget_space(dset_id);
	hsize_t dims[2];
	H5Sget_simple_extent_dims(dataspaceId, dims, NULL);
	H5Sclose(dataspaceId);

	if(dims[0]!=dims[1])
	{
		std::stringstream oss;
		oss << "RasterLoader::fillHDF5Raster - file: " << fileName << " and raster name: " << rasterName << " not squared, with width: " << dims[0] << " and height: " << dims[1] << std::endl;
		throw Engine::Exception(oss.str());
	}
	if(world && dims[0]!=world->getSimulation().getSize())
	{
		std::stringstream oss;
		oss << "RasterLoader::fillHDF5Raster - file: " << fileName << " and raster name: " << rasterName << " with size: " << dims[0] << " different from defined size: " << world->getSimulation().getSize() << std::endl;
		throw Engine::Exception(oss.str());
	}
	
	int * dset_data = 0;
	if(world)
	{
		dset_data = (int*)malloc(sizeof(int)*world->getOverlapBoundaries()._size._x*world->getOverlapBoundaries()._size._y);
		raster.resize(world->getOverlapBoundaries()._size);
	}
	else
	{
		dset_data = (int*)malloc(sizeof(int)*dims[0]*dims[1]);
		// squared
		raster.resize(Point2D<int>(dims[0], dims[1]));
	}

	H5Dread(dset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, dset_data);
	H5Dclose(dset_id);

	if(world)
	{
		const Rectangle<int> & overlapBoundaries = world->getOverlapBoundaries();
		Point2D<int> index;
		for(index._x=overlapBoundaries._origin._x; index._x<overlapBoundaries._origin._x+overlapBoundaries._size._x; index._x++)
		{
			for(index._y=overlapBoundaries._origin._y; index._y<overlapBoundaries._origin._y+overlapBoundaries._size._y; index._y++)
			{
				Point2D<int> index2(index - overlapBoundaries._origin);
				int index = overlapBoundaries._size._y*index2._y+index2._x;
				raster._values[index2._x][index2._y] = (int)dset_data[index];
			}
		}
	}
	else
	{
		for(int i=0; i<dims[0]; i++)
		{
			for(int j=0; j<dims[1]; j++)
			{
				int index = i+j*dims[0];
				raster._values[i][j] = dset_data[index];	
			}
		}
	}
	free(dset_data);
	H5Fclose(fileId);
	raster.updateMinMaxValues();
	
	// if dynamic, copy to maxValues
	Raster * dynamicRaster = dynamic_cast<Raster*>(&raster);
	if(dynamicRaster)
	{
		std::copy(dynamicRaster->_values.begin(), dynamicRaster->_values.end(), dynamicRaster->_maxValues.begin());
		dynamicRaster->updateCurrentMinMaxValues();
	}
	log_DEBUG(logName.str(), "file: " << fileName << " rasterName: " << rasterName << " loaded");
}

void RasterLoader::fillGrassCellRaster( StaticRaster & raster, const std::string & rasterName, World * world )
{
	std::stringstream logName;
	logName << "RasterLoader_world_" << world->getSimulation().getId();
	log_DEBUG(logName.str(), "loading GRASS raster: " << rasterName);
	G_gisinit("pandora");
	std::cout << "loading grass raster from : " << rasterName << " in location: " << G_location() << " and mapset: " << G_mapset() << "...";	
	int fileId = G_open_cell_old(rasterName.c_str(), G_mapset());
	if(fileId==-1)
	{
		std::stringstream oss;
		oss << "StaticRaster::loadGrassCellRasterFile - error opening raster file from GRASS: " << rasterName << std::endl;
		throw Engine::Exception(oss.str());
		return;
	}
	// squared rasters
	if(G_window_cols()!=G_window_rows())
	{
		std::stringstream oss;
		oss << "StaticRaster::loadGrassCellRasterFile - loading raster from GRASS raster: " <<rasterName << "  with num rows: " << G_window_rows() << " and num cols.: " << G_window_cols() << ". Rasters must be squared" << std::endl;
		throw Engine::Exception(oss.str());
		return;
	}
	if(world && G_window_cols()!=world->getSimulation().getSize())
	{
		std::stringstream oss;
		oss << "StaticRaster::loadGrassCellRasterFile - Grass raster: " << rasterName << " with size: " << G_window_cols << " different from defined size: " << world->getSimulation().getSize() << std::endl;
		throw Engine::Exception(oss.str());
		return;
	}

	if(world)
	{
		raster.resize(world->getOverlapBoundaries()._size);
	}
	else
	{
		raster.resize(Engine::Point2D<int>(G_window_rows(), G_window_cols()));
	}

	if(world)
	{
		// TODO use GRASS regions
		std::vector<CELL> rowData;
		rowData.resize(G_window_cols());
		for( int i=0; i<G_window_rows(); i++)
		{
			G_get_c_raster_row(fileId, &rowData[0], i);
			for( int j=0; j<G_window_cols(); j++)
			{
				Engine::Point2D<int> cellPosition(j,i);
				if(world->getOverlapBoundaries().isInside(cellPosition))
				{
					Point2D<int> localPosition(cellPosition - world->getOverlapBoundaries()._origin);
					raster._values[localPosition._x][localPosition._y] = rowData[j];
				}
			}
		}
	}
	else
	{
		for( int i=0; i<G_window_rows(); i++)
		{
			G_get_c_raster_row(fileId, &(raster._values[i][0]), i);
		}
	}
	G_close_cell(fileId);
	raster.updateMinMaxValues();

	// if dynamic, copy to maxValues
	Raster * dynamicRaster = dynamic_cast<Raster*>(&raster);
	if(dynamicRaster)
	{
		std::copy(dynamicRaster->_values.begin(), dynamicRaster->_values.end(), dynamicRaster->_maxValues.begin());
		dynamicRaster->updateCurrentMinMaxValues();
	}
	log_DEBUG(logName.str(), "raster: " << rasterName << " loaded");
}

} // namespace Engine

