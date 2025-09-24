#pragma once

#include <GitExecResult.h>
#include <cache/Commit.h>

#include <QObject>
#include <QSharedPointer>
#include <QVector>

class GitBase;
class GitCache;
namespace Graph
{
    class Cache;
}
class GitQlientSettings;
class GitTags;
class GitRequestorProcess;

class GitRepoLoader : public QObject
{
    Q_OBJECT

signals:
    void signalLoadingStarted();
    void signalLoadingFinished(bool full);
    void cancelAllProcesses(QPrivateSignal);
public slots:
    void loadLogHistory();
    void loadReferences();
    void loadAll();

public:
    explicit GitRepoLoader(
        QSharedPointer<GitBase> gitBase,
        QSharedPointer<GitCache> cache,
        const QSharedPointer<Graph::Cache>& graphCache,
        const QSharedPointer<GitQlientSettings>& settings,
        QObject* parent = nullptr);
    void cancelAll();
    void setShowAll(bool showAll = true) { mShowAll = showAll; }

private:
    bool mShowAll = true;
    bool mLocked = false;
    bool mRefreshReferences = true;
    std::atomic<int> mSteps{0};
    QSharedPointer<GitBase> mGitBase;
    QSharedPointer<GitCache> mRevCache;
    QSharedPointer<Graph::Cache> mGraphCache;
    QSharedPointer<GitQlientSettings> mSettings;
    QSharedPointer<GitTags> mGitTags;
    GitRequestorProcess* mRevRequestor = nullptr;
    GitRequestorProcess* mRefRequestor = nullptr;

    bool configureRepoDirectory();
    void requestReferences();
    void processReferences(QByteArray ba);
    void requestRevisions();
    void processRevisions(QByteArray ba);
    QVector<Commit> processUnsignedLog(QByteArray& log) const;
    QVector<Commit> processSignedLog(QByteArray& log) const;
    void notifyLoadingFinished();
};
