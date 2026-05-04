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

class CommitInfoPanel;
class GitBase;
class QPinnableTabWidget;
class IDiffWidget;
class QVBoxLayout;
class FileListWidget;
class SacredTimeline;
class QListWidgetItem;

class DiffWidget : public QFrame
{
    Q_OBJECT

signals:
    void signalShowFileHistory(const QString& fileName);
    void signalDiffEmpty();

public:
    explicit DiffWidget(
        const QSharedPointer<GitBase> git, QSharedPointer<SacredTimeline> cache, QWidget* parent = nullptr);
    ~DiffWidget() override;
    void reload();
    void clear() const;
    bool loadFileDiff(const QString& sha, const QString& previousSha, const QString& file);
    bool loadCommitDiff(const QString& sha, const QString& parentSha);
    void onDiffFontSizeChanged();

private:
    QSharedPointer<GitBase> mGit;
    QSharedPointer<SacredTimeline> mCache;
    CommitInfoPanel* mInfoPanelBase = nullptr;
    CommitInfoPanel* mInfoPanelParent = nullptr;
    QPinnableTabWidget* mCenterStackedWidget = nullptr;
    QMap<QString, IDiffWidget*> mDiffWidgets;
    FileListWidget* fileListWidget = nullptr;
    QString mCurrentSha;
    QString mParentSha;
    void changeSelection(int index);
    void onTabClosed(int index);
    void onDoubleClick(QListWidgetItem* item);
};
