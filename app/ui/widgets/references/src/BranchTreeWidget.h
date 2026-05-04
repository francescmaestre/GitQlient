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

#include <QTreeWidget>

class GitBase;
class SacredTimeline;

class BranchTreeWidget : public QTreeWidget
{
    Q_OBJECT

signals:
    void fullReload();
    void logReload();
    void signalSelectCommit(const QString& sha);
    void signalMergeRequired(const QString& currentBranch, const QString& fromBranch);
    void signalPullConflict();
    void signalRefreshPRsCache();
    void mergeSqushRequested(const QString& origin, const QString& destination);

public:
    explicit BranchTreeWidget(QWidget* parent = nullptr);
    explicit BranchTreeWidget(
        const QSharedPointer<SacredTimeline>& cache, const QSharedPointer<GitBase>& git, QWidget* parent = nullptr);

    void init(const QSharedPointer<SacredTimeline>& cache, const QSharedPointer<GitBase>& git);
    void setLocalRepo(bool isLocal) { mIsLocal = isLocal; }
    bool isLocal() const { return mIsLocal; }
    void reloadCurrentBranchLink();
    void clearSelection();
    int focusOnBranch(const QString& itemText, int startSearchPos = -1);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    bool mIsLocal = false;
    QSharedPointer<SacredTimeline> mCache;
    QSharedPointer<GitBase> mGit;
    QTreeWidgetItem* mFolderToRemove = nullptr;

    QVector<QTreeWidgetItem*> findChildItem(const QString& text) const;
    void discoverBranchesInFolder(QTreeWidgetItem* folder, QStringList& branches);
    void showBranchesContextMenu(const QPoint& pos);
    void showDeleteFolderMenu(QTreeWidgetItem* item, const QPoint& pos);
    void checkoutBranch(QTreeWidgetItem* item);
    void selectCommit(QTreeWidgetItem* item);
    void onSelectionChanged();
    void deleteFolder();
    void onDeleteBranch();
};
