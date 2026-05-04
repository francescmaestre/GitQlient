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

#include <QButtonGroup>
#include <QMap>
#include <QWidget>

class GitBase;
class QTimer;
class FileEditor;
class QPushButton;
class QLabel;
class QAbstractButton;

namespace Ui
{
    class ConfigWidget;
}

class ConfigWidget : public QWidget
{
    Q_OBJECT

signals:
    void goBack();
    void reloadDiffFont();
    void buildSystemEnabled(bool enabled);
    void gitServerEnabled(bool enabled);
    void terminalEnabled(bool enabled);
    void commitTitleMaxLenghtChanged();
    void moveLogsAndClose();
    void autoRefreshChanged(int seconds);

public:
    explicit ConfigWidget(const QSharedPointer<GitBase>& git, QWidget* parent = nullptr);
    ~ConfigWidget();

    void onPanelsVisibilityChanged();

private:
    Ui::ConfigWidget* ui;
    QSharedPointer<GitBase> mGit;
    bool mShowResetMsg = false;
    QTimer* mFeedbackTimer = nullptr;
    QPushButton* mSave = nullptr;
    FileEditor* mLocalGit = nullptr;
    FileEditor* mGlobalGit = nullptr;
    QButtonGroup* mDownloadButtons = nullptr;
    QVector<QWidget*> mPluginWidgets;
    QStringList mPluginNames;
    QPushButton* mPbFeaturesTour;

    void clearLogs();
    void clearFolder(const QString& folder, QLabel* label);
    void calculateLogsSize();
    uint64_t calculateDirSize(const QString& dirPath);
    void enableWidgets();
    void saveFile();
    void showCredentialsDlg();
    void selectFolder();
    void selectEditor();
    void useDefaultLogsFolder();
    void readRemotePluginsInfo();
    void showFeaturesTour();
    void fillLanguageBox() const;

private slots:
    void saveConfig();
    void onCredentialsOptionChanged(QAbstractButton* button);
};
