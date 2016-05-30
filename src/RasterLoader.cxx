
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
#include <DynamicRaster.hxx>
#include <World.hxx>
#include <Exception.hxx>

#include <GeneralState.hxx>
#include <Logger.hxx>

#include <vector>
#include <gdal_priv.h>

#include <hdf5.h>

namespace Engine
{

RasterLoader::RasterLoader()
{
}

RasterLoader::~RasterLoader()
{
}

void RasterLoader::fillGDALRaster( StaticRaster & raster, const std::string & fileName, const Rectangle<int> & definedBoundaries )
{
	std::stringstream logName;
	logName << "RasterLoader";
	log_DEBUG(logName.str(), "loading file: " << fileName);

	GDALAllRegister();
	GDALDataset * dataset = (GDALDataset *)GDALOpen(fileName.c_str(), GA_ReadOnly );
	
	if(!dataset)
	{
		std::stringstream oss;
		oss << "StaticRaster::loadGDALFile - file: " << fileName << " not found";
		throw Engine::Exception(oss.str());
	}

    Rectangle<int> boundaries = Rectangle<int>(definedBoundaries);
    // no boundaries passed
    if(boundaries._size._width == -1)
    {
        boundaries._origin = Point2D<int>(0,0);
        boundaries._size = Size<int>(dataset->GetRasterXSize(), dataset->GetRasterYSize());
    }
	raster.resize(boundaries._size);

	GDALRasterBand * band = dataset->GetRasterBand(1);
	double minMaxValues[2];
	int hasMin, hasMax;
	minMaxValues[0] = band->GetMinimum(&hasMin);
	minMaxValues[1] = band->GetMaximum(&hasMax);
	if(!(hasMin && hasMax))
	{
		GDALComputeRasterMinMax((GDALRasterBandH)band, TRUE, minMaxValues);
	}
	
	float * pafScanline = (float *)CPLMalloc(sizeof(float)*(boundaries._size._width*boundaries._size._height));
	band->RasterIO( GF_Read, boundaries._origin._x, boundaries._origin._y, boundaries._size._width, boundaries._size._height, pafScanline, boundaries._size._width, boundaries._size._height, GDT_Float32, 0, 0 );

	log_DEBUG(logName.str(), "raster IO done");

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
	DynamicRaster * dynamicRaster = dynamic_cast<DynamicRaster*>(&raster);
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
			}
		}
	}
	log_DEBUG(logName.str(), "finished, closing");	
	GDALClose(dataset);
	log_DEBUG(logName.str(), "done!");	
}

void RasterLoader::fillHDF5RasterDirectPath( StaticRaster & raster, const std::string & fileName, const std::string & pathToData, World * world )
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
	log_DEBUG(logName.str(), "loading file: " << fileName << " pathToData: " << pathToData);


	hid_t fileId = H5Fopen(fileName.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
	hid_t dset_id = H5Dopen(fileId, pathToData.c_str(), H5P_DEFAULT);
	hid_t dataspaceId = H5Dget_space(dset_id);
	hsize_t dims[2];
	H5Sget_simple_extent_dims(dataspaceId, dims, NULL);
	H5Sclose(dataspaceId);
	
	if(world && (dims[0]!=world->getConfig().getSize()._width || dims[1]!=world->getConfig().getSize()._height))
	{
		std::stringstream oss;
		oss << "RasterLoader::fillHDF5RasterDirectPath - file: " << fileName << " and dataset: " << pathToData<< " with size: " << dims[0] << "/" << dims[1] << " different from defined size: " << world->getConfig().getSize() << std::endl;
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
				int index = index2._y+boundaries._size._height*index2._x;
				raster._values[index2._x][index2._y] = (int)dset_data[index];
			}
		}
	}
	else
	{
		size_t index = 0;
		for(size_t y=0; y<dims[0]; y++)
		{
			for(size_t x=0; x<dims[1]; x++)
			{
				raster._values[x][y] = dset_data[index];	
				++index;
			}
		}
	}
	free(dset_data);

    int lastIndex = pathToData.find_last_of("/"); 
    std::string rasterName = pathToData.substr(0, lastIndex); 
	std::ostringstream oss2;
	oss2 << "/colorTables/" << rasterName; 
    
    H5E_auto2_t oldfunc;
    void *old_client_data;
    H5Eget_auto(H5E_DEFAULT, &oldfunc, &old_client_data);


    /* Turn off error handling */
    H5Eset_auto(H5E_DEFAULT, NULL, NULL);
	hid_t colorTableId = H5Dopen(fileId, oss2.str().c_str(), H5P_DEFAULT);
    
    /* Restore previous error handler */
    H5Eset_auto(H5E_DEFAULT, oldfunc, old_client_data);
    std::cout << "checking: " << oss2.str() << " -> " << colorTableId << std::endl;

    if(colorTableId>0)
    {
        hid_t colorTableSpaceId = H5Dget_space(colorTableId);
        hsize_t colorTableDims[2];
        H5Sget_simple_extent_dims(colorTableSpaceId, colorTableDims, NULL);

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
            }
            free(colors);
        }

        H5Sclose(colorTableSpaceId);
        H5Dclose(colorTableId);
    }
	H5Fclose(fileId);
	raster.updateMinMaxValues();
	
	// if dynamic, copy to maxValues
	DynamicRaster * dynamicRaster = dynamic_cast<DynamicRaster*>(&raster);
	if(dynamicRaster)
	{
		std::copy(dynamicRaster->_values.begin(), dynamicRaster->_values.end(), dynamicRaster->_maxValues.begin());
		dynamicRaster->updateCurrentMinMaxValues();
	}
	log_DEBUG(logName.str(), "file: " << fileName << " path to data: " << pathToData << " loaded");
}

void RasterLoader::fillHDF5Raster( StaticRaster & raster, const std::string & fileName, const std::string & rasterName, int step, World * world )
{
	std::ostringstream oss;
	oss << "/" << rasterName << "/step" << step;
    fillHDF5RasterDirectPath(raster, fileName, oss.str(), world);
}

void RasterLoader::fillHDF5Raster( StaticRaster & raster, const std::string & fileName, const std::string & rasterName, World * world )
{	
    std::ostringstream oss;
	oss << "/" << rasterName << "/values";
    fillHDF5RasterDirectPath(raster, fileName, oss.str(), world);
}

} // namespace Engine

