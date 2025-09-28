#include "RefListWidget.h"
#include "GitBase.h"

#include "RefListDelegate.h"
#include <custom-widgets/ClickableFrame.h>

#include <QListWidget>
#include <QSettings>
#include <QVBoxLayout>

RefListWidget::RefListWidget(
    const QString& title,
    const QString& settingsKey,
    const QSharedPointer<GitCache>& cache,
    const QSharedPointer<GitBase>& git,
    QWidget* parent)
    : QWidget(parent)
    , mSettingsKey(settingsKey)
{
    mFrame = new ClickableFrame(title);
    mFrame->setExpandable(true);
    mFrame->setObjectName("RefListWidget");

    mList = new QListWidget();
    mList->setMouseTracking(true);
    mList->setContextMenuPolicy(Qt::CustomContextMenu);
    mList->setItemDelegate(mDelegate = new RefListDelegate());

    mFrame->setContentWidget(mList);

    QSettings settings;
    const auto isExpanded = settings.value(settingsKey, true).toBool();
    mFrame->setExpanded(isExpanded);

    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(mFrame);

    connect(mFrame, &ClickableFrame::expanded, this, [this](bool expanded) {
        saveExpansionState(expanded);
    });

    connect(mList, &QListWidget::itemClicked, this, [this](QListWidgetItem* item) {
        emit itemSelected(item->data(Qt::UserRole).toString());
    });

    connect(mList, &QListWidget::itemDoubleClicked, this, [this](QListWidgetItem* item) {
        emit itemDoubleClicked(item->data(Qt::UserRole).toString());
    });

    connect(mList, &QListWidget::customContextMenuRequested, this, [this](const QPoint& pos) {
        if (auto item = mList->itemAt(pos))
            emit contextMenuRequested(mList->viewport()->mapToGlobal(pos), item->data(Qt::UserRole).toString());
    });
}

void RefListWidget::setCount(int count) { mFrame->setCount(count); }

void RefListWidget::addItem(const QString& text, const QString& data)
{
    auto item = new QListWidgetItem(text);
    item->setData(Qt::UserRole, data.isEmpty() ? text : data);
    mList->addItem(item);
}

void RefListWidget::clear()
{
    mList->clear();
    mFrame->setCount(0);
}

void RefListWidget::reloadVisibility()
{
    QSettings settings;
    const auto isExpanded = settings.value(mSettingsKey, true).toBool();
    mFrame->setExpanded(isExpanded);
}

void RefListWidget::saveExpansionState(bool expanded)
{
    QSettings settings;
    settings.setValue(mSettingsKey, expanded);
}
