#pragma once

#include <QMenu>

class GitBase;

class SubmodulesContextMenu : public QMenu
{
    Q_OBJECT

signals:
    void openSubmodule(const QString& path);
    void infoUpdated();

public:
    explicit SubmodulesContextMenu(
        const QSharedPointer<GitBase>& git, const QModelIndex& index, int totalSubmodules, QWidget* parent = nullptr);
    explicit SubmodulesContextMenu(const QSharedPointer<GitBase>& git, const QString& name, QWidget* parent = nullptr);

private:
    QSharedPointer<GitBase> mGit;
};
