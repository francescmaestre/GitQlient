#include "ShaFilterProxyModel.h"

#include "GraphColumns.h"

ShaFilterProxyModel::ShaFilterProxyModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
}

bool ShaFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    const auto shaIndex = sourceModel()->index(sourceRow, static_cast<int>(GraphColumns::Sha), sourceParent);
    const auto sha = sourceModel()->data(shaIndex).toString();
    return mAcceptedShas.contains(sha);
}
