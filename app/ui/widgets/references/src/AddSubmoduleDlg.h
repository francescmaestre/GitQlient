#pragma once

#include <QDialog>

class GitSubmodules;

namespace Ui
{
    class AddSubmoduleDlg;
}

class AddSubmoduleDlg : public QDialog
{
    Q_OBJECT

public:
    explicit AddSubmoduleDlg(const QSharedPointer<GitSubmodules>& git, QWidget* parent = nullptr);
    ~AddSubmoduleDlg() override;

    void accept() override;

private:
    Ui::AddSubmoduleDlg* ui;
    QSharedPointer<GitSubmodules> mGit;
};
