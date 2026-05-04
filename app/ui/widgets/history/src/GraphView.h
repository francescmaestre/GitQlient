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

#include <QTreeView>

#include <GitExecResult.h>
#include <cache/Commit.h>

class SacredTimeline;
class GitBase;
namespace Graph
{
    class TemporalLoom;
}
class GraphModel;
class ShaFilterProxyModel;

class GraphView : public QTreeView
{
    Q_OBJECT

signals:
    void fullReload();
    void referencesReload();
    void logReload();
    void onClick(const QModelIndex& index);
    void signalOpenDiff(const QString& sha);
    void signalAmendCommit(const QString& sha);
    void signalRebaseConflict();
    void signalMergeRequired(const QString& origin, const QString& destination);
    void mergeSqushRequested(const QString& origin, const QString& destination);
    void signalCherryPickConflict(const QStringList& pendingShas);
    void signalPullConflict();

public:
    explicit GraphView(
        const QSharedPointer<SacredTimeline>& cache,
        const QSharedPointer<Graph::TemporalLoom>& graphCache,
        const QSharedPointer<GitBase>& git,
        QWidget* parent = nullptr);
    ~GraphView() override;

    void setModel(QAbstractItemModel* model) override;
    QStringList getSelectedShaList() const;
    void filterBySha(const QStringList& shaList);
    void activateFilter(bool activate) { mIsFiltering = activate; }
    bool hasActiveFilter() const { return mIsFiltering; }
    void clear();
    void focusOnCommit(const QString& goToSha);
    QString getCurrentSha() const { return mCurrentSha; }
    QModelIndexList selectedIndexes() const override;

private:
    QSharedPointer<SacredTimeline> mCache;
    QSharedPointer<Graph::TemporalLoom> mGraphCache;
    QSharedPointer<GitBase> mGit;
    GraphModel* mCommitHistoryModel = nullptr;
    ShaFilterProxyModel* mProxyModel = nullptr;
    bool mIsFiltering = false;
    QString mCurrentSha = ZERO_SHA;
    int mLastSelectedRow = -1;
    void showContextMenu(const QPoint& p);
    void setupGeometry();
    void currentChanged(const QModelIndex& index, const QModelIndex& parent) override;
    void refreshView();
    void onHeaderContextMenu(const QPoint& pos);
    void handleItemClick(const QModelIndex& index);
};
