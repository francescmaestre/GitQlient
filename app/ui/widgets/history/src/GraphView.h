#pragma once

#include <QTreeView>

#include <GitExecResult.h>
#include <cache/Commit.h>

class GitCache;
class GitBase;
namespace Graph
{
    class Cache;
}
class GraphModel;
class ShaFilterProxyModel;

class GraphView : public QTreeView
{
    Q_OBJECT

signals:
    void fullReload();
    void referencesReload();
    void logReload();
    void onClick(const QModelIndex& index);
    void signalOpenDiff(const QString& sha);
    void signalAmendCommit(const QString& sha);
    void signalRebaseConflict();
    void signalMergeRequired(const QString& origin, const QString& destination);
    void mergeSqushRequested(const QString& origin, const QString& destination);
    void signalCherryPickConflict(const QStringList& pendingShas);
    void signalPullConflict();

public:
    explicit GraphView(
        const QSharedPointer<GitCache>& cache,
        const QSharedPointer<Graph::Cache>& graphCache,
        const QSharedPointer<GitBase>& git,
        QWidget* parent = nullptr);
    ~GraphView() override;

    void setModel(QAbstractItemModel* model) override;
    QStringList getSelectedShaList() const;
    void filterBySha(const QStringList& shaList);
    void activateFilter(bool activate) { mIsFiltering = activate; }
    bool hasActiveFilter() const { return mIsFiltering; }
    void clear();
    void focusOnCommit(const QString& goToSha);
    QString getCurrentSha() const { return mCurrentSha; }
    QModelIndexList selectedIndexes() const override;

private:
    QSharedPointer<GitCache> mCache;
    QSharedPointer<Graph::Cache> mGraphCache;
    QSharedPointer<GitBase> mGit;
    GraphModel* mCommitHistoryModel = nullptr;
    ShaFilterProxyModel* mProxyModel = nullptr;
    bool mIsFiltering = false;
    QString mCurrentSha = ZERO_SHA;
    int mLastSelectedRow = -1;
    void showContextMenu(const QPoint& p);
    void setupGeometry();
    void currentChanged(const QModelIndex& index, const QModelIndex& parent) override;
    void refreshView();
    void onHeaderContextMenu(const QPoint& pos);
    void handleItemClick(const QModelIndex& index);
};
