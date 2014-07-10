
#include <ShpLoader.hxx>
#include <Exception.hxx>

#include <ogrsf_frmts.h>

namespace Engine
{

ShpLoader::ShpLoader() : _dataSource(0), _activeLayer(0)
{
}

ShpLoader::~ShpLoader()
{
	if(_dataSource)
	{
		OGRDataSource::DestroyDataSource( _dataSource );
	}
}

void ShpLoader::open( const std::string & file )
{
	OGRRegisterAll();
	_dataSource  = OGRSFDriverRegistrar::Open(file.c_str(), false);
	if(!_dataSource)
	{
		std::stringstream oss;
		oss << "ShpLoader::open - error loading shapefile: " << file;
		throw Exception(oss.str());
		return;
	}
	if(!getNumLayers())
	{
		std::stringstream oss;
		oss << "ShpLoader::open - error loading shapefile: " << file << ", the file does not have any layers";
		throw Exception(oss.str());
		return;
	}
	setActiveLayer(0);
}

int ShpLoader::getNumLayers()
{
	if(!_dataSource)
	{
		return 0;
	}
	return _dataSource->GetLayerCount();
}
	
void ShpLoader::setActiveLayer( int activeLayerIndex )
{
	if(!_dataSource)
	{	
		std::stringstream oss;
		oss << "ShpLoader::setActiveLayer - trying to open layer: " << activeLayerIndex << " while data source not initialized";
		throw Exception(oss.str());
		return;
	}
	_activeLayer = _dataSource->GetLayer(activeLayerIndex);
	_activeLayer->ResetReading();
}

int ShpLoader::getNumFeatures()
{
	if(!_dataSource || !_activeLayer)
	{
		std::stringstream oss;
		oss << "ShpLoader::getNumFeatures - no data source / layer correctly initialized";
		throw Exception(oss.str());
		return -1;
	}
	return _activeLayer->GetFeatureCount();
}
	
Point2D<int> ShpLoader::getPosition( int indexFeature )
{
	if(indexFeature<0 || indexFeature>=getNumFeatures())
	{
		std::stringstream oss;
		oss << "ShpLoader::getPosition - asking for index: " << indexFeature << " out of bounds with num features: " << getNumFeatures();
		throw Exception(oss.str());
		return Point2D<int>(-1,-1);
	}
	OGRFeature * feature = _activeLayer->GetFeature(indexFeature);
	OGRGeometry * geometry = feature->GetGeometryRef();	
	if(!geometry && wkbFlatten(geometry->getGeometryType())!=wkbPoint)
	{	
		std::stringstream oss;
		oss << "ShpLoader::getPosition - can't load point geometry for index: " << indexFeature;
		throw Exception(oss.str());
		return Point2D<int>(-1,-1);
	}
	OGRPoint * point = (OGRPoint *)geometry;
	return Point2D<int>(point->getX(), point->getY());
}

int ShpLoader::getFieldIndex( const std::string & fieldName )
{
	OGRFeatureDefn * definition = _activeLayer->GetLayerDefn();
	return definition->GetFieldIndex(fieldName.c_str());
}

std::string ShpLoader::getFieldAsString( int indexFeature, const std::string & fieldName )
{
	if(indexFeature<0 || indexFeature>=getNumFeatures())
	{	
		std::stringstream oss;
		oss << "ShpLoader::getPosition - asking for index: " << indexFeature << " out of bounds with num features: " << getNumFeatures();
		throw Exception(oss.str());
		return "";
	}

	OGRFeature * feature = _activeLayer->GetFeature(indexFeature);
	return feature->GetFieldAsString(getFieldIndex(fieldName));
}

int ShpLoader::getFieldAsInt( int indexFeature, const std::string & fieldName )
{
	if(indexFeature<0 || indexFeature>=getNumFeatures())
	{
		std::stringstream oss;
		oss << "ShpLoader::getPosition - asking for index: " << indexFeature << " out of bounds with num features: " << getNumFeatures();
		throw Exception(oss.str());
		return -1;
	}

	OGRFeature * feature = _activeLayer->GetFeature(indexFeature);
	return feature->GetFieldAsInteger(getFieldIndex(fieldName));
}

double ShpLoader::getFieldAsFloat( int indexFeature, const std::string & fieldName )
{
	if(indexFeature<0 || indexFeature>=getNumFeatures())
	{
		std::stringstream oss;
		oss << "ShpLoader::getPosition - asking for index: " << indexFeature << " out of bounds with num features: " << getNumFeatures();
		throw Exception(oss.str());
		return -1.0f;
	}

	OGRFeature * feature = _activeLayer->GetFeature(indexFeature);
	return feature->GetFieldAsDouble(getFieldIndex(fieldName));
}

} // namespace Engin

