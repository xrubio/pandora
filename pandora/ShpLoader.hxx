
#ifndef __ShpLoader_hxx__
#define __ShpLoader_hxx__

#include <string>

#include <Point2D.hxx>

class OGRDataSource;
class OGRLayer;

namespace Engine
{

class ShpLoader
{
	// shapefile source
	OGRDataSource * _dataSource;
	// layer to be loaded
	OGRLayer * _activeLayer;

	// returns the index of field which name is fieldName inside _activeLayer 
	int getFieldIndex( const std::string & fieldName );
public:
	ShpLoader();
	virtual ~ShpLoader();

	// this method opens the shapefile in file and allocates its info to _dataSource, selecting the first layer
	// throws an exception if the file can't be opened or there are no layers
	void open( const std::string & file );

	// returns the number of layers (0 if no active _dataSource
	int getNumLayers();
	// sets the layer to be read. thows an exception if no active _dataSource
	void setActiveLayer( int activeLayerIndex );
	// returns the number of features in layer _activeLayer from source _dataSource. throws an exception of layer or datasource were not loaded
	int getNumFeatures();
	
	// field related getters
	// returns the geometry of feature in position indexFeature. Throws an exception if index out of range
	Point2D<int> getPosition( int indexFeature );

	// returns the field fieldName of of feature in position indexFeature as string. Throws an exception if index out of range or fieldName does not exist
	std::string getFieldAsString( int indexFeature, const std::string & fieldName );
	// returns the field fieldName of feature in position indexFeature as integer. Throws an exception if index out of range or fieldName does not exist
	int getFieldAsInt( int indexFeature, const std::string & fieldName );
	// returns the field fieldName of feature in position indexFeature as double. Throws an exception if index out of range or fieldName does not exist
	double getFieldAsFloat( int indexFeature, const std::string & fieldName );
};

} // namespace Engine

#endif // __ShpLoader_hxx__

