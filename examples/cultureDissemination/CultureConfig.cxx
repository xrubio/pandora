
#include <CultureConfig.hxx>

namespace Examples
{

CultureConfig::CultureConfig( const std::string & xmlFile ) : Config(xmlFile), _features(5), _traitsPerFeature(10), _transmission(0.1f), _range(1.0f)
{
}

CultureConfig::~CultureConfig()
{
}

void CultureConfig::loadParams()
{
	_features = getParamInt( "culture", "features");
	_traitsPerFeature = getParamInt( "culture", "traitsPerFeature");
	_transmission  = getParamFloat( "culture", "transmission");
	_range = getParamFloat( "culture", "range");
	_useDistance = getParamBool( "culture", "useDistance");
}
	
} // namespace Examples

