#pragma once

#include <QDialog>

namespace Ui
{
    class CredentialsDlg;
}

class GitBase;

class CredentialsDlg : public QDialog
{
    Q_OBJECT

public:
    explicit CredentialsDlg(const QSharedPointer<GitBase>& git, QWidget* parent = nullptr);
    ~CredentialsDlg();

    void accept() override;

private:
    Ui::CredentialsDlg* ui;
    QSharedPointer<GitBase> mGit;
};
