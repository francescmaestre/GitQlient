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
#include <QPointer>
#include <QThread>

class GitBase;
class SacredTimeline;
class GitRepoLoader;
class QCloseEvent;
class QStackedLayout;
class Controls;
class HistoryWidget;
class DiffWidget;
class BlameWidget;
class MergeWidget;
class QTimer;
class WaitingDlg;
class GitTags;

namespace Graph
{
    class TemporalLoom;
}
class ConfigWidget;

namespace GitServer
{
    class IRestApi;
}

enum class ControlsMainViews;

namespace Ui
{
    class MainWindow;
}

class GitQlientRepo : public QFrame
{
    Q_OBJECT

signals:
    void signalOpenSubmodule(const QString& submoduleName);
    void loadRepo();
    void fullReload();
    void referencesReload();
    void logReload();
    void repoOpened(const QString& repoPath);
    void currentBranchChanged();
    void moveLogsAndClose();

public:
    explicit GitQlientRepo(
        const QSharedPointer<GitBase>& git,
        QWidget* parent = nullptr);
    ~GitQlientRepo() override;

    QString currentDir() const { return mCurrentDir; }
    QString currentBranch() const;
    QSharedPointer<SacredTimeline> getGitQlientCache() const { return mGitQlientCache; }

protected:
    void closeEvent(QCloseEvent* ce) override;

private:
    QString mCurrentDir;
    QSharedPointer<SacredTimeline> mGitQlientCache;
    QSharedPointer<Graph::TemporalLoom> mGraphCache;
    QSharedPointer<GitBase> mGitBase;
    QSharedPointer<GitRepoLoader> mGitLoader;
    HistoryWidget* mHistoryWidget = nullptr;
    QStackedLayout* mStackedLayout = nullptr;
    Controls* mControls = nullptr;
    DiffWidget* mDiffWidget = nullptr;
    BlameWidget* mBlameWidget = nullptr;
    MergeWidget* mMergeWidget = nullptr;
    QTimer* mAutoFilesUpdate = nullptr;
    QTimer* mAutoPrUpdater = nullptr;
    QPointer<WaitingDlg> mWaitDlg;
    int mPreviousView;
    QMap<ControlsMainViews, int> mIndexMap;
    QSharedPointer<GitServer::IRestApi> mApi;
    bool mIsInit = false;
    QThread* m_loaderThread;

    void updateUiFromWatcher();
    void onChangesCommitted();
    void clearWindow();
    void setWidgetsEnabled(bool enabled);
    void showFileHistory(const QString& fileName);
    void createProgressDialog();
    void onRepoLoadFinished();
    void loadFileDiff(const QString& currentSha, const QString& previousSha, const QString& file);
    void showHistoryView();
    void showBlameView();
    void showDiffView();
    void showWarningMerge();
    void showRebaseConflict();
    void showCherryPickConflict(const QStringList& shas = QStringList());
    void showPullConflict();
    void showMergeView();
    void showPreviousView();
    void updateWip();
    void reconfigureAutoRefresh(int newInterval);
};
