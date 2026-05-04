#pragma once

#include <AGitProcess.h>
#include <GitBase.h>
#include <GitRepoConfig.h>
#include <system/GitRepoLoader.h>

#include <QMetaObject>

// ---------------------------------------------------------------------------
// FakeLogRequestor
//
// Replaces GitRequestorProcess in benchmarks: emits procDataReady with
// pre-loaded data the moment run() is called, without ever spawning git.
// ---------------------------------------------------------------------------
class FakeLogRequestor : public AGitProcess
{
public:
    explicit FakeLogRequestor(GitRepoConfig config, const QByteArray& data)
        : AGitProcess(std::move(config))
        , mData(data)
    {
    }

    GitExecResult run(const QString&) override
    {
        QMetaObject::invokeMethod(
            this,
            [this]() {
                if (!mCanceling)
                    emit procDataReady(mData);
                deleteLater();
            },
            Qt::QueuedConnection);
        return { true, {} };
    }

protected:
    void onFinished(int, QProcess::ExitStatus) override {}

private:
    QByteArray mData;
};

// ---------------------------------------------------------------------------
// BenchmarkableLoader
//
// Subclass that overrides createLogRequestor() to inject FakeLogRequestor.
// ---------------------------------------------------------------------------
class BenchmarkableLoader : public GitRepoLoader
{
public:
    BenchmarkableLoader(
        QSharedPointer<GitBase> git,
        QSharedPointer<SacredTimeline> cache,
        const QSharedPointer<Graph::TemporalLoom>& loom,
        const QByteArray& logData)
        : GitRepoLoader(git, cache, loom)
        , mGit(std::move(git))
        , mLogData(logData)
    {
    }

protected:
    bool resolveWorkingDirectory() override { return true; }
    WipData fetchWipData() const override { return {}; }

    AGitProcess* createLogRequestor() override
    {
        return new FakeLogRequestor(mGit->config(), mLogData);
    }

private:
    QSharedPointer<GitBase> mGit;
    const QByteArray& mLogData;
};
