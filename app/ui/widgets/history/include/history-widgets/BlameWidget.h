#pragma once

#include <QFrame>
#include <QMap>

class GitCache;
namespace Graph
{
    class Cache;
}
class GitBase;
class QFileSystemModel;
class FileBlameWidget;
class QTreeView;
class GraphModel;
class GraphView;
class QTabWidget;
class QModelIndex;
class GraphViewDelegate;

class BlameWidget : public QFrame
{
    Q_OBJECT

signals:
    void showFileDiff(const QString& sha, const QString& parentSha, const QString& file);

public:
    explicit BlameWidget(
        const QSharedPointer<GitCache>& cache,
        const QSharedPointer<Graph::Cache>& graphCache,
        const QSharedPointer<GitBase>& git,
        QWidget* parent = nullptr);
    ~BlameWidget();

    void init(const QString& workingDirectory);
    void showFileHistory(const QString& filePath);
    void onNewRevisions(int totalCommits);

private:
    QSharedPointer<GitCache> mCache;
    QSharedPointer<Graph::Cache> mGraphCache;
    QSharedPointer<GitBase> mGit;
    QFileSystemModel* mFileSystemModel = nullptr;
    GraphModel* mRepoModel = nullptr;
    GraphView* mRepoView = nullptr;
    QTreeView* mFileSystemView = nullptr;
    QTabWidget* mTabWidget = nullptr;
    QString mWorkingDirectory;
    QMap<QString, FileBlameWidget*> mTabsMap;
    GraphViewDelegate* mItemDelegate = nullptr;
    int mSelectedRow = -1;
    int mLastTabIndex = 0;

    void showFileHistoryByIndex(const QModelIndex& index);
    void showRepoViewMenu(const QPoint& pos);
    void reloadBlame(const QModelIndex& index);
    void reloadHistory(int tabIndex);
    void showFileSystemMenu(const QPoint& pos);
    void openExternalEditor();
};
