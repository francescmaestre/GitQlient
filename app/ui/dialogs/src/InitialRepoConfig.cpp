#include "ui_InitialRepoConfig.h"
#include <dialogs/InitialRepoConfig.h>

#include <GitConfig.h>
#include <GitCredentials.h>
#include <dialogs/CredentialsDlg.h>
#include <system/GitQlientSettings.h>
#include <system/GitQlientStyles.h>

InitialRepoConfig::InitialRepoConfig(
    const QSharedPointer<GitBase>& git, const QSharedPointer<GitQlientSettings>& _settings, QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::InitialRepoConfig)
    , mGit(git)
    , mSettings(_settings)
{
    setAttribute(Qt::WA_DeleteOnClose);

    ui->setupUi(this);

    QSettings settings;
    ui->sbMaxCommits->setValue(settings.value("MaxCommits", 0).toInt());

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
    settings.setValue("MaxCommits", ui->sbMaxCommits->value());

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
