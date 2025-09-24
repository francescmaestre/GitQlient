#pragma once

#include <QSortFilterProxyModel>

class ShaFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit ShaFilterProxyModel(QObject* parent = nullptr);

    void setAcceptedSha(const QStringList& acceptedShaList) { mAcceptedShas = acceptedShaList; }
    void beginResetModel() { QSortFilterProxyModel::beginResetModel(); }
    void endResetModel() { QSortFilterProxyModel::endResetModel(); }

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

private:
    QStringList mAcceptedShas;
};
