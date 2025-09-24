#pragma once

#include <QFrame>
#include <QMap>

class GitBase;
class QVBoxLayout;
class QPushButton;
class MergeInfoWidget;
class QLineEdit;
class QTextEdit;
class FileDiffWidget;
class RevisionFiles;
class GitCache;
class QListWidget;
class QListWidgetItem;
class QStackedWidget;

class MergeWidget : public QFrame
{
    Q_OBJECT

signals:
    void signalMergeFinished();

public:
    enum class ConflictReason
    {
        Merge,
        CherryPick,
        Pull
    };

    explicit MergeWidget(
        const QSharedPointer<GitCache>& gitQlientCache, const QSharedPointer<GitBase>& git, QWidget* parent = nullptr);

    void configure(const RevisionFiles& files, ConflictReason reason);
    void configureForCherryPick(const RevisionFiles& files, const QStringList& pendingShas = QStringList());
    void configureForRebase();

private:
    QSharedPointer<GitCache> mGitQlientCache;
    QSharedPointer<GitBase> mGit;
    QListWidget* mConflictFiles = nullptr;
    QListWidget* mMergedFiles = nullptr;
    QLineEdit* mCommitTitle = nullptr;
    QTextEdit* mDescription = nullptr;
    QPushButton* mMergeBtn = nullptr;
    QPushButton* mAbortBtn = nullptr;
    ConflictReason mReason = ConflictReason::Merge;
    QStackedWidget* mStacked = nullptr;
    FileDiffWidget* mFileDiff = nullptr;
    QStringList mPendingShas;

    void fillButtonFileList(const RevisionFiles& files);
    void changeDiffView(QListWidgetItem* item);
    void abort();
    bool checkMsg(QString& msg);
    void commit();
    void removeMergeComponents();
    void onConflictResolved(const QString& fileName);
    void cherryPickCommit();
};
