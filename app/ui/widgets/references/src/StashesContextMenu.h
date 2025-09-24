#pragma once

#include <QMenu>

class GitBase;

class StashesContextMenu : public QMenu
{
    Q_OBJECT

signals:
    void signalContentRemoved();
    void signalUpdateView();

public:
    explicit StashesContextMenu(const QSharedPointer<GitBase>& git, const QString& stashId, QWidget* parent = nullptr);

private:
    QSharedPointer<GitBase> mGit;
    QString mStashId;

    void drop();
    void clear();
    void branch();
};
