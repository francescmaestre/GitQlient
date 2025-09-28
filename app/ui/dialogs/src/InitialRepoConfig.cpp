#include "ui_InitialRepoConfig.h"
#include <dialogs/InitialRepoConfig.h>

#include <GitConfig.h>
#include <GitCredentials.h>
#include <dialogs/CredentialsDlg.h>
#include <system/GitQlientStyles.h>
#include <system/SettingsKeys.h>

#include <QSettings>

using namespace System;

InitialRepoConfig::InitialRepoConfig(const QSharedPointer<GitBase>& git, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::InitialRepoConfig)
    , mGit(git)
{
    setAttribute(Qt::WA_DeleteOnClose);

    ui->setupUi(this);

    QSettings settings;
    ui->sbMaxCommits->setValue(settings.value(GlobalKey::MaxCommits, 0).toInt());

    QScopedPointer<GitConfig> gitConfig(new GitConfig(git));

    const auto url = gitConfig->getServerUrl();
    ui->credentialsFrames->setVisible(url.startsWith("https"));

    connect(
        ui->buttonGroup,
        qOverload<QAbstractButton*>(&QButtonGroup::buttonClicked),
        this,
        &InitialRepoConfig::onCredentialsOptionChanged);
}

InitialRepoConfig::~InitialRepoConfig()
{
    QSettings settings;
    settings.setValue(GlobalKey::MaxCommits, ui->sbMaxCommits->value());

    delete ui;
}

void InitialRepoConfig::accept()
{
    // Store credentials if allowed and the user checked the box
    if (ui->credentialsFrames->isVisible() && ui->chbCredentials->isChecked())
    {
        if (ui->rbCache->isChecked())
            GitCredentials::configureCache(ui->sbTimeout->value(), mGit);
        else
        {
            CredentialsDlg dlg(mGit, this);
            dlg.exec();
        }
    }

    QDialog::accept();
}

void InitialRepoConfig::onCredentialsOptionChanged(QAbstractButton* button)
{
    ui->sbTimeout->setEnabled(button == ui->rbCache);
}
