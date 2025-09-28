#pragma once

#include <QMap>
#include <QSet>
#include <QWidget>

class QPinnableTabWidget;
class InitScreen;
class ProgressDlg;
class GitConfig;
class GitQlientSettings;
class GitBase;
class ConfigWidget;

class QStackedLayout;

class GitQlient : public QWidget
{
    Q_OBJECT

public:
    explicit GitQlient(QWidget* parent = nullptr);
    ~GitQlient() override;

    void setRepositories(const QStringList& repositories);
    bool setArgumentsPostInit(const QStringList& arguments);
    void restorePinnedRepos();
    static bool parseArguments(const QStringList& arguments, QStringList* repos);

protected:
    bool eventFilter(QObject* obj, QEvent* event) override;

private:
    QStackedLayout* mStackedLayout = nullptr;
    QPinnableTabWidget* mRepos = nullptr;
    QMap<QWidget*, QString> mTabsMap;
    InitScreen* mInitWidget = nullptr;
    QSet<QString> mCurrentRepos;
    QSharedPointer<GitConfig> mGitConfig;
    ProgressDlg* mProgressDlg = nullptr;
    QString mPathToOpen;
    bool mMoveLogs = false;
    ConfigWidget* mConfigWidget = nullptr;

    void createRepoManagementMenu();
    void createOptionsMenu();
    void openRepo();
    void openRepoWithPath(const QString& path);
    void cloneRepo();
    void initRepo();
    void updateProgressDialog(QString stepDescription, int value);
    void showError(int error, QString description);
    void addRepoTab(const QString& repoPath);
    void addNewRepoTab(const QString& repoPath, bool pinned);
    void closeTab(int tabIndex);
    void onSuccessOpen(const QString& fullPath);
    void conditionallyOpenPreConfigDlg(
        const QSharedPointer<GitBase>& git);
    void updateTabName();
    void moveLogsBeforeClose();
    void closeRepoIfNotPinned();
};
