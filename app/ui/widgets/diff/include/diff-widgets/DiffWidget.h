#pragma once

#include <QFrame>
#include <QMap>

class CommitInfoPanel;
class GitBase;
class QPinnableTabWidget;
class IDiffWidget;
class QVBoxLayout;
class FileListWidget;
class GitCache;
class QListWidgetItem;

class DiffWidget : public QFrame
{
    Q_OBJECT

signals:
    void signalShowFileHistory(const QString& fileName);
    void signalDiffEmpty();

public:
    explicit DiffWidget(const QSharedPointer<GitBase> git, QSharedPointer<GitCache> cache, QWidget* parent = nullptr);
    ~DiffWidget() override;
    void reload();
    void clear() const;
    bool loadFileDiff(const QString& sha, const QString& previousSha, const QString& file);
    bool loadCommitDiff(const QString& sha, const QString& parentSha);
    void onDiffFontSizeChanged();

private:
    QSharedPointer<GitBase> mGit;
    QSharedPointer<GitCache> mCache;
    CommitInfoPanel* mInfoPanelBase = nullptr;
    CommitInfoPanel* mInfoPanelParent = nullptr;
    QPinnableTabWidget* mCenterStackedWidget = nullptr;
    QMap<QString, IDiffWidget*> mDiffWidgets;
    FileListWidget* fileListWidget = nullptr;
    QString mCurrentSha;
    QString mParentSha;
    void changeSelection(int index);
    void onTabClosed(int index);
    void onDoubleClick(QListWidgetItem* item);
};
