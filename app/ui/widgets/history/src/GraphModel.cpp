#include "GraphModel.h"

#include "GraphColumns.h"

#include <GitBase.h>
#include <cache/Commit.h>
#include <cache/GitCache.h>

#include <QDateTime>
#include <QLocale>

GraphModel::GraphModel(const QSharedPointer<GitCache>& cache, const QSharedPointer<GitBase>& git, QObject* p)
    : QAbstractItemModel(p)
    , mCache(cache)
    , mGit(git)
{
    mColumns.insert(GraphColumns::TreeViewIcon, "");
    mColumns.insert(GraphColumns::Graph, "");
    mColumns.insert(GraphColumns::Refs, "Branch/Tag");
    mColumns.insert(GraphColumns::Sha, "Sha");
    mColumns.insert(GraphColumns::Log, "History");
    mColumns.insert(GraphColumns::Author, "Author");
    mColumns.insert(GraphColumns::Date, "Date");
}

int GraphModel::rowCount(const QModelIndex& parent) const { return !parent.isValid() ? mCache->commitCount() : 0; }

bool GraphModel::hasChildren(const QModelIndex& parent) const { return !parent.isValid(); }

QString GraphModel::sha(int row) const { return index(row, static_cast<int>(GraphColumns::Sha)).data().toString(); }

void GraphModel::clear()
{
    beginResetModel();
    endResetModel();
    emit headerDataChanged(Qt::Horizontal, 0, 5);
}

void GraphModel::onNewRevisions(int totalCommits)
{
    beginResetModel();
    endResetModel();

    beginInsertRows(QModelIndex(), 0, totalCommits - 2);
    endInsertRows();
}

QVariant GraphModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return mColumns.value(static_cast<GraphColumns>(section));

    return QVariant();
}

QModelIndex GraphModel::index(int row, int column, const QModelIndex&) const
{
    return row >= 0 && row < mCache->commitCount() ? createIndex(row, column, nullptr) : QModelIndex();
}

QModelIndex GraphModel::parent(const QModelIndex&) const { return QModelIndex(); }

QVariant GraphModel::getToolTipData(const Commit& r) const
{
    QString auxMessage;
    const auto sha = r.sha;

    if (mGit->getCurrentBranch().isEmpty())
        auxMessage.append(tr("<p>Status: <b>detached</b></p>"));

    const auto localBranches = mCache->getReferences(sha, References::Type::LocalBranch);

    if (!localBranches.isEmpty())
        auxMessage.append(tr("<p><b>Local: </b>%1</p>").arg(localBranches.join(",")));

    const auto remoteBranches = mCache->getReferences(sha, References::Type::RemoteBranche);

    if (!remoteBranches.isEmpty())
        auxMessage.append(tr("<p><b>Remote: </b>%1</p>").arg(remoteBranches.join(",")));

    const auto tags = mCache->getReferences(sha, References::Type::LocalTag);

    if (!tags.isEmpty())
        auxMessage.append(tr("<p><b>Tags: </b>%1</p>").arg(tags.join(",")));

    QDateTime d;
    d.setSecsSinceEpoch(r.dateSinceEpoch.count());

    QLocale locale;

    return sha == ZERO_SHA
        ? QString()
        : QString("<p>%1 - %2</p><p>%3</p>%4%5")
              .arg(
                  r.author.split("<").first(),
                  d.toString(locale.dateTimeFormat(QLocale::ShortFormat)),
                  sha,
                  !auxMessage.isEmpty() ? QString("<p>%1</p>").arg(auxMessage) : "",
                  r.isSigned()
                      ? tr("<p> GPG key (%1): %2</p>")
                            .arg(QString::fromUtf8(r.verifiedSignature() ? "verified" : "not verified"), r.gpgKey)
                      : "");
}

QVariant GraphModel::getDisplayData(const Commit& rev, int column) const
{
    switch (static_cast<GraphColumns>(column))
    {
    case GraphColumns::Sha: {
        const auto sha = rev.sha;
        return sha;
    }
    case GraphColumns::Log:
        return rev.shortLog;
    case GraphColumns::Author: {
        const auto author = rev.author.split("<").first();
        return author;
    }
    case GraphColumns::Date: {
        return QDateTime::fromSecsSinceEpoch(rev.dateSinceEpoch.count()).toString("dd MMM yyyy hh:mm");
    }
    default:
        return QVariant();
    }
}

QVariant GraphModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || (role != Qt::DisplayRole && role != Qt::ToolTipRole))
        return QVariant();

    const auto r = mCache->commitInfo(index.row());

    if (role == Qt::ToolTipRole)
        return getToolTipData(r);

    if (role == Qt::DisplayRole)
        return getDisplayData(r, index.column());

    return QVariant();
}
