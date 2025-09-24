#pragma once

#include <QAbstractItemModel>
#include <QSharedPointer>

class GitCache;
class GitBase;
class Commit;
enum class GraphColumns;

class GraphModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit GraphModel(
        const QSharedPointer<GitCache>& cache, const QSharedPointer<GitBase>& git, QObject* parent = nullptr);

    void clear();
    QString sha(int row) const;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int s, Qt::Orientation o, int role = Qt::DisplayRole) const override;
    QModelIndex index(int r, int c, const QModelIndex& par = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& par = QModelIndex()) const override;
    bool hasChildren(const QModelIndex& par = QModelIndex()) const override;
    int columnCount(const QModelIndex&) const override { return mColumns.count(); }
    void onNewRevisions(int totalCommits);
    int columnCount() const { return mColumns.count(); }

private:
    QSharedPointer<GitCache> mCache;
    QSharedPointer<GitBase> mGit;
    QMap<GraphColumns, QString> mColumns;

    QVariant getToolTipData(const Commit& r) const;
    QVariant getDisplayData(const Commit& rev, int column) const;
};
