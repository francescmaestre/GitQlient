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

class GitBase;
class SacredTimeline;
class GitTags;
class RefTreeWidget;
class RefListWidget;
class BranchesWidgetMinimal;

class QPushButton;

class BranchesWidget : public QFrame
{
    Q_OBJECT

signals:
    void fullReload();
    void logReload();
    void panelsVisibilityChanged();
    void signalSelectCommit(const QString& sha);
    void signalOpenSubmodule(const QString& submoduleName);
    void signalMergeRequired(const QString& currentBranch, const QString& fromBranch);
    void signalPullConflict();
    void mergeSqushRequested(const QString& origin, const QString& destination);

public:
    explicit BranchesWidget(
        const QSharedPointer<SacredTimeline>& cache, const QSharedPointer<GitBase>& git, QWidget* parent = nullptr);
    ~BranchesWidget() = default;

    void showBranches();
    void refreshCurrentBranchLink();
    void clear();
    void returnToSavedView();
    void forceMinimalView();
    void onPanelsVisibilityChaned();

private:
    QSharedPointer<SacredTimeline> mCache;
    QSharedPointer<GitBase> mGit;
    QSharedPointer<GitTags> mGitTags;
    RefTreeWidget* mLocalBranches = nullptr;
    RefTreeWidget* mRemoteBranches = nullptr;
    RefTreeWidget* mTags = nullptr;
    RefListWidget* mStashes = nullptr;
    RefListWidget* mSubmodules = nullptr;
    RefListWidget* mSubtrees = nullptr;
    QPushButton* mMinimize = nullptr;
    QFrame* mFullBranchFrame = nullptr;
    BranchesWidgetMinimal* mMinimal = nullptr;

    void setupUI();
    void setupConnections();
    void loadSettings();
    void processRefs();
    void processBranches();
    void processTags();
    void processListItems();
    void fullView();
    void minimalView();
    void handleTagsContextMenu(const QPoint& pos, const QString& data);
    void handleStashesContextMenu(const QPoint& pos, const QString& data);
    void handleSubmodulesContextMenu(const QPoint& pos, const QString& data);
    void handleSubtreesContextMenu(const QPoint& pos, const QString& data);
    QPair<QString, QString> getSubtreeData(const QString& prefix);
    void resizeWidget();

    template <typename Processor>
    void processGenericList(RefListWidget* widget, Processor processor);
};
