#pragma once

#include <QDialog>

namespace Ui
{
    class AddSubmoduleDlg;
}

class GitBase;

class AddRemoteDlg : public QDialog
{
    Q_OBJECT

public:
    explicit AddRemoteDlg(const QSharedPointer<GitBase>& git, QWidget* parent = nullptr);
    ~AddRemoteDlg() override;

    void accept() override;

private:
    Ui::AddSubmoduleDlg* ui;
    QSharedPointer<GitBase> mGit;

    void proposeName();
};
