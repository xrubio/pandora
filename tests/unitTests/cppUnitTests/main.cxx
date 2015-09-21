
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE "Pandora Sequential Unit Tests"

#include "TestAgent.hxx"
#include "TestWorld.hxx"
#include <Config.hxx>
#include <World.hxx>
#include <Point2D.hxx>
#include <Size.hxx>
#include <ShpLoader.hxx>
#include <GeneralState.hxx>
#include <Exception.hxx>

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
	Engine::Config myConfig(Engine::Size<int>(10,10), 1);
	Engine::Size<int> size(9,10);
	
	BOOST_CHECK(size!=myConfig.getSize());
	size._width = 10;
	BOOST_CHECK(size==myConfig.getSize());
}

BOOST_AUTO_TEST_CASE( testExecuteTwoWorlds ) 
{	
	TestWorld myWorld(new Engine::Config(Engine::Size<int>(10,10), 1), TestWorld::useSpacePartition(1, false));
	myWorld.initialize(boost::unit_test::framework::master_test_suite().argc, boost::unit_test::framework::master_test_suite().argv);
	myWorld.run();

	TestWorld myWorld2(new Engine::Config(Engine::Size<int>(10,10), 1), TestWorld::useSpacePartition(1, false));
	myWorld2.initialize(boost::unit_test::framework::master_test_suite().argc, boost::unit_test::framework::master_test_suite().argv);
	myWorld2.run();
}

BOOST_AUTO_TEST_CASE( testAgentRemovedIsNotExecuted ) 
{	
	TestWorld myWorld(new Engine::Config(Engine::Size<int>(10,10), 1), TestWorld::useSpacePartition(1, false));
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
	TestWorld myWorld(new Engine::Config(Engine::Size<int>(10,10), 1), TestWorld::useSpacePartition(1, false));
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
    myWorld.run();

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

    BOOST_CHECK_EQUAL(4, loader.getNumFeatures());
    BOOST_CHECK_EQUAL(Engine::Point2D<int>(32,22), loader.getPosition(3));
    BOOST_CHECK_EQUAL("label d", loader.getFieldAsString(3,"label"));
    BOOST_CHECK_EQUAL(4, loader.getFieldAsInt(3,"intValue"));
    BOOST_CHECK_CLOSE(4.5f,loader.getFieldAsFloat(3,"floatValue"), 0.00001f);
}

BOOST_AUTO_TEST_CASE( testGetUnknownRasterThrowsException) 
{      
	TestWorld myWorld(new Engine::Config(Engine::Size<int>(10,10), 1), TestWorld::useSpacePartition(1, false));
	myWorld.initialize(boost::unit_test::framework::master_test_suite().argc, boost::unit_test::framework::master_test_suite().argv);
    myWorld.run();
    try
    {
        myWorld.getDynamicRaster("unknown");
        BOOST_FAIL("getDynamicRaster by key - unexistent raster should raise an exception");
    }
    catch(Engine::Exception & exception)
    {

    }
    try
    {
        myWorld.getStaticRaster("unknown");
        BOOST_FAIL("getStaticRaster by key - unexistent raster should raise an exception");
    }
    catch(Engine::Exception & exception)
    {

    } 
    try
    {
        myWorld.getStaticRaster(1000);
        BOOST_FAIL("getStaticRaster by index - unexistent raster should raise an exception");
    }
    catch(Engine::Exception & exception)
    {

    } 
    try
    {
        myWorld.getDynamicRaster(1000);
        BOOST_FAIL("getDynamicRaster by index - unexistent raster should raise an exception");
    }
    catch(Engine::Exception & exception)
    {

    }


}

BOOST_AUTO_TEST_CASE( testGetUnknownBetaDistributionThrowsException) 
{      
    try
    {
				double value = Engine::GeneralState::statistics().getBetaDistributionValue("test");
				BOOST_FAIL("Statistics::getBetaDistributionValue - ask value from unknown beta distribution: test");
    }
    catch(Engine::Exception & exception)
    {

    }
}

BOOST_AUTO_TEST_CASE( testBetaDistributionReturnsValueWithinBounds ) 
{      
		Engine::GeneralState::statistics().addBetaDistribution("test",2.0,2.0,3.0);
		double value = Engine::GeneralState::statistics().getBetaDistributionValue("test");
		BOOST_CHECK_LE(value,3.0);
		BOOST_CHECK_GE(value,0.0);
}

BOOST_AUTO_TEST_CASE( testLoadStaticRaster ) 
{
    Engine::StaticRaster aRaster;
	Engine::GeneralState::rasterLoader().fillGDALRaster(aRaster, "../../resources/test.tiff");
    BOOST_CHECK_EQUAL(120, aRaster.getSize()._width);
    BOOST_CHECK_EQUAL(120, aRaster.getSize()._height);
    BOOST_CHECK_EQUAL(139, aRaster.getValue(Engine::Point2D<int>(39,39)));
}

BOOST_AUTO_TEST_CASE( testLoadDynamicRaster ) 
{
    Engine::DynamicRaster aRaster;
	Engine::GeneralState::rasterLoader().fillGDALRaster(aRaster, "../../resources/test.tiff");
    BOOST_CHECK_EQUAL(120, aRaster.getSize()._width);
    BOOST_CHECK_EQUAL(120, aRaster.getSize()._height);
    BOOST_CHECK_EQUAL(139, aRaster.getValue(Engine::Point2D<int>(39,39)));
}

BOOST_AUTO_TEST_CASE( testAddAgent) 
{
	TestWorld myWorld(new Engine::Config(Engine::Size<int>(10,10), 1), TestWorld::useSpacePartition(1, false));
	myWorld.initialize(boost::unit_test::framework::master_test_suite().argc, boost::unit_test::framework::master_test_suite().argv);

	TestAgent * myAgent0 = new TestAgent("agent_0");
	myWorld.addAgent(myAgent0);
	myAgent0->setRandomPosition();
    BOOST_CHECK_EQUAL(&myWorld, myAgent0->getWorld());
}
BOOST_AUTO_TEST_SUITE_END()

} // namespace Test

