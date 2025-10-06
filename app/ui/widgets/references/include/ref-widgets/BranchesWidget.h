#pragma once

#include <QFrame>
#include <QMap>

class GitBase;
class GitCache;
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
        const QSharedPointer<GitCache>& cache, const QSharedPointer<GitBase>& git, QWidget* parent = nullptr);
    ~BranchesWidget() = default;

    void showBranches();
    void refreshCurrentBranchLink();
    void clear();
    void returnToSavedView();
    void forceMinimalView();
    void onPanelsVisibilityChaned();

private:
    QSharedPointer<GitCache> mCache;
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
