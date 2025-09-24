#pragma once

#include <QFrame>

class QPushButton;
class QButtonGroup;
class GitConfig;
class ProgressDlg;
class GitQlientSettings;
class QVBoxLayout;

class InitScreen : public QFrame
{
    Q_OBJECT

signals:
    void signalOpenRepo(const QString& repoPath);
    void signalOpenRepo();
    void signalCloneRepo();
    void signalInitRepo();

public:
    explicit InitScreen(QWidget* parent = nullptr);

    void onRepoOpened();

private:
    QPushButton* mOpenRepo = nullptr;
    QPushButton* mCloneRepo = nullptr;
    QPushButton* mInitRepo = nullptr;
    QButtonGroup* mBtnGroup = nullptr;
    QVBoxLayout* mRecentProjectsLayout = nullptr;
    QVBoxLayout* mUsedProjectsLayout = nullptr;
    QWidget* mInnerWidget = nullptr;
    QWidget* mMostUsedInnerWidget = nullptr;

    QWidget* createRecentProjectsPage();
    QWidget* createUsedProjectsPage();
    void showAbout();
    void openConfigDlg();
};
