#pragma once

#include <QDialog>

namespace Ui
{
    class AddSubtreeDlg;
}

class GitBase;

class AddSubtreeDlg : public QDialog
{
    Q_OBJECT

public:
    explicit AddSubtreeDlg(const QSharedPointer<GitBase>& git, QWidget* parent = nullptr);
    explicit AddSubtreeDlg(const QString& prefix, const QSharedPointer<GitBase>& git, QWidget* parent = nullptr);
    explicit AddSubtreeDlg(
        const QString& prefix,
        const QString& url,
        const QString& reference,
        const QSharedPointer<GitBase>& git,
        QWidget* parent = nullptr);

    ~AddSubtreeDlg() override;
    void accept() override;

private:
    Ui::AddSubtreeDlg* ui;
    QSharedPointer<GitBase> mGit;

    void proposeName();
};
