#pragma once

#include <QMenu>

class GitBase;
class GitCache;

struct BranchContextMenuConfig
{
    QString currentBranch;
    QString branchSelected;
    bool isLocal;
    QSharedPointer<GitCache> mCache;
    QSharedPointer<GitBase> mGit;
};

class BranchContextMenu : public QMenu
{
    Q_OBJECT

signals:
    void fullReload();
    void logReload();
    void signalCheckoutBranch();
    void signalMergeRequired(const QString& currentBranch, const QString& fromBranch);
    void signalPullConflict();
    void signalRefreshPRsCache();
    void mergeSqushRequested(const QString& origin, const QString& destination);

public:
    explicit BranchContextMenu(BranchContextMenuConfig config, QWidget* parent = nullptr);

private:
    BranchContextMenuConfig mConfig;

    void pull();
    void fetch();
    void resetToOrigin();
    void resetToSha();
    void push();
    void unsetUpstream();
    void pushForce();
    void createBranch();
    void createCheckoutBranch();
    void merge();
    void mergeSquash();
    void rename();
    void deleteBranch();
};
