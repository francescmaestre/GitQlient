#pragma once

#include <QFrame>
#include <QPointer>

class QToolButton;
class QPushButton;
class GitBase;
class GitCache;
class GitCache;
class GitQlientUpdater;
class QButtonGroup;
class QHBoxLayout;

enum class ControlsMainViews
{
    History,
    Diff,
    Blame,
    Merge,
};

class Controls : public QFrame
{
    Q_OBJECT

signals:
    void signalGoMerge();
    void signalPullConflict();
    void signalRefreshPRsCache();
    void requestFullReload();
    void requestReferencesReload();

public:
    explicit Controls(
        const QSharedPointer<GitCache>& cache, const QSharedPointer<GitBase>& git, QWidget* parent = nullptr);
    ~Controls() = default;

    void setCurrentSha(const QString& sha) { mCurrentSha = sha; }
    void enableButtons(bool enabled);
    void fetchAll();
    void activateMergeWarning();
    void disableMergeWarning();
    ControlsMainViews getCurrentSelectedButton() const;

private:
    QString mCurrentSha;
    QSharedPointer<GitCache> mCache;
    QSharedPointer<GitBase> mGit;
    QToolButton* mStashPop = nullptr;
    QToolButton* mStashPush = nullptr;
    QToolButton* mPullBtn = nullptr;
    QToolButton* mPullOptions = nullptr;
    QToolButton* mPushBtn = nullptr;
    QToolButton* mRefreshBtn = nullptr;
    QToolButton* mVersionCheck = nullptr;
    QPushButton* mMergeWarning = nullptr;
    GitQlientUpdater* mUpdater = nullptr;
    QFrame* mLastSeparator = nullptr;

    QToolButton*
    createToolButton(const QString& iconPath, const QString& tooltip, const QKeySequence& shortcut = QKeySequence());

    void stashPush();
    void stashPop();
    void pullCurrentBranch();
    void pushCurrentBranch();
    void pruneBranches();
    bool eventFilter(QObject* obj, QEvent* event);
};
