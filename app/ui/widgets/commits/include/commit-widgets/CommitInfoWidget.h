#pragma once

#include <QFrame>

class GitCache;
class GitBase;
class QLabel;
class FileListWidget;
class CommitInfoPanel;
class QListWidgetItem;

class CommitInfoWidget : public QFrame
{
    Q_OBJECT

signals:
    void showFileDiff(const QString& file, const QString& currentSha, const QString& previousSha);
    void signalOpenFileContextMenu(const QString&, int);
    void signalShowFileHistory(const QString& fileName);
    void signalReturnToHistory();
    void signalEditFile(const QString& fileName, int line, int column);

public:
    explicit CommitInfoWidget(
        const QSharedPointer<GitCache>& cache, const QSharedPointer<GitBase>& git, QWidget* parent = nullptr);

    void configure(const QString& sha);
    QString getCurrentCommitSha() const;
    void clear();

private:
    QSharedPointer<GitCache> mCache;
    QSharedPointer<GitBase> mGit;
    QString mCurrentSha;
    QString mParentSha;
    CommitInfoPanel* mInfoPanel = nullptr;
    FileListWidget* mFileListWidget = nullptr;
    QListWidgetItem* mLastSelectedItem = nullptr;

    void handleItemClick(QListWidgetItem* item);
};
