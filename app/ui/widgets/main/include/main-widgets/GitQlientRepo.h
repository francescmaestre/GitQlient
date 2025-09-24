#pragma once

#include <QFrame>
#include <QMap>
#include <QPointer>
#include <QThread>

class GitBase;
class GitQlientSettings;
class GitCache;
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
    class Cache;
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
        const QSharedPointer<GitQlientSettings>& settings,
        QWidget* parent = nullptr);
    ~GitQlientRepo() override;

    QString currentDir() const { return mCurrentDir; }
    QString currentBranch() const;
    QSharedPointer<GitCache> getGitQlientCache() const { return mGitQlientCache; }

protected:
    void closeEvent(QCloseEvent* ce) override;

private:
    QString mCurrentDir;
    QSharedPointer<GitCache> mGitQlientCache;
    QSharedPointer<Graph::Cache> mGraphCache;
    QSharedPointer<GitBase> mGitBase;
    QSharedPointer<GitQlientSettings> mSettings;
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
