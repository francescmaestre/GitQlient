#pragma once

#include <QListWidget>

class GitBase;
class GitCache;
class FileListDelegate;

class FileListWidget : public QListWidget
{
    Q_OBJECT

signals:
    void signalShowFileHistory(const QString& fileName);
    void signalEditFile(const QString& fileName, int line, int column);

public:
    explicit FileListWidget(
        const QSharedPointer<GitBase>& git, QSharedPointer<GitCache> cache, QWidget* parent = nullptr);
    ~FileListWidget() override;

    void insertFiles(const QString& currentSha, const QString& compareToSha);

private:
    QSharedPointer<GitBase> mGit;
    QSharedPointer<GitCache> mCache;
    FileListDelegate* mFileDelegate = nullptr;
    QString mCurrentSha;

    void showContextMenu(const QPoint&);
    void addItem(const QString& label, const QColor& clr);
};
