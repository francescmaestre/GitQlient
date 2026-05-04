#pragma once

/****************************************************************************************
 ** GitQlient is an application to manage and operate one or several Git repositories. With
 ** GitQlient you will be able to add commits, branches and manage all the options Git provides.
 ** Copyright (C) 2021  Francesc Maestre
 **
 ** LinkedIn: https://www.linkedin.com/in/francescmaestre/
 **
 ** This program is free software; you can redistribute it and/or
 ** modify it under the terms of the GNU Lesser General Public
 ** License as published by the Free Software Foundation; either
 ** version 2 of the License, or (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 ** Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public
 ** License along with this library; if not, write to the Free Software
 ** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 ***************************************************************************************/

#include <QDateTime>
#include <QElapsedTimer>
#include <QStyledItemDelegate>

class GraphView;
class SacredTimeline;
class GitBase;
class Commit;
namespace Graph
{
    class TemporalLoom;
    class Strand;
} // namespace Graph

const int ROW_HEIGHT = 25;
const int LANE_WIDTH = 3 * ROW_HEIGHT / 4;
const int TEXT_HEIGHT_OFFSET = 2;

class GraphViewDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    GraphViewDelegate(
        const QSharedPointer<SacredTimeline>& cache,
        const QSharedPointer<Graph::TemporalLoom>& graphCache,
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
    QSharedPointer<SacredTimeline> mCache;
    QSharedPointer<Graph::TemporalLoom> mGraphCache;
    QSharedPointer<GitBase> mGit;
    GraphView* mView = nullptr;
    mutable QColor mCurrentTextColor;
    mutable QImage mCurrentTagIcon;
    mutable QImage mCurrentLocalBranchIcon;
    mutable QImage mCurrentRemoteBranchIcon;

    // Paint timing — accumulated per 5-second window, reported via QLogger.
    mutable QElapsedTimer mPaintWindowTimer;
    mutable qint64 mPaintWindowRows = 0;
    mutable qint64 mPaintWindowNs = 0;
    mutable qint64 mPaintWindowMaxNs = 0;
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
        const Graph::Strand& type,
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
        const Graph::Strand& currentLane,
        const Commit& commit,
        int currentLaneIndex,
        const QColor& defaultColor,
        bool& isSet) const;
    QImage renderSvgToPixmap(const QString& fileName, QSize forcedSize = QSize()) const;
};
