
#include <SurveyConfig.hxx>

namespace BattleSim
{

SurveyConfig::SurveyConfig( const std::string & xmlFile ) : Config(xmlFile), _battleFile(""), _distance(5), _horizontal(true)
{
}
  
SurveyConfig::~SurveyConfig()
{
}

void SurveyConfig::loadParams()
{
    _battleFile = getParamStr("battle", "file");
    _distance = getParamInt("survey", "distance");
    _horizontal = getParamBool("survey", "horizontal");
}
	
} // namespace BattleSim

