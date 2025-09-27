#include "ui_ConfigWidget.h"
#include <main-widgets/ConfigWidget.h>

#include <GitBase.h>
#include <GitConfig.h>
#include <GitCredentials.h>
#include <QLogger>
#include <custom-widgets/CheckBox.h>
#include <dialogs/CredentialsDlg.h>
#include <dialogs/NewVersionInfoDlg.h>
#include <diff-widgets/FileEditor.h>
#include <system/GitQlientSettings.h>

#include <QApplication>
#include <QDir>
#include <QDirIterator>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDatabase>
#include <QGridLayout>
#include <QMessageBox>
#include <QProcess>
#include <QPushButton>
#include <QStandardPaths>
#include <QTimer>

using namespace QLogger;

namespace
{
    qint64 dirSize(QString dirPath)
    {
        qint64 size = 0;
        QDir dir(dirPath);

        auto entryList = dir.entryList(QDir::Files | QDir::System | QDir::Hidden);

        for (const auto& filePath : std::as_const(entryList))
        {
            QFileInfo fi(dir, filePath);
            size += fi.size();
        }

        entryList = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot | QDir::System | QDir::Hidden);

        for (const auto& childDirPath : std::as_const(entryList))
            size += dirSize(dirPath + QDir::separator() + childDirPath);

        return size;
    }
} // namespace

ConfigWidget::ConfigWidget(const QSharedPointer<GitBase>& git, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::ConfigWidget)
    , mGit(git)
    , mFeedbackTimer(new QTimer(this))
    , mSave(new QPushButton(this))
    , mDownloadButtons(new QButtonGroup(this))
{
    ui->setupUi(this);

    ui->lTerminalColorScheme->setVisible(false);
    ui->cbTerminalColorScheme->setVisible(false);

    mFeedbackTimer->setInterval(3000);

    mSave->setIcon(QIcon(":/icons/save"));
    mSave->setToolTip(tr("Save"));
    connect(mSave, &QPushButton::clicked, this, &ConfigWidget::saveFile);
    ui->tabWidget->setCornerWidget(mSave);

    ui->mainLayout->setColumnStretch(0, 1);
    ui->mainLayout->setColumnStretch(1, 3);

    const auto localGitLayout = new QVBoxLayout(ui->localGit);
    localGitLayout->setContentsMargins(QMargins());

    mLocalGit = new FileEditor(false, this);
    mLocalGit->editFile(mGit->getGitDir().append("/config"));
    localGitLayout->addWidget(mLocalGit);

    const auto globalGitLayout = new QVBoxLayout(ui->globalGit);
    globalGitLayout->setContentsMargins(QMargins());

    mGlobalGit = new FileEditor(false, this);
    mGlobalGit->editFile(QString("%1/%2").arg(
        QStandardPaths::writableLocation(QStandardPaths::HomeLocation), QString::fromUtf8(".gitconfig")));
    globalGitLayout->addWidget(mGlobalGit);

    QSettings settings;
    QSettings qSettings;

    const auto logsFolder = settings.value("logsFolder").toString();
    if (logsFolder.isEmpty())
        settings.setValue("logsFolder", QString(QDir::currentPath()).append("/logs/"));

    ui->leLogsLocation->setText(settings.value("logsFolder").toString());

    ui->chDevMode->setChecked(qSettings.value("DevMode", false).toBool());
    enableWidgets();

    ui->chDisableLogs->setChecked(settings.value("logsDisabled", true).toBool());
    ui->cbLogLevel->setCurrentIndex(settings.value("logsLevel", static_cast<int>(LogLevel::Warning)).toInt());
    ui->spCommitTitleLength->setValue(settings.value("commitTitleMaxLength", 50).toInt());
    ui->sbUiFontSize->setValue(
        settings.value("UiBaseFontSize", QFontDatabase::systemFont(QFontDatabase::GeneralFont).pointSize()).toInt());
    ui->sbHistoryViewFontSize->setValue(
        settings.value("HistoryView/FontSize", QFontDatabase::systemFont(QFontDatabase::GeneralFont).pointSize())
            .toInt());
    ui->rbShowCommit->setChecked(settings.value("HistoryView/PreferCommit", true).toBool());
    ui->sbEditorFontSize->setValue(settings.value("FileDiffView/FontSize", 8).toInt());

#ifdef Q_OS_LINUX
    ui->leEditor->setText(settings.value("ExternalEditor", QString()).toString());
    ui->leExtFileExplorer->setText(settings.value("FileExplorer", "xdg-open").toString());
#else
    ui->leExtFileExplorer->setHidden(true);
    ui->labelExtFileExplorer->setHidden(true);
#endif

    const auto originalStyles = settings.value("colorSchema", 0).toInt();

    ui->cbStyle->setCurrentIndex(originalStyles);
    connect(
        ui->cbStyle,
        static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
        this,
        [this, originalStyles](int newIndex) {
            mShowResetMsg = newIndex != originalStyles;
            saveConfig();
        });

    ui->sbMaxCommits->setValue(qSettings.value("MaxCommits", 0).toInt());

    ui->tabWidget->setCurrentIndex(0);
    connect(ui->pbClearLogs, &ButtonLink::clicked, this, &ConfigWidget::clearLogs);

    ui->cbLocal->setChecked(qSettings.value("LocalHeader", true).toBool());
    ui->cbRemote->setChecked(qSettings.value("RemoteHeader", true).toBool());
    ui->cbTags->setChecked(qSettings.value("TagsHeader", true).toBool());
    ui->cbStash->setChecked(qSettings.value("StashesHeader", true).toBool());
    ui->cbSubmodule->setChecked(qSettings.value("SubmodulesHeader", true).toBool());
    ui->cbSubtree->setChecked(qSettings.value("SubtreeHeader", true).toBool());

    QScopedPointer<GitConfig> gitConfig(new GitConfig(mGit));

    const auto url = gitConfig->getServerUrl();
    ui->credentialsFrames->setVisible(url.startsWith("https"));

    fillLanguageBox();

    connect(ui->buttonGroup, &QButtonGroup::buttonClicked, this, &ConfigWidget::onCredentialsOptionChanged);
    connect(ui->pbAddCredentials, &QPushButton::clicked, this, &ConfigWidget::showCredentialsDlg);

    connect(ui->chDevMode, &CheckBox::stateChanged, this, &ConfigWidget::enableWidgets);
    connect(ui->chDisableLogs, &CheckBox::stateChanged, this, &ConfigWidget::saveConfig);
    connect(ui->cbLogLevel, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));
    connect(ui->leGitPath, &QLineEdit::editingFinished, this, &ConfigWidget::saveConfig);
    connect(ui->spCommitTitleLength, SIGNAL(valueChanged(int)), this, SLOT(saveConfig()));
    connect(ui->sbUiFontSize, SIGNAL(valueChanged(int)), this, SLOT(saveConfig()));
    connect(ui->sbHistoryViewFontSize, SIGNAL(valueChanged(int)), this, SLOT(saveConfig()));
    connect(ui->bgHistoryViewPreferredView, &QButtonGroup::buttonClicked, this, &ConfigWidget::saveConfig);
    connect(ui->sbEditorFontSize, SIGNAL(valueChanged(int)), this, SLOT(saveConfig()));
    connect(ui->sbMaxCommits, SIGNAL(valueChanged(int)), this, SLOT(saveConfig()));
    connect(ui->autoRefresh, SIGNAL(valueChanged(int)), this, SLOT(saveConfig()));
    connect(ui->cbLocal, &QCheckBox::stateChanged, this, &ConfigWidget::saveConfig);
    connect(ui->cbRemote, &QCheckBox::stateChanged, this, &ConfigWidget::saveConfig);
    connect(ui->cbTags, &QCheckBox::stateChanged, this, &ConfigWidget::saveConfig);
    connect(ui->cbStash, &QCheckBox::stateChanged, this, &ConfigWidget::saveConfig);
    connect(ui->cbSubmodule, &QCheckBox::stateChanged, this, &ConfigWidget::saveConfig);
    connect(ui->cbSubtree, &QCheckBox::stateChanged, this, &ConfigWidget::saveConfig);
    connect(ui->pbSelectFolder, &QPushButton::clicked, this, &ConfigWidget::selectFolder);
    connect(ui->pbDefault, &QPushButton::clicked, this, &ConfigWidget::useDefaultLogsFolder);
    connect(ui->leEditor, &QLineEdit::editingFinished, this, &ConfigWidget::saveConfig);
    connect(ui->pbSelectEditor, &QPushButton::clicked, this, &ConfigWidget::selectEditor);
    connect(ui->leExtFileExplorer, &QLineEdit::editingFinished, this, &ConfigWidget::saveConfig);
    connect(ui->pbFeaturesTour, &QPushButton::clicked, this, &ConfigWidget::showFeaturesTour);
    connect(ui->cbDiffView, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));
    connect(ui->cbBranchSeparator, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));
    connect(ui->cbLanguage, SIGNAL(currentIndexChanged(int)), this, SLOT(saveConfig()));
    connect(ui->leLogsLocation, &QLineEdit::editingFinished, this, &ConfigWidget::saveConfig);
    connect(ui->pbBack, &QPushButton::clicked, this, &ConfigWidget::goBack);

    ui->cbDiffView->setCurrentIndex(settings.value("DefaultDiffView").toInt());
    ui->cbBranchSeparator->setCurrentText(settings.value("BranchSeparator", "-").toString());

    auto size = calculateDirSize(ui->leLogsLocation->text());
    ui->lLogsSize->setText(QString("%1 KB").arg(size));
}

ConfigWidget::~ConfigWidget() { delete ui; }

void ConfigWidget::onPanelsVisibilityChanged()
{
    QSettings settings;

    ui->cbLocal->setChecked(settings.value("LocalHeader", true).toBool());
    ui->cbRemote->setChecked(settings.value("RemoteHeader", true).toBool());
    ui->cbTags->setChecked(settings.value("TagsHeader", true).toBool());
    ui->cbStash->setChecked(settings.value("StashesHeader", true).toBool());
    ui->cbSubmodule->setChecked(settings.value("SubmodulesHeader", true).toBool());
    ui->cbSubtree->setChecked(settings.value("SubtreeHeader", true).toBool());
}

void ConfigWidget::onCredentialsOptionChanged(QAbstractButton* button)
{
    ui->sbTimeout->setEnabled(button == ui->rbCache);
}

void ConfigWidget::clearLogs()
{
    const auto path = ui->leLogsLocation->text();
    QDir dir(path, {"GitQlient_*.log"});
    const auto entryList = dir.entryList();
    for (const auto& filename : entryList)
        dir.remove(filename);

    const auto size = calculateDirSize(path);
    ui->lLogsSize->setText(QString("%1 KB").arg(size));
}

void ConfigWidget::clearFolder(const QString& folder, QLabel* label)
{
    const auto path = folder;
    QProcess p;
    p.setWorkingDirectory(path);
    p.start("rm", {"-rf", path});

    if (p.waitForFinished())
    {
        const auto size = calculateDirSize(path);
        label->setText(QString("%1 KB").arg(size));
    }
}

uint64_t ConfigWidget::calculateDirSize(const QString& dirPath)
{
    auto size = 0U;
    QDir dir(dirPath);
    QDir::Filters dirFilters = QDir::Dirs | QDir::NoDotAndDotDot | QDir::System | QDir::Hidden | QDir::Files;
    const auto& list = dir.entryInfoList(dirFilters);

    for (const QFileInfo& file : list)
    {
        size += file.size();
        size += dirSize(dirPath + "/" + file.fileName());
    }

    return size / 1024.0;
}

void ConfigWidget::saveConfig()
{
    mFeedbackTimer->stop();

    ui->lFeedback->setText(tr("Changes saved"));

    QSettings settings;

    settings.setValue("logsDisabled", ui->chDisableLogs->isChecked());
    settings.setValue("logsLevel", ui->cbLogLevel->currentIndex());
    settings.setValue("logsFolder", ui->leLogsLocation->text());
    settings.setValue("commitTitleMaxLength", ui->spCommitTitleLength->value());
    settings.setValue("UiBaseFontSize", ui->sbUiFontSize->value());
    settings.setValue("HistoryView/FontSize", ui->sbHistoryViewFontSize->value());
    settings.setValue("HistoryView/PreferCommit", ui->rbShowCommit->isChecked());
    settings.setValue("FileDiffView/FontSize", ui->sbEditorFontSize->value());
    settings.setValue("colorSchema", ui->cbStyle->currentIndex());
    settings.setValue("gitLocation", ui->leGitPath->text());
    settings.setValue("DefaultDiffView", ui->cbDiffView->currentIndex());
    settings.setValue("BranchSeparator", ui->cbBranchSeparator->currentText());
    settings.setValue("UILanguage", ui->cbLanguage->currentData().toString());

    if (!ui->leEditor->text().isEmpty())
        settings.setValue("ExternalEditor", ui->leEditor->text());

#ifdef Q_OS_LINUX
    settings.setValue("FileExplorer", ui->leExtFileExplorer->text());
#endif

    mLocalGit->changeFontSize();
    mGlobalGit->changeFontSize();

    emit reloadDiffFont();
    emit commitTitleMaxLenghtChanged();

    if (mShowResetMsg || qobject_cast<QComboBox*>(sender()) == ui->cbLanguage)
    {
        QMessageBox::information(
            this, tr("Reset needed!"), tr("You need to restart GitQlient to see the changes in the styles applied."));
    }

    const auto logger = QLoggerManager::getInstance();
    logger->overwriteLogLevel(static_cast<LogLevel>(ui->cbLogLevel->currentIndex()));

    if (ui->chDisableLogs->isChecked())
        logger->pause();
    else
        logger->resume();

    settings.setValue("AutoRefresh", ui->autoRefresh->value());

    emit autoRefreshChanged(ui->autoRefresh->value());

    settings.setValue("MaxCommits", ui->sbMaxCommits->value());

    settings.setValue("LocalHeader", ui->cbLocal->isChecked());
    settings.setValue("RemoteHeader", ui->cbRemote->isChecked());
    settings.setValue("TagsHeader", ui->cbTags->isChecked());
    settings.setValue("StashesHeader", ui->cbStash->isChecked());
    settings.setValue("SubmodulesHeader", ui->cbSubmodule->isChecked());
    settings.setValue("SubtreeHeader", ui->cbSubtree->isChecked());

    mFeedbackTimer->singleShot(3000, ui->lFeedback, &QLabel::clear);
}

void ConfigWidget::enableWidgets()
{
    const auto enable = ui->chDevMode->isChecked();

    QSettings settings;
    settings.setValue("DevMode", enable);

    ui->tabWidget->setEnabled(enable);
}

void ConfigWidget::saveFile()
{
    const auto id = ui->tabWidget->currentIndex();

    if (id == 0)
        mLocalGit->saveFile();
    else
        mGlobalGit->saveFile();
}

void ConfigWidget::showCredentialsDlg()
{
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
}

void ConfigWidget::selectFolder()
{
    const QString dirName(
        QFileDialog::getExistingDirectory(this, "Choose the directory for the GitQlient logs", QDir::currentPath()));

    if (!dirName.isEmpty() && dirName != QDir::currentPath().append("logs"))
    {
        QDir d(dirName);

        const auto ret = QMessageBox::information(
            this,
            tr("Restart needed!"),
            tr("The folder chosen to store GitQlient logs is: <br> <strong>%1</strong>. If you "
               "confirm the change, GitQlient will move all the logs to that folder. Once done, "
               "GitQlient will close. You need to restart it.")
                .arg(d.absolutePath()),
            QMessageBox::StandardButton::Ok,
            QMessageBox::StandardButton::Cancel);

        if (ret == QMessageBox::Ok)
        {
            ui->leLogsLocation->setText(d.absolutePath());

            saveConfig();

            emit moveLogsAndClose();
        }
    }
}

void ConfigWidget::selectEditor()
{
    const QString dirName(
        QFileDialog::getOpenFileName(this, "Choose the directory of the external editor", QDir::currentPath()));

    if (!dirName.isEmpty())
    {
        QDir d(dirName);

        ui->leEditor->setText(d.absolutePath());

        saveConfig();
    }
}

void ConfigWidget::useDefaultLogsFolder()
{
    const auto dir = QDir::currentPath().append("/logs");

    if (dir != ui->leLogsLocation->text())
    {
        const auto ret = QMessageBox::information(
            this,
            tr("Restart needed!"),
            tr("The folder chosen to store GitQlient logs is: <br> <strong>%1</strong>. If you "
               "confirm the change, GitQlient will move all the logs to that folder. Once done, "
               "GitQlient will close. You need to restart it.")
                .arg(dir),
            QMessageBox::StandardButton::Ok,
            QMessageBox::StandardButton::Cancel);

        if (ret == QMessageBox::Ok)
        {
            ui->leLogsLocation->setText(dir);

            saveConfig();

            emit moveLogsAndClose();
        }
    }
}

void ConfigWidget::readRemotePluginsInfo() { }

void ConfigWidget::showFeaturesTour()
{
    NewVersionInfoDlg dlg(this);
    dlg.setFixedSize(600, 400);
    dlg.exec();
}

void ConfigWidget::fillLanguageBox() const
{
    const auto currentLanguage = QSettings().value("UILanguage", "gitqlient_en").toString();

    const auto list = QDir(":translations", "gitqlient_*.qm").entryList();
    QDirIterator trIter(":translations", QStringList() << "gitqlient_*.qm");

    while (trIter.hasNext())
    {
        trIter.next();

        auto name = trIter.fileName();
        name.remove(".qm");

        const auto lang = name.mid(name.indexOf('_') + 1);
        QLocale tmpLocale(lang);
        const auto languageItem = QString::fromUtf8("%1 (%2)").arg(
            QLocale::languageToString(tmpLocale.language()), QLocale::territoryToString(tmpLocale.territory()));

        ui->cbLanguage->addItem(languageItem, name);

        if (name == currentLanguage)
            ui->cbLanguage->setCurrentIndex(ui->cbLanguage->count() - 1);
    }
}
