
#include <RemoveBulletConfig.hxx>

namespace BattleSim
{

RemoveBulletConfig::RemoveBulletConfig( const std::string & xmlFile ) : Config(xmlFile), _battleFile(""), _lossProbability(0.0f)
{
}
  
RemoveBulletConfig::~RemoveBulletConfig()
{
}

void RemoveBulletConfig::loadParams()
{
    _battleFile = getParamStr("battle", "file");
    _lossProbability = getParamFloat("battle", "lossProbability");
}
	
} // namespace BattleSim

