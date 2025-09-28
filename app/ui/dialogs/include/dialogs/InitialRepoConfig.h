#pragma once

#include <QDialog>

namespace Ui
{
    class InitialRepoConfig;
}

class GitBase;
class QAbstractButton;

class InitialRepoConfig : public QDialog
{
    Q_OBJECT

public:
    explicit InitialRepoConfig(
        const QSharedPointer<GitBase>& git,
        QWidget* parent = nullptr);
    ~InitialRepoConfig();

    void accept() override;

private:
    Ui::InitialRepoConfig* ui;
    QSharedPointer<GitBase> mGit;
private slots:
    void onCredentialsOptionChanged(QAbstractButton* button);
};
