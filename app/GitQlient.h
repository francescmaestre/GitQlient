#pragma once

/****************************************************************************************
 ** GitQlient is an application to manage and operate one or several Git repositories. With
 ** GitQlient you will be able to add commits, branches and manage all the options Git provides.
 ** Copyright (C) 2021  Francesc Maestre
 **
 ** LinkedIn: https://www.linkedin.com/in/francescmaestre/
 **
 ** This program is free software; you can redistribute it and/or
 ** modify it under the terms of the GNU Lesser General Public
 ** License as published by the Free Software Foundation; either
 ** version 2 of the License, or (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 ** Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public
 ** License along with this library; if not, write to the Free Software
 ** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 ***************************************************************************************/

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
    void conditionallyOpenPreConfigDlg(const QSharedPointer<GitBase>& git);
    void updateTabName();
    void moveLogsBeforeClose();
    void closeRepoIfNotPinned();
};
