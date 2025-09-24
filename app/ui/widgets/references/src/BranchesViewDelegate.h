#pragma once

#include <QStyledItemDelegate>

class QPainter;

class BranchesViewDelegate : public QStyledItemDelegate
{
public:
    explicit BranchesViewDelegate(bool isTag = false, QObject* parent = nullptr);

    void paint(QPainter* p, const QStyleOptionViewItem& o, const QModelIndex& i) const override;
    QSize sizeHint(const QStyleOptionViewItem&, const QModelIndex&) const override;
    bool
    helpEvent(QHelpEvent* event, QAbstractItemView* view, const QStyleOptionViewItem& option, const QModelIndex& index)
        override;

private:
    bool mIsTag = false;
};
