
#include <Site.hxx>
#include <Statistics.hxx>
#include <GeneralState.hxx>
#include <World.hxx>
#include <CultureConfig.hxx>

namespace Examples
{

Site::Site( const std::string & id, float numFeatures, float numTraits ) : Agent(id), _traits( numTraits)
{
    _features.resize(numFeatures);
    for( size_t i=0; i<_features.size(); i++)
    {
        _features.at(i) = Engine::GeneralState::statistics().getUniformDistValue(0, numTraits-1);
    }
}

Site::~Site()
{
}

void Site::updateState()
{
    const CultureConfig & cultureConfig = (const CultureConfig &)_world->getConfig();
    // probability of transmission
    if(!Engine::GeneralState::statistics().getUniformDistValue()>cultureConfig._transmission)
    {
        return;
    }
    Engine::AgentsVector neighbours = _world->getNeighbours(this, cultureConfig._range);
    float similarity = getSimilarity(neighbours.at(0));
    // too different
    if(Engine::GeneralState::statistics().getUniformDistValue()>similarity)
    {
        return;
    }
    transmitFeature(neighbours.at(0));
}

void Site::transmitFeature( Engine::AgentPtr agent )
{
    Site & site = (Site &)(*agent);
    std::vector<int> differentFeatures;
    for(size_t i=0; i<_features.size(); i++)
    {
        if(site.getFeature(i)==_features.at(i))
        {
            continue;
        }
        differentFeatures.push_back(i);
    }
    // identical
    if(differentFeatures.size()==0)
    {
        return;
    }
    std::random_shuffle(differentFeatures.begin(), differentFeatures.end());
    _features.at(differentFeatures.at(0)) = site.getFeature(differentFeatures.at(0));
}

void Site::registerAttributes()
{
	registerFloatAttribute("similarity");
	registerStringAttribute("cultureStr");
}

void Site::serialize()
{
	serializeAttribute("cultureStr", getCultureString());
	serializeAttribute("similarity", getSimilarityNeighbors());
}

std::string Site::getCultureString() const
{
    std::stringstream oss;
    for(size_t i=0; i<_features.size(); i++)
    {
        oss << _features.at(i);
    }
    return oss.str();
}

float Site::getSimilarityNeighbors()
{
    const CultureConfig & cultureConfig = (const CultureConfig &)_world->getConfig();
    float sumSim = 0.0f;
    Engine::AgentsVector neighbours = _world->getNeighbours(this, cultureConfig._range);

    for( auto agent : neighbours)
    {
        sumSim += getSimilarity(agent);
    }
    return sumSim/float(neighbours.size());
}

float Site::getSimilarity( Engine::AgentPtr agent )
{
    Site & site = (Site &)(*agent);
    const CultureConfig & cultureConfig = (const CultureConfig &)_world->getConfig();

    float simNeighbour = 0.0f;
    for( size_t i=0; i<_features.size(); i++)
    {
        if(cultureConfig._useDistance)
        {
            float diff = fabs(site.getFeature(i) - _features.at(i));
            diff /= float(cultureConfig._traitsPerFeature);
            simNeighbour += 1.0f - diff;
        }
        else
        {
            if(site.getFeature(i)==_features.at(i))
            {
                simNeighbour += 1.0f;
            }
        }
    }
    return simNeighbour/float(_features.size());
}

int Site::getFeature( size_t index ) const
{
    return _features.at(index);
}

} // namespace Examples

