#pragma once

#include <QCoreApplication>

class TestsGraphCache : public QObject
{
   Q_OBJECT

private:
   struct Relationship
   {
      QString sha;
      QStringList parents;
   };

private slots:
   void initTestCase();
   void cleanupTestCase();
   void test_buildTemporalLoom();
   void test_buildTemporalLoom2();
   void test_buildTemporalLoom3();
   void test_buildTemporalLoom4();
   void test_buildTemporalLoomInfiniteColumns();
};
