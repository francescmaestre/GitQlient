#pragma once

#include <QDialog>

namespace Ui
{
    class InputShaDlg;
}

class GitCache;
class GitBase;

enum class InputShaDlgMode
{
    CREATE,
    CREATE_CHECKOUT,
    CREATE_FROM_COMMIT,
    CREATE_CHECKOUT_FROM_COMMIT,
    RENAME,
    STASH_BRANCH,
    PUSH_UPSTREAM
};

struct InputShaDlgConfig
{
    QString mCurrentBranchName;
    InputShaDlgMode mDialogMode;
    QSharedPointer<GitCache> mCache;
};

class InputShaDlg : public QDialog
{
    Q_OBJECT

public:
    explicit InputShaDlg(const QString& branch, QSharedPointer<GitBase> git, QWidget* parent = nullptr);
    ~InputShaDlg() override;

private:
    Ui::InputShaDlg* ui = nullptr;
    QSharedPointer<GitBase> mGit;
    QString mBranch;

    void accept() override;
};
