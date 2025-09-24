#pragma once

#include <QFrame>

class GitCache;
class GitBase;
class GraphModel;
class GraphView;
class QLineEdit;
class BranchesWidget;
class QStackedWidget;
class CommitChangesWidget;
class CommitInfoWidget;
class CheckBox;
class GraphViewDelegate;
class FileDiffWidget;
class FullDiffWidget;
class BranchesWidgetMinimal;
class QPushButton;
namespace Graph
{
    class Cache;
}
class QLabel;
class GitQlientSettings;
struct GitExecResult;

class HistoryWidget : public QFrame
{
    Q_OBJECT

signals:
    void fullReload();
    void referencesReload();
    void logReload();
    void signalOpenSubmodule(const QString& submodule);
    void changesCommitted();
    void signalShowFileHistory(const QString& fileName);
    void signalAllBranchesActive(bool showAll);
    void signalMergeConflicts();
    void signalRebaseConflict();
    void signalCherryPickConflict(const QStringList& pendingShas);
    void signalPullConflict();
    void signalUpdateWip();
    void panelsVisibilityChanged();

public:
    explicit HistoryWidget(
        const QSharedPointer<GitCache>& cache,
        const QSharedPointer<Graph::Cache>& graphCache,
        const QSharedPointer<GitBase> git,
        const QSharedPointer<GitQlientSettings>& settings,
        QWidget* parent = nullptr);
    ~HistoryWidget();

    void clear();
    void resetWip();
    void loadBranches(bool fullReload);
    void updateUiFromWatcher();
    void selectCommit(const QString& goToSha);
    void updateGraphView(int totalCommits);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private:
    enum class Pages
    {
        Graph,
        FileDiff,
        FullDiff
    };

    QSharedPointer<GitBase> mGit;
    QSharedPointer<GitCache> mCache;
    QSharedPointer<Graph::Cache> mGraphCache;
    QSharedPointer<GitQlientSettings> mSettings;
    QFrame* mCommitInfoFrame = nullptr;
    GraphModel* mRepositoryModel = nullptr;
    GraphView* mRepositoryView = nullptr;
    BranchesWidget* mBranchesWidget = nullptr;
    QLineEdit* mSearchInput = nullptr;
    QStackedWidget* mCommitStackedWidget = nullptr;
    QStackedWidget* mCenterStackedWidget = nullptr;
    CommitChangesWidget* mCommitChangesWidget = nullptr;
    CommitInfoWidget* mCommitInfoWidget = nullptr;
    CheckBox* mChShowAllBranches = nullptr;
    GraphViewDelegate* mItemDelegate = nullptr;
    QFrame* mGraphFrame = nullptr;
    FileDiffWidget* mWipFileDiff = nullptr;
    FullDiffWidget* mFullDiffWidget = nullptr;
    QPushButton* mReturnFromFull = nullptr;
    bool mReverseSearch = false;
    int mLastSelectedRow = -1;

    void search();
    void goToSha(const QString& sha);
    void commitSelected(const QModelIndex& index);
    void onShowAllUpdated(bool showAll);

    void onAmendCommit(const QString& sha);

    void mergeBranch(const QString& current, const QString& branchToMerge);

    void mergeSquashBranch(const QString& current, const QString& branchToMerge);

    void processMergeResponse(const GitExecResult& ret);

    void returnToView();

    void returnToViewIfObsolete(const QString& fileName);

    void cherryPickCommit();

    void showWipFileDiff(const QString& fileName, bool isCached);

    void showFileDiff(const QString& fileName, const QString& currentSha, const QString& parentSha);

    void onOpenFullDiff(const QString& sha);

    void rearrangeSplittrer(bool minimalActive);

    void onRevertedChanges();
};
