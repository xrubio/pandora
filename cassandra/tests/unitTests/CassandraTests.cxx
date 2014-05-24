
#include <CassandraTests.hxx>

#include <TimeSeriesModel.hxx>

namespace Tests 
{

void CassandraTests::timeSeriesModelLoadsParams()
{
    GUI::TimeSeriesModel model;
    model.loadGroupFile("../data/EDA/groupResults.csv");
    QCOMPARE(int(model.params().size()),2);

    QCOMPARE(model.params().at(0).c_str(),"width_size");
    QCOMPARE(model.params().at(1).c_str(),"value_numAgents");
}

void CassandraTests::timeSeriesModelLoadsResults()
{
    GUI::TimeSeriesModel model;
    model.loadGroupFile("../data/EDA/groupResults.csv");
    QCOMPARE(int(model.results().size()),4);
    
    QCOMPARE(model.results().at(0).c_str(),"Sum exists");
    QCOMPARE(model.results().at(1).c_str(),"Mean resources");
    QCOMPARE(model.results().at(2).c_str(),"Mean x");
    QCOMPARE(model.results().at(3).c_str(),"Mean y");
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

} // namespace Tests 

QTEST_MAIN(Tests::CassandraTests)

