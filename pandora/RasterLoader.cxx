
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
	logName << "RasterLoader_world_" << world->getId();
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

	Size<int> size = Size<int>(dataset->GetRasterXSize(), dataset->GetRasterYSize());
	if(size!=simulation.getSize())
	{
		std::stringstream oss;
		oss << "RasterLoader::fillGDALRaster - file: " << fileName << " with size: " << size << " different from defined size: " << simulation.getSize() << std::endl;
		throw Engine::Exception(oss.str());
	}
	log_DEBUG(logName.str(), "size of raster: " << size);

	raster.resize(world->getBoundaries()._size);
	log_DEBUG(logName.str(), "resize done with value: " << world->getBoundaries()._size);

	GDALRasterBand * band = dataset->GetRasterBand(1);
	double minMaxValues[2];
	int hasMin, hasMax;
	minMaxValues[0] = band->GetMinimum(&hasMin);
	minMaxValues[1] = band->GetMaximum(&hasMax);
	if(!(hasMin && hasMax))
	{
		GDALComputeRasterMinMax((GDALRasterBandH)band, TRUE, minMaxValues);
	}
	
	float * pafScanline = (float *)CPLMalloc(sizeof(float)*(world->getBoundaries()._size._width*world->getBoundaries()._size._height));

	band->RasterIO( GF_Read, world->getBoundaries()._origin._x, world->getBoundaries()._origin._y, world->getBoundaries()._size._width, world->getBoundaries()._size._height, pafScanline, world->getBoundaries()._size._width, world->getBoundaries()._size._height, GDT_Float32, 0, 0 );

	log_DEBUG(logName.str(), "raster IO done");
	const Rectangle<int> & boundaries = world->getBoundaries();
	log_DEBUG(logName.str(), "boundaries of world: " << boundaries);

	Point2D<int> index;

	for(index._x=boundaries._origin._x; index._x<boundaries._origin._x+boundaries._size._width; index._x++)
	{
		for(index._y=boundaries._origin._y; index._y<boundaries._origin._y+boundaries._size._height; index._y++)
		{
			Point2D<int> index2(index - boundaries._origin);
			log_EDEBUG(logName.str(), "index: " << index << " and index2: " << index2 << " accessing to: " << boundaries._size._width*index2._y+index2._x);
			raster._values[index2._x][index2._y] = (int)(pafScanline[boundaries._size._width*index2._y+index2._x]);
			log_EDEBUG(logName.str(), "value in index2: " << index2 << " is: " << (int)(pafScanline[boundaries._size._width*index2._y+index2._x]));
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

	// load color table if possible
	GDALColorTable * colorTable = band->GetColorTable();
	if(colorTable)
	{
		raster.setColorTable(true, colorTable->GetColorEntryCount());
		for(int i=0; i<colorTable->GetColorEntryCount(); i++)
		{ 	
			GDALColorEntry newEntry;
			if(colorTable->GetColorEntryAsRGB(i, &newEntry))
			{
				raster.addColorEntry(i, newEntry.c1, newEntry.c2, newEntry.c3, newEntry.c4);
				//std::cout << "entry: " <<i << " color: " << newEntry.c1 << "/" << newEntry.c2 << "/" << newEntry.c3 << " alpha: " << newEntry.c4 << std::endl;
			}
		}
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
		logName << "RasterLoader_world_" << world->getId();
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
	
	if(world && (dims[0]!=world->getSimulation().getSize()._width || dims[1]!=world->getSimulation().getSize()._height))
	{
		std::stringstream oss;
		oss << "RasterLoader::fillHDF5Raster - file: " << fileName << " and raster name: " << rasterName << " with size: " << dims[0] << "/" << dims[1] << " different from defined size: " << world->getSimulation().getSize() << std::endl;
		throw Engine::Exception(oss.str());
	}
	
	int * dset_data = 0;
	if(world)
	{
		dset_data = (int*)malloc(sizeof(int)*world->getBoundaries()._size._width*world->getBoundaries()._size._height);
		raster.resize(world->getBoundaries()._size);
	}
	else
	{
		dset_data = (int*)malloc(sizeof(int)*dims[0]*dims[1]);
		// squared
		raster.resize(Size<int>(dims[0], dims[1]));
	}

	H5Dread(dset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, dset_data);
	H5Dclose(dset_id);

	if(world)
	{
		const Rectangle<int> & boundaries = world->getBoundaries();
		Point2D<int> index;
		for(index._x=boundaries._origin._x; index._x<boundaries._origin._x+boundaries._size._width; index._x++)
		{
			for(index._y=boundaries._origin._y; index._y<boundaries._origin._y+boundaries._size._height; index._y++)
			{
				Point2D<int> index2(index - boundaries._origin);
				int index = boundaries._size._height*index2._y+index2._x;
				raster._values[index2._x][index2._y] = (int)dset_data[index];
			}
		}
	}
	else
	{
		for(size_t i=0; i<dims[0]; i++)
		{
			for(size_t j=0; j<dims[1]; j++)
			{
				size_t index = i+j*dims[0];
				raster._values[i][j] = dset_data[index];	
			}
		}
	}
	free(dset_data);

	std::ostringstream oss2;
	oss2 << "/colorTables/" << rasterName;
	hid_t colorTableId = H5Dopen(fileId, oss2.str().c_str(), H5P_DEFAULT);
	hid_t colorTableSpaceId = H5Dget_space(colorTableId);
	hsize_t colorTableDims[2];
	H5Sget_simple_extent_dims(colorTableSpaceId, colorTableDims, NULL);

	//std::cout << "raster: " << rasterName << " has color table of size: " << colorTableDims[0] << "/" << colorTableDims[1] << std::endl;
	// color table
	if(colorTableDims[0]!=0)
	{
		raster.setColorTable(true, colorTableDims[0]);
		int * colors = (int*)malloc(sizeof(int)*colorTableDims[0]*colorTableDims[1]);
		H5Dread(colorTableId, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, colors);
		for(size_t i=0; i<colorTableDims[0]; i++)
		{
			size_t index = colorTableDims[1]*i;
			raster.addColorEntry(i, short(colors[index]), short(colors[index+1]), short(colors[index+2]), short(colors[index+3]));
			//std::cout << "entry: " << i << " colors: " << short(colors[index]) << "/" << short(colors[index+1]) << "/" << short(colors[index+2]) << " alpha: " << short(colors[index+3]) << std::endl;
		}
		free(colors);
	}

	H5Sclose(colorTableSpaceId);
	H5Dclose(colorTableId);


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
	logName << "RasterLoader_world_" << world->getId();
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
	if(world && (G_window_cols()!=world->getSimulation().getSize()._width || G_window_rows()!=world->getSimulation().getSize()._height))
	{
		std::stringstream oss;
		oss << "StaticRaster::loadGrassCellRasterFile - Grass raster: " << rasterName << " with size: " << G_window_cols() << "/" << G_window_rows() << " different from defined size: " << world->getSimulation().getSize() << std::endl;
		throw Engine::Exception(oss.str());
		return;
	}

	if(world)
	{
		raster.resize(world->getBoundaries()._size);
	}
	else
	{
		raster.resize(Engine::Size<int>(G_window_rows(), G_window_cols()));
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
				if(world->getBoundaries().isInside(cellPosition))
				{
					Point2D<int> localPosition(cellPosition - world->getBoundaries()._origin);
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

