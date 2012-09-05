
#include <ShpWorld.hxx>
#include <ShpAgent.hxx>

#include <ShpLoader.cxx>
#include <GeneralState.cxx>
#include <Simulation.hxx>

namespace Examples 
{

ShpWorld::ShpWorld( const Engine::Simulation & simulation, const std::string & shpFile ) : World(simulation, 1, false, "data/shp.h5"), _shpFile(shpFile)
{
}

ShpWorld::~ShpWorld()
{
}

void ShpWorld::createRasters()
{
}

void ShpWorld::createAgents()
{
	Engine::ShpLoader & loader = Engine::GeneralState::shpLoader();
	loader.open(_shpFile);
	std::cout << "selecting layer 0 from: " << loader.getNumLayers() << std::endl;;
	loader.setActiveLayer(0);

	for(int i=0; i<loader.getNumFeatures(); i++)
	{
		std::ostringstream oss;
		oss << "ShpAgent_" << loader.getFieldAsString(i, "name");
		ShpAgent * newAgent = new ShpAgent(oss.str());
		Engine::Point2D<int> position = loader.getPosition(i);
		// we reverse y position, that in GIS usually works from max (North) to min (South)
		position._y = getSimulation().getSize() - position._y;
		newAgent->setPosition(position);
		newAgent->setLabel( loader.getFieldAsString(i, "label"));
		newAgent->setIntValue( loader.getFieldAsInt(i, "intValue"));
		newAgent->setFloatValue( loader.getFieldAsFloat(i, "floatValue"));

		std::cout << "loading agent num: " << i << " - " << newAgent << " string label: " << newAgent->getLabel() << " int value: " << newAgent->getIntValue() << " float value: " << newAgent->getFloatValue() << std::endl;
		addAgent(newAgent);
	}
}

} // namespace Examples

