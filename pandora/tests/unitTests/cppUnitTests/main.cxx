
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE "Pandora Sequential Unit Tests"

#include "TestAgent.hxx"
#include "TestWorld.hxx"
#include <Simulation.hxx>
#include <World.hxx>
#include <Point2D.hxx>
#include <Size.hxx>
#include <ShpLoader.hxx>
#include <GeneralState.hxx>

#include <boost/test/unit_test.hpp>

namespace Test
{

BOOST_AUTO_TEST_SUITE( PandoraBasicUse )

BOOST_AUTO_TEST_CASE( testEqualityPoint ) 
{
	Engine::Point2D<int> point1(4,5);
	Engine::Point2D<int> point2(2,5);

	BOOST_CHECK(point1!=point2);
	point2._x = 4;
	BOOST_CHECK_EQUAL(point1, point2);
	point2._y = 100;
	BOOST_CHECK(point1!=point2);
}

BOOST_AUTO_TEST_CASE( testSimulationSize ) 
{	
	Engine::Simulation mySimulation(Engine::Size<int>(10,10), 1);
	Engine::Size<int> size(9,10);
	
	BOOST_CHECK(size!=mySimulation.getSize());
	size._width = 10;
	BOOST_CHECK(size==mySimulation.getSize());
}

BOOST_AUTO_TEST_CASE( testExecuteTwoWorlds ) 
{	
	Engine::Simulation mySimulation(Engine::Size<int>(10,10), 1);
	TestWorld myWorld(mySimulation, TestWorld::useSpacePartition("data/results.h5", 1, false));
	myWorld.initialize(boost::unit_test::framework::master_test_suite().argc, boost::unit_test::framework::master_test_suite().argv);
	myWorld.run();

	TestWorld myWorld2(mySimulation, TestWorld::useSpacePartition("data/results.h5", 1, false));
	myWorld2.initialize(boost::unit_test::framework::master_test_suite().argc, boost::unit_test::framework::master_test_suite().argv);
	myWorld2.run();
}

BOOST_AUTO_TEST_CASE( testAgentRemovedIsNotExecuted ) 
{	
	Engine::Simulation mySimulation(Engine::Size<int>(10,10), 1);
	TestWorld myWorld(mySimulation, TestWorld::useSpacePartition("data/results.h5", 1, false));
	myWorld.initialize(boost::unit_test::framework::master_test_suite().argc, boost::unit_test::framework::master_test_suite().argv);

	TestAgent * myAgent = new TestAgent("agent_0");
	myWorld.addAgent(myAgent);
	myAgent->setRandomPosition();
	BOOST_CHECK_EQUAL(myAgent->exists(), true);
	myAgent->remove();
	BOOST_CHECK_EQUAL(myAgent->exists(), false);
	myWorld.run();
}

BOOST_AUTO_TEST_CASE( testAgentRemovedIsNotInInsideNeighbours ) 
{
	Engine::Simulation mySimulation(Engine::Size<int>(10,10), 1);
	TestWorld myWorld(mySimulation, TestWorld::useSpacePartition("data/results.h5", 1, true));
	myWorld.initialize(boost::unit_test::framework::master_test_suite().argc, boost::unit_test::framework::master_test_suite().argv);

	TestAgent * myAgent0 = new TestAgent("agent_0");
	TestAgent * myAgent1 = new TestAgent("agent_1");
	TestAgent * myAgent2 = new TestAgent("agent_2");
	myWorld.addAgent(myAgent0);
	myWorld.addAgent(myAgent1);
	myWorld.addAgent(myAgent2);
	myAgent0->setRandomPosition();
	myAgent1->setRandomPosition();
	myAgent2->setRandomPosition();

	Engine::AgentsVector neighbours = myWorld.getNeighbours(myAgent0, 20);
	BOOST_CHECK_EQUAL(neighbours.size(), 2);
	myAgent1->remove();
	myAgent2->remove();
	Engine::AgentsVector neighbours2 = myWorld.getNeighbours(myAgent0, 20);
	BOOST_CHECK_EQUAL(neighbours2.size(), 0);

}

BOOST_AUTO_TEST_CASE( testRectangleEquals ) 
{
	Engine::Rectangle<int> aRectangle(5,10,20,30);
	Engine::Rectangle<int> bRectangle(Engine::Size<int>(1,1), Engine::Point2D<int>(3,7));

	BOOST_CHECK(aRectangle!=bRectangle);

	bRectangle._origin = Engine::Point2D<int>(5,10);
	BOOST_CHECK(aRectangle!=bRectangle);

	bRectangle._size = Engine::Size<int>(16,21);
	BOOST_CHECK(aRectangle==bRectangle);

}

BOOST_AUTO_TEST_CASE( testRectangleContainsPoint ) 
{
	Engine::Rectangle<int> aRectangle(0,0,20,10);
	Engine::Point2D<int> aPoint(-1,-1);
	BOOST_CHECK_EQUAL(aRectangle.contains(aPoint), false);
	
	aPoint._x = 0;
	BOOST_CHECK_EQUAL(aRectangle.contains(aPoint), false);

	aPoint._y = 0;
	BOOST_CHECK_EQUAL(aRectangle.contains(aPoint), true);

	aPoint._x = 20;
	BOOST_CHECK_EQUAL(aRectangle.contains(aPoint), true);

	aPoint._y = 10;
	BOOST_CHECK_EQUAL(aRectangle.contains(aPoint), true);

	aPoint._x = 21;
	BOOST_CHECK_EQUAL(aRectangle.contains(aPoint), false);
}

BOOST_AUTO_TEST_CASE( testRectangleIterator ) 
{
	Engine::Rectangle<int> aRectangle(Engine::Size<int>(5,4), Engine::Point2D<int>(5,7));
	Engine::Point2D<int> sum(0,0);
	
	for(auto coordinate : aRectangle )
	{
		sum = sum + coordinate;
	}
	BOOST_CHECK_EQUAL(sum._x, 140);
	BOOST_CHECK_EQUAL(sum._y, 170);
}

BOOST_AUTO_TEST_CASE( testLoadShapefile ) 
{      
    Engine::ShpLoader & loader = Engine::GeneralState::shpLoader();
    loader.open("../../resources/test.shp");
    loader.setActiveLayer(0);

    BOOST_CHECK_EQUAL(4, loader.getNumFeatures());
    BOOST_CHECK_EQUAL(Engine::Point2D<int>(32,22), loader.getPosition(3));
    BOOST_CHECK_EQUAL("label d", loader.getFieldAsString(3,"label"));
    BOOST_CHECK_EQUAL(4, loader.getFieldAsInt(3,"intValue"));
    BOOST_CHECK_CLOSE(4.5f,loader.getFieldAsFloat(3,"floatValue"), 0.00001f);
}


BOOST_AUTO_TEST_SUITE_END()

} // namespace Test

