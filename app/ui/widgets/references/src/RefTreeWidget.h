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

#include <QWidget>
#include <memory>

class ClickableFrame;
class BranchTreeWidget;
class SacredTimeline;
class GitBase;
class BranchesViewDelegate;

class RefTreeWidget : public QWidget
{
    Q_OBJECT

signals:
    void fullReload();
    void logReload();
    void signalSelectCommit(const QString& sha);
    void signalMergeRequired(const QString& currentBranch, const QString& fromBranch);
    void mergeSqushRequested(const QString& origin, const QString& destination);
    void signalPullConflict();
    void clearSelection();

public:
    enum RefType
    {
        LocalBranches,
        RemoteBranches,
        Tags
    };

    explicit RefTreeWidget(
        const QString& title,
        const QString& settingsKey,
        RefType type,
        const QSharedPointer<SacredTimeline>& cache,
        const QSharedPointer<GitBase>& git,
        QWidget* parent = nullptr);
    ~RefTreeWidget();

    void setCount(int count);
    void adjustBranchesTree();
    void reloadCurrentBranchLink();
    void clear();
    void reloadVisibility();
    void addItems(bool isCurrentBranch, const QString& fullBranchName, const QString& sha);

private:
    ClickableFrame* mFrame = nullptr;
    BranchTreeWidget* mTreeWidget = nullptr;
    RefType mRefType;
    QString mSettingsKey;
    BranchesViewDelegate* mDelegate = nullptr;

    void saveExpansionState(bool expanded);
};
