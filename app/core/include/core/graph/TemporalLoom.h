#pragma once

#include <core/graph/StateTracker.h>
#include <core/graph/Timeline.h>

#include <QString>
#include <QVector>

namespace Graph
{
class TemporalLoom
{
public:
   TemporalLoom();

   Timeline createTimeline(const QString &sha, const QStringList &parents);

private:
   int add(StateType type, const QString &next, int pos);

   int activeLane = 0;
   StateTracker mStateTracker;
   Timeline mTimeline;

   bool isFork(const QString &sha);
   void setFork(const QString &sha);
   void setMerge(const QStringList &parents);
   void setInitial();
   void changeActiveLane(const QString &sha);
   void afterMerge();
   void afterFork();
   bool isBranch();
   void afterBranch();
};
}
