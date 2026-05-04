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

class SacredTimeline;
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
    class TemporalLoom;
}
class QLabel;
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
        const QSharedPointer<SacredTimeline>& cache,
        const QSharedPointer<Graph::TemporalLoom>& graphCache,
        const QSharedPointer<GitBase> git,
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
    QSharedPointer<SacredTimeline> mCache;
    QSharedPointer<Graph::TemporalLoom> mGraphCache;
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

    void onRevertedChanges();
};
