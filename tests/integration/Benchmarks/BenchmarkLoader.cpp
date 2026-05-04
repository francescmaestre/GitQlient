#include "BenchmarkCommon.h"

#include <cache/SacredTimeline.h>
#include <graph/TemporalLoom.h>

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QFile>
#include <QThreadPool>
#include <QTimer>

#include <algorithm>
#include <numeric>
#include <vector>

static bool runOnce(const QSharedPointer<GitBase>& git, const QByteArray& logData, qint64* outMs)
{
    auto cache = QSharedPointer<SacredTimeline>::create();
    auto graphCache = QSharedPointer<Graph::TemporalLoom>::create();

    bool finished = false;
    QEventLoop loop;
    QElapsedTimer timer;

    if (logData.isEmpty())
    {
        GitRepoLoader loader(git, cache, graphCache);
        QObject::connect(&loader, &GitRepoLoader::loadingFinished, [&finished, &loop] {
            finished = true;
            loop.quit();
        });
        timer.start();
        loader.loadAll();
        QTimer::singleShot(600'000, &loop, &QEventLoop::quit);
        loop.exec();
    }
    else
    {
        BenchmarkableLoader loader(git, cache, graphCache, logData);
        QObject::connect(&loader, &GitRepoLoader::loadingFinished, [&finished, &loop] {
            finished = true;
            loop.quit();
        });
        timer.start();
        loader.loadLogHistory();
        QTimer::singleShot(600'000, &loop, &QEventLoop::quit);
        loop.exec();
    }

    if (!finished)
    {
        qWarning("  [timeout] load did not finish within 10 minutes");
        return false;
    }

    if (outMs)
        *outMs = timer.elapsed();

    // Cancel any still-running Phase-2 graph builder.
    graphCache->init();
    QThreadPool::globalInstance()->waitForDone(60'000);

    return true;
}

static int benchmarkRepo(const QString& repoPath, const QByteArray& logData, int runs)
{
    auto git = QSharedPointer<GitBase>::create(GitRepoConfig{repoPath});
    if (logData.isEmpty() && git->run("git rev-parse --git-dir").output.trimmed().isEmpty())
    {
        qWarning("  Not a git repository: %s", qPrintable(repoPath));
        return 1;
    }

    qInfo("Repo: %s", qPrintable(repoPath));

    // Warm-up: primes the allocator, OS page cache, and QtConcurrent thread pool.
    qInfo("  Warm-up run...");
    if (!runOnce(git, logData, nullptr))
        return 1;

    std::vector<qint64> timings;
    timings.reserve(static_cast<size_t>(runs));

    for (int i = 0; i < runs; ++i)
    {
        qint64 ms = 0;
        if (!runOnce(git, logData, &ms))
            return 1;
        timings.push_back(ms);
        qInfo("  run %d/%d: %lld ms", i + 1, runs, static_cast<long long>(ms));
    }

    std::sort(timings.begin(), timings.end());

    const auto median = timings[static_cast<size_t>(runs) / 2];
    const auto mean = std::accumulate(timings.begin(), timings.end(), 0LL) / runs;

    qInfo(
        "  Result  min=%lldms  median=%lldms  mean=%lldms  max=%lldms",
        static_cast<long long>(timings.front()),
        static_cast<long long>(median),
        static_cast<long long>(mean),
        static_cast<long long>(timings.back()));

    return 0;
}

int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    const auto args = app.arguments();

    int runs = 5;
    QString logFilePath;
    QStringList repos;

    for (int i = 1; i < args.size(); ++i)
    {
        if (args[i] == u"--runs" && i + 1 < args.size())
            runs = args[++i].toInt();
        else if (args[i] == u"--log-file" && i + 1 < args.size())
            logFilePath = args[++i];
        else
            repos << args[i];
    }

    QByteArray logData;

    if (!logFilePath.isEmpty())
    {
        QFile file(logFilePath);
        if (!file.open(QIODevice::ReadOnly))
        {
            qWarning("  Cannot open log file: %s", qPrintable(logFilePath));
            return 1;
        }
        logData = file.readAll();
        qInfo("Log source: file %s (%lld bytes)", qPrintable(logFilePath), static_cast<long long>(logData.size()));
    }

    if (repos.isEmpty())
    {
        if (!logData.isEmpty())
            repos << ".";
        else
        {
            qInfo("Usage: BenchmarkLoader [--log-file <path>] [--runs N] [<repo-path> ...]");
            qInfo("  --log-file    Pre-recorded git log (replays through parse pipeline, no live I/O).");
            qInfo("                Repo path is optional in this mode (defaults to '.').");
            qInfo("  --runs N      Timed iterations after warm-up (default 5).");
            return 1;
        }
    }

    int result = 0;
    for (const auto& repo : std::as_const(repos))
        result |= benchmarkRepo(repo, logData, runs);

    return result;
}
