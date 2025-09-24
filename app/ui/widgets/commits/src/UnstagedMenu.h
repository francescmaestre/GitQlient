#pragma once

#include <QMenu>

class GitBase;

class UnstagedMenu : public QMenu
{
    Q_OBJECT

signals:
    void signalShowDiff(const QString& fileName);
    void signalCommitAll();
    void changeReverted(const QString& revertedFile);
    void signalRevertAll();
    void signalCheckedOut();
    void signalShowFileHistory(const QString& fileName);
    void signalStageFile();
    void untrackedDeleted();

public:
    explicit UnstagedMenu(const QSharedPointer<GitBase>& git, const QString& fileName, QWidget* parent = nullptr);

private:
    QSharedPointer<GitBase> mGit;
    QString mFileName;

    bool addEntryToGitIgnore(const QString& entry);
    void onDeleteFile();
    void openFileExplorer();
    void openExternalEditor();
    void deleteUntracked();
};
