#pragma once

#include <QDateTime>
#include <QStyledItemDelegate>

class GraphView;
class GitCache;
class GitBase;
class Commit;
namespace Graph
{
    class Cache;
    class State;
} // namespace Graph

const int ROW_HEIGHT = 25;
const int LANE_WIDTH = 3 * ROW_HEIGHT / 4;
const int TEXT_HEIGHT_OFFSET = 2;

class GraphViewDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    GraphViewDelegate(
        const QSharedPointer<GitCache>& cache,
        const QSharedPointer<Graph::Cache>& graphCache,
        const QSharedPointer<GitBase>& git,
        GraphView* view);

    void paint(QPainter* p, const QStyleOptionViewItem& o, const QModelIndex& i) const override;
    QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const override;

protected:
    bool
    editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
        override;
    bool
    helpEvent(QHelpEvent* event, QAbstractItemView* view, const QStyleOptionViewItem& option, const QModelIndex& index)
        override;

private:
    QSharedPointer<GitCache> mCache;
    QSharedPointer<Graph::Cache> mGraphCache;
    QSharedPointer<GitBase> mGit;
    GraphView* mView = nullptr;
    mutable QColor mCurrentTextColor;
    mutable QImage mCurrentTagIcon;
    mutable QImage mCurrentLocalBranchIcon;
    mutable QImage mCurrentRemoteBranchIcon;
    int diffTargetRow = -1;
    int mColumnPressed = -1;

    QColor getActiveColor(const Commit& commit) const;
    void paintBranchHelper(
        QPainter* p, const QStyleOptionViewItem& o, const Commit& commit, const QColor& activeColor) const;
    void paintLog(QPainter* p, const QStyleOptionViewItem& o, const Commit& commit) const;
    void paintGraph(QPainter* p, const QStyleOptionViewItem& o, const Commit& commit) const;
    void paintGraphLane(
        QPainter* p,
        const QStyleOptionViewItem& opt,
        const Graph::State& type,
        bool laneHeadPresent,
        int x1,
        int x2,
        const QColor& col,
        const QColor& activeCol,
        const QColor& mergeColor,
        bool isWip = false,
        bool hasChilds = true) const;
    void paintTagBranch(QPainter* painter, QStyleOptionViewItem opt, int& startPoint, const Commit& commit) const;
    QColor getMergeColor(
        const Graph::State& currentLane,
        const Commit& commit,
        int currentLaneIndex,
        const QColor& defaultColor,
        bool& isSet) const;
    QImage renderSvgToPixmap(const QString& fileName, QSize forcedSize = QSize()) const;
};
