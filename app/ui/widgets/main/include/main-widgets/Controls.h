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
#include <QPointer>

class QToolButton;
class QPushButton;
class GitBase;
class SacredTimeline;
class SacredTimeline;
class GitQlientUpdater;
class QButtonGroup;
class QHBoxLayout;

enum class ControlsMainViews
{
    History,
    Diff,
    Blame,
    Merge,
};

class Controls : public QFrame
{
    Q_OBJECT

signals:
    void signalGoMerge();
    void signalPullConflict();
    void signalRefreshPRsCache();
    void requestFullReload();
    void requestReferencesReload();

public:
    explicit Controls(
        const QSharedPointer<SacredTimeline>& cache, const QSharedPointer<GitBase>& git, QWidget* parent = nullptr);
    ~Controls() = default;

    void setCurrentSha(const QString& sha) { mCurrentSha = sha; }
    void enableButtons(bool enabled);
    void fetchAll();
    void activateMergeWarning();
    void disableMergeWarning();
    ControlsMainViews getCurrentSelectedButton() const;

private:
    QString mCurrentSha;
    QSharedPointer<SacredTimeline> mCache;
    QSharedPointer<GitBase> mGit;
    QToolButton* mStashPop = nullptr;
    QToolButton* mStashPush = nullptr;
    QToolButton* mPullBtn = nullptr;
    QToolButton* mPullOptions = nullptr;
    QToolButton* mPushBtn = nullptr;
    QToolButton* mRefreshBtn = nullptr;
    QToolButton* mVersionCheck = nullptr;
    QPushButton* mMergeWarning = nullptr;
    GitQlientUpdater* mUpdater = nullptr;
    QFrame* mLastSeparator = nullptr;

    QToolButton*
    createToolButton(const QString& iconPath, const QString& tooltip, const QKeySequence& shortcut = QKeySequence());

    void stashPush();
    void stashPop();
    void pullCurrentBranch();
    void pushCurrentBranch();
    void pruneBranches();
    bool eventFilter(QObject* obj, QEvent* event);
};
