
#ifndef __CassandraTests_hxx__
#define __CassandraTests_hxx__

#include <QtTest/QtTest>

namespace Tests
{

class CassandraTests : public QObject
{
    Q_OBJECT

private slots:
    void timeSeriesModelLoadsParams();
    void timeSeriesModelLoadsResults();
    void timeSeriesModelLoadsTimeSteps();
};

} // namespace  Tests

#endif // __CassandraTests_hxx__

