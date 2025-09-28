#include "GraphView.h"

#include "GraphColumns.h"
#include "GraphContextMenu.h"
#include "GraphModel.h"
#include "ShaFilterProxyModel.h"

#include <GitBase.h>
#include <GitConfig.h>
#include <cache/Commit.h>
#include <cache/GitCache.h>

#include <QDateTime>
#include <QHeaderView>
#include <QSettings>

#include <QLogger>
using namespace QLogger;

GraphView::GraphView(
    const QSharedPointer<GitCache>& cache,
    const QSharedPointer<Graph::Cache>& graphCache,
    const QSharedPointer<GitBase>& git,
    QWidget* parent)
    : QTreeView(parent)
    , mCache(cache)
    , mGraphCache(graphCache)
    , mGit(git)
{
    setEnabled(false);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setItemsExpandable(false);
    setMouseTracking(true);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setAttribute(Qt::WA_DeleteOnClose);

    header()->setSortIndicatorShown(false);
    header()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(header(), &QHeaderView::customContextMenuRequested, this, &GraphView::onHeaderContextMenu);

    connect(mCache.get(), &GitCache::signalCacheUpdated, this, &GraphView::refreshView);
    connect(this, &GraphView::clicked, this, &GraphView::handleItemClick);
    connect(this, &GraphView::doubleClicked, this, [this](const QModelIndex& index) {
        if (mCommitHistoryModel)
        {
            const auto sha = mCommitHistoryModel->sha(index.row());
            emit signalOpenDiff(sha);
        }
    });
}

void GraphView::handleItemClick(const QModelIndex& index)
{
    if (index.column() == static_cast<int>(GraphColumns::Sha))
        return;

    if (mLastSelectedRow == index.row())
    {
        clearSelection();
        mLastSelectedRow = -1;

        emit onClick(index);
    }
    else
    {
        mLastSelectedRow = index.row();
        emit onClick(index);
    }
}

void GraphView::setModel(QAbstractItemModel* model)
{
    connect(this, &GraphView::customContextMenuRequested, this, &GraphView::showContextMenu, Qt::UniqueConnection);

    mCommitHistoryModel = dynamic_cast<GraphModel*>(model);
    QTreeView::setModel(model);
    setupGeometry();
    connect(
        this->selectionModel(),
        &QItemSelectionModel::selectionChanged,
        this,
        [this](const QItemSelection& selected, const QItemSelection&) {
            const auto indexes = selected.indexes();
            if (!indexes.isEmpty())
                scrollTo(indexes.first());
        });
}

void GraphView::filterBySha(const QStringList& shaList)
{
    mIsFiltering = true;

    if (mProxyModel)
    {
        mProxyModel->beginResetModel();
        mProxyModel->setAcceptedSha(shaList);
        mProxyModel->endResetModel();
    }
    else
    {
        mProxyModel = new ShaFilterProxyModel(this);
        mProxyModel->setSourceModel(mCommitHistoryModel);
        mProxyModel->setAcceptedSha(shaList);
        setModel(mProxyModel);
    }

    setupGeometry();
}

GraphView::~GraphView()
{
    QSettings settings;
    settings.setValue(QString("%1").arg(objectName()), header()->saveState());
}

void GraphView::setupGeometry()
{
    QSettings settings;
    const auto previousState = settings.value(QString("%1").arg(objectName()), QByteArray()).toByteArray();

    if (previousState.isEmpty())
    {
        const auto hv = header();
        hv->setMinimumSectionSize(75);
        hv->resizeSection(static_cast<int>(GraphColumns::Graph), 120);
        hv->resizeSection(static_cast<int>(GraphColumns::Author), 160);
        hv->resizeSection(static_cast<int>(GraphColumns::Date), 125);
        hv->resizeSection(static_cast<int>(GraphColumns::Sha), 75);
        hv->resizeSection(static_cast<int>(GraphColumns::Refs), 300);
        hv->setSectionResizeMode(static_cast<int>(GraphColumns::Author), QHeaderView::Fixed);
        hv->setSectionResizeMode(static_cast<int>(GraphColumns::Date), QHeaderView::Fixed);
        hv->setSectionResizeMode(static_cast<int>(GraphColumns::Sha), QHeaderView::Fixed);
        hv->setSectionResizeMode(static_cast<int>(GraphColumns::Refs), QHeaderView::Fixed);
        hv->setSectionResizeMode(static_cast<int>(GraphColumns::Log), QHeaderView::Stretch);
        hv->setStretchLastSection(false);

        hideColumn(static_cast<int>(GraphColumns::TreeViewIcon));
    }
    else
    {
        header()->restoreState(previousState);
        header()->setSectionResizeMode(static_cast<int>(GraphColumns::Log), QHeaderView::Stretch);
    }
}

void GraphView::currentChanged(const QModelIndex& index, const QModelIndex&)
{
    mCurrentSha = model()->index(index.row(), static_cast<int>(GraphColumns::Sha)).data().toString();
}

void GraphView::refreshView()
{
    QModelIndex topLeft;
    QModelIndex bottomRight;

    if (!mProxyModel)
    {
        topLeft = mCommitHistoryModel->index(0, 0);
        bottomRight = mCommitHistoryModel->index(
            mCommitHistoryModel->rowCount() - 1, mCommitHistoryModel->columnCount() - 1);
        mCommitHistoryModel->onNewRevisions(mCache->commitCount());

        const auto auxTL = visualRect(topLeft);
        const auto auxBR = visualRect(bottomRight);
        viewport()->update(auxTL.x(), auxTL.y(), auxBR.x() + auxBR.width(), auxBR.y() + auxBR.height());
    }
}

void GraphView::onHeaderContextMenu(const QPoint& pos)
{
    const auto menu = new QMenu(this);
    const auto total = header()->count();

    for (auto column = 2; column < total; ++column)
    {
        const auto columnName = model()->headerData(column, Qt::Horizontal, Qt::DisplayRole).toString();
        const auto action = menu->addAction(columnName);
        const auto isHidden = isColumnHidden(column);
        action->setCheckable(true);
        action->setChecked(!isHidden);
        connect(action, &QAction::triggered, this, [column, this, action]() {
            action->setChecked(!action->isChecked());
            setColumnHidden(column, !isColumnHidden(column));
        });
    }

    menu->exec(header()->mapToGlobal(pos));
}

void GraphView::clear() { mCommitHistoryModel->clear(); }

void GraphView::focusOnCommit(const QString& goToSha)
{
    mCurrentSha = goToSha;

    QLog_Info("UI", QString("Setting the focus on the commit {%1}").arg(mCurrentSha));

    auto row = mCache->commitInfo(mCurrentSha).pos;

    if (mIsFiltering)
    {
        const auto sourceIndex = mProxyModel->sourceModel()->index(row, 1);
        row = mProxyModel->mapFromSource(sourceIndex).row();
    }

    clearSelection();

    const auto index = model()->index(row, 1);

    setCurrentIndex(index);
    scrollTo(index);
}

QModelIndexList GraphView::selectedIndexes() const { return QTreeView::selectedIndexes(); }

void GraphView::showContextMenu(const QPoint& pos)
{
    if (!mIsFiltering)
    {
        const auto shas = getSelectedShaList();

        if (!shas.isEmpty())
        {
            const auto menu = new GraphContextMenu(mCache, mGraphCache, mGit, shas, this);
            connect(menu, &GraphContextMenu::fullReload, this, &GraphView::fullReload);
            connect(menu, &GraphContextMenu::referencesReload, this, &GraphView::referencesReload);
            connect(menu, &GraphContextMenu::logReload, this, &GraphView::logReload);
            connect(menu, &GraphContextMenu::signalOpenDiff, this, &GraphView::signalOpenDiff);
            connect(menu, &GraphContextMenu::signalAmendCommit, this, &GraphView::signalAmendCommit);
            connect(menu, &GraphContextMenu::signalMergeRequired, this, &GraphView::signalMergeRequired);
            connect(menu, &GraphContextMenu::signalRebaseConflict, this, &GraphView::signalRebaseConflict);
            connect(menu, &GraphContextMenu::mergeSqushRequested, this, &GraphView::mergeSqushRequested);
            connect(menu, &GraphContextMenu::signalCherryPickConflict, this, &GraphView::signalCherryPickConflict);
            connect(menu, &GraphContextMenu::signalPullConflict, this, &GraphView::signalPullConflict);
            menu->exec(viewport()->mapToGlobal(pos));
        }
        else
            QLog_Warning("UI", "SHAs selected belong to different branches. They need to share at least one branch.");
    }
}

QStringList GraphView::getSelectedShaList() const
{
    const auto indexes = selectedIndexes();

    if (indexes.count() > 0)
    {
        QMap<QDateTime, QString> shas;

        for (auto index : indexes)
        {
            const auto sha = mCommitHistoryModel->sha(index.row());
            const auto dtStr
                = mCommitHistoryModel->index(index.row(), static_cast<int>(GraphColumns::Date)).data().toString();

            shas.insert(QDateTime::fromString(dtStr, "dd MMM yyyy hh:mm"), sha);
        }

        return shas.values();
    }

    return QStringList();
}
