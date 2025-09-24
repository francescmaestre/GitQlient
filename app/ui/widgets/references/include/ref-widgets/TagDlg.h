#pragma once

#include <QDialog>

class GitBase;

namespace Ui
{
    class TagDlg;
}

class TagDlg : public QDialog
{
    Q_OBJECT

public:
    explicit TagDlg(const QSharedPointer<GitBase>& git, const QString& sha, QWidget* parent = nullptr);

    TagDlg(const TagDlg&) = delete;
    TagDlg& operator=(const TagDlg&) = delete;
    ~TagDlg() override;

    void accept() override;

private:
    Ui::TagDlg* ui;
    QSharedPointer<GitBase> mGit;
    QString mSha;
};
