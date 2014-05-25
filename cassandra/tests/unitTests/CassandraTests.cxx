
#include <CassandraTests.hxx>

#include <TimeSeriesModel.hxx>

namespace Tests 
{

void CassandraTests::timeSeriesModelLoadsParamNames()
{
    GUI::TimeSeriesModel model;
    model.loadGroupFile("../data/EDA/groupResults.csv");
    QCOMPARE(int(model.paramNames().size()),2);

    QCOMPARE(model.paramNames().at(0).c_str(),"width_size");
    QCOMPARE(model.paramNames().at(1).c_str(),"value_numAgents");
}

void CassandraTests::timeSeriesModelLoadsResultNames()
{
    GUI::TimeSeriesModel model;
    model.loadGroupFile("../data/EDA/groupResults.csv");
    QCOMPARE(int(model.resultNames().size()),4);
    
    QCOMPARE(model.resultNames().at(0).c_str(),"Sum exists");
    QCOMPARE(model.resultNames().at(1).c_str(),"Mean resources");
    QCOMPARE(model.resultNames().at(2).c_str(),"Mean x");
    QCOMPARE(model.resultNames().at(3).c_str(),"Mean y");
}

void CassandraTests::timeSeriesModelLoadsTimeSteps()
{
    GUI::TimeSeriesModel model;
    model.loadGroupFile("../data/EDA/groupResults.csv");
    QCOMPARE(int(model.timeSteps().size()),11);
    for(size_t i=0; i<model.timeSteps().size(); i++)
    {
        QCOMPARE(int(model.timeSteps().at(i)),int(i));
    }
}

void CassandraTests::timeSeriesModelLoadParams()
{
    GUI::TimeSeriesModel model;
    model.loadGroupFile("../data/EDA/groupResults.csv");

    // total runs
    QCOMPARE(int(model.numRuns()),100);
    // particular data
    QCOMPARE(float(model.params(25, 0)),80.0f);
    QCOMPARE(float(model.params(48, 1)),10.0f);
}

void CassandraTests::timeSeriesModelLoadResults()
{
    GUI::TimeSeriesModel model;
    model.loadGroupFile("../data/EDA/groupResults.csv");

    // particular data
    QCOMPARE(float(model.results(14, 0, 8)), 5.00f);
    QCOMPARE(float(model.results(14, 1, 5)), 13.40f);
    QCOMPARE(float(model.results(14, 2, 0)), 4.20f);
    QCOMPARE(float(model.results(14, 3, 10)), 5.20f);
}

} // namespace Tests 

QTEST_MAIN(Tests::CassandraTests)

