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

#include <QFrame>
#include <QMap>

class SacredTimeline;
namespace Graph
{
    class TemporalLoom;
}
class GitBase;
class QFileSystemModel;
class FileBlameWidget;
class QTreeView;
class GraphModel;
class GraphView;
class QTabWidget;
class QModelIndex;
class GraphViewDelegate;

class BlameWidget : public QFrame
{
    Q_OBJECT

signals:
    void showFileDiff(const QString& sha, const QString& parentSha, const QString& file);

public:
    explicit BlameWidget(
        const QSharedPointer<SacredTimeline>& cache,
        const QSharedPointer<Graph::TemporalLoom>& graphCache,
        const QSharedPointer<GitBase>& git,
        QWidget* parent = nullptr);
    ~BlameWidget();

    void init(const QString& workingDirectory);
    void showFileHistory(const QString& filePath);
    void onNewRevisions(int totalCommits);

private:
    QSharedPointer<SacredTimeline> mCache;
    QSharedPointer<Graph::TemporalLoom> mGraphCache;
    QSharedPointer<GitBase> mGit;
    QFileSystemModel* mFileSystemModel = nullptr;
    GraphModel* mRepoModel = nullptr;
    GraphView* mRepoView = nullptr;
    QTreeView* mFileSystemView = nullptr;
    QTabWidget* mTabWidget = nullptr;
    QString mWorkingDirectory;
    QMap<QString, FileBlameWidget*> mTabsMap;
    GraphViewDelegate* mItemDelegate = nullptr;
    int mSelectedRow = -1;
    int mLastTabIndex = 0;

    void showFileHistoryByIndex(const QModelIndex& index);
    void showRepoViewMenu(const QPoint& pos);
    void reloadBlame(const QModelIndex& index);
    void reloadHistory(int tabIndex);
    void showFileSystemMenu(const QPoint& pos);
    void openExternalEditor();
};
