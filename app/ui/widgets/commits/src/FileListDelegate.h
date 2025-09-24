#pragma once

#include <QItemDelegate>

class FileListDelegate : public QItemDelegate
{
public:
    explicit FileListDelegate(QObject* parent = nullptr);

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};
