#include <diff-widgets/IDiffWidget.h>

IDiffWidget::IDiffWidget(const QSharedPointer<GitBase>& git, QSharedPointer<SacredTimeline> cache, QWidget* parent)
    : QFrame(parent)
    , mGit(git)
    , mCache(cache)
{
}
