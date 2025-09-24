#pragma once

#include <QDialog>

namespace Ui
{
    class GitConfigDlg;
}

class GitBase;

class GitConfigDlg : public QDialog
{
    Q_OBJECT

public:
    explicit GitConfigDlg(const QSharedPointer<GitBase>& git, QWidget* parent = nullptr);
    ~GitConfigDlg() override;

    void keyPressEvent(QKeyEvent* e) override;
    void closeEvent(QCloseEvent* e) override;
    void close();

private:
    Ui::GitConfigDlg* ui;
    QSharedPointer<GitBase> mGit;
    bool mPrepareToClose = false;

    void accept() override;
    void copyGlobalSettings(int checkState);
};
