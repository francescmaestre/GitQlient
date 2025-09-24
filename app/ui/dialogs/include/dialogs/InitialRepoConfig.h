#pragma once

#include <QDialog>

namespace Ui
{
    class InitialRepoConfig;
}

class GitQlientSettings;
class GitBase;
class QAbstractButton;

class InitialRepoConfig : public QDialog
{
    Q_OBJECT

public:
    explicit InitialRepoConfig(
        const QSharedPointer<GitBase>& git,
        const QSharedPointer<GitQlientSettings>& settings,
        QWidget* parent = nullptr);
    ~InitialRepoConfig();

    void accept() override;

private:
    Ui::InitialRepoConfig* ui;
    QSharedPointer<GitBase> mGit;
    QSharedPointer<GitQlientSettings> mSettings;
private slots:
    void onCredentialsOptionChanged(QAbstractButton* button);
};
