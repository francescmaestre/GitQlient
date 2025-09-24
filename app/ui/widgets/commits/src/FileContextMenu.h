#pragma once

#include <QMenu>

class FileContextMenu : public QMenu
{
    Q_OBJECT

signals:
    void signalOpenFileDiff();
    void signalShowFileHistory();
    void signalEditFile();

public:
    explicit FileContextMenu(
        const QString gitProject, const QString& file, bool editionAllowed = false, QWidget* parent = nullptr);

private:
    QString mFile;
    QString mGitProject;
    void openFileExplorer();
};
