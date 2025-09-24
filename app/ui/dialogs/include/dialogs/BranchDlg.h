#pragma once

#include <QDialog>

namespace Ui
{
    class BranchDlg;
}

class GitCache;
class GitBase;

enum class BranchDlgMode
{
    CREATE,
    CREATE_CHECKOUT,
    CREATE_FROM_COMMIT,
    CREATE_CHECKOUT_FROM_COMMIT,
    RENAME,
    STASH_BRANCH,
    PUSH_UPSTREAM
};

struct BranchDlgConfig
{
    QString mCurrentBranchName;
    BranchDlgMode mDialogMode;
    QSharedPointer<GitCache> mCache;
    QSharedPointer<GitBase> mGit;
};

class BranchDlg : public QDialog
{
    Q_OBJECT

public:
    explicit BranchDlg(BranchDlgConfig config, QWidget* parent = nullptr);
    ~BranchDlg() override;

private:
    Ui::BranchDlg* ui = nullptr;
    BranchDlgConfig mConfig;

    void checkNewBranchName();
    void replaceWhiteSpaces(const QString& newName);
    void accept() override;
    void copyBranchName();
};
