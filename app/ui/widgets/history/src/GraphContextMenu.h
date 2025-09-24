#pragma once

#include <QMenu>

class GitCache;
namespace Graph
{
    class Cache;
}
class GitBase;

class GraphContextMenu : public QMenu
{
    Q_OBJECT

signals:
    void fullReload();
    void referencesReload();
    void logReload();
    void signalOpenDiff(const QString& sha);
    void signalAmendCommit(const QString& sha);
    void signalRebaseConflict();
    void signalMergeRequired(const QString& origin, const QString& destination);
    void mergeSqushRequested(const QString& origin, const QString& destination);
    void signalCherryPickConflict(const QStringList& pendingShas = QStringList());
    void signalPullConflict();
    void signalRefreshPRsCache();

public:
    explicit GraphContextMenu(
        const QSharedPointer<GitCache>& cache,
        const QSharedPointer<Graph::Cache>& graphCache,
        const QSharedPointer<GitBase>& git,
        const QStringList& shas,
        QWidget* parent = nullptr);

private:
    QSharedPointer<GitCache> mCache;
    QSharedPointer<Graph::Cache> mGraphCache;
    QSharedPointer<GitBase> mGit;
    QStringList mShas;

    void createIndividualShaMenu();
    void createMultipleShasMenu();
    void createBranch();
    void createTag();
    void exportAsPatch();
    void checkoutBranch();
    void createCheckoutBranch();
    void checkoutCommit();
    void cherryPickCommit();
    void applyPatch();
    void applyCommit();
    void push();
    void pull();
    void fetch();
    void revertCommit();
    void resetSoft();
    void resetMixed();
    void resetHard();
    void rebase();
    void merge();
    void mergeSquash();
    void addBranchActions(const QString& sha);
    void showSquashDialog();
    void amendNoEdit();
};
