#include "BenchmarkCommon.h"

#include <cache/SacredTimeline.h>
#include <graph/TemporalLoom.h>

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QFile>
#include <QProcess>
#include <QTemporaryFile>
#include <QThreadPool>
#include <QTimer>

#include <algorithm>
#include <numeric>
#include <vector>

// ---------------------------------------------------------------------------
// captureGitLog
//
// Runs the same git-log command that GitRepoLoader uses in production and
// returns the raw output as bytes. Called once before the benchmark loop to
// separate I/O time (disk + git process) from parsing time (what we measure).
// ---------------------------------------------------------------------------
static QByteArray captureGitLog(const QString& repoPath)
{
    static const char* FORMAT = "%m%HX%P%n%cn<%ce>%n%an<%ae>%n%at%n%s ";

    QTemporaryFile tmp;
    if (!tmp.open())
    {
        qWarning("captureGitLog: failed to create temp file");
        return {};
    }

    QProcess proc;
    proc.setWorkingDirectory(repoPath);
    proc.setStandardOutputFile(tmp.fileName());
    proc.start(
        "git",
        QStringList {
            "log",
            "--author-date-order",
            "--no-color",
            "--log-size",
            "--parents",
            "--boundary",
            "-z",
            QString("--pretty=format:") + QString::fromUtf8(FORMAT),
            "--all",
        });

    if (!proc.waitForFinished(600'000))
    {
        qWarning("captureGitLog: git log timed out");
        return {};
    }

    tmp.seek(0);
    return tmp.readAll();
}

// ---------------------------------------------------------------------------
// runOnce — one benchmark iteration
// ---------------------------------------------------------------------------
static bool runOnce(const QSharedPointer<GitBase>& git, const QByteArray& logData, qint64* outMs)
{
    auto cache = QSharedPointer<SacredTimeline>::create();
    auto graphCache = QSharedPointer<Graph::TemporalLoom>::create();
    BenchmarkableLoader loader(git, cache, graphCache, logData);

    bool finished = false;
    QEventLoop loop;
    QObject::connect(&loader, &GitRepoLoader::loadingFinished, [&finished, &loop] {
        finished = true;
        loop.quit();
    });

    QElapsedTimer timer;
    timer.start();

    // loadLogHistory() exercises: resolveWorkingDirectory → fetchCommitLog
    // → FakeLogRequestor::run() → onCommitLogReceived (parse + processCommits
    // + graph init) → onLoadStepComplete → loadingFinished.
    loader.loadLogHistory();

    QTimer::singleShot(600'000, &loop, &QEventLoop::quit);
    loop.exec();

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

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    QCoreApplication app(argc, argv);

    const auto args = app.arguments();

    int runs = 5;
    QString repoPath;
    QString logFilePath;

    for (int i = 1; i < args.size(); ++i)
    {
        if (args[i] == u"--runs" && i + 1 < args.size())
            runs = args[++i].toInt();
        else if (args[i] == u"--log-file" && i + 1 < args.size())
            logFilePath = args[++i];
        else
            repoPath = args[i];
    }

    if (repoPath.isEmpty())
    {
        qInfo("Usage: BenchmarkParseLog <repo-path> [--log-file <path>] [--runs N]");
        qInfo("  <repo-path>     Git repository used for WIP/config context queries.");
        qInfo("  --log-file      Pre-recorded git log file (binary, -z delimited).");
        qInfo("                  If omitted, git log is captured from <repo-path>.");
        qInfo("  --runs N        Number of timed iterations after the warm-up (default 5).");
        return 1;
    }

    auto git = QSharedPointer<GitBase>::create(GitRepoConfig { repoPath });
    if (git->run("git rev-parse --git-dir").output.trimmed().isEmpty())
    {
        qWarning("  Not a git repository: %s", qPrintable(repoPath));
        return 1;
    }

    // Load the raw log data: from file or by capturing it live.
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
    else
    {
        qInfo("Capturing git log from %s ...", qPrintable(repoPath));
        logData = captureGitLog(repoPath);
        if (logData.isEmpty())
        {
            qWarning("  Failed to capture git log");
            return 1;
        }
        qInfo("  Captured %lld bytes", static_cast<long long>(logData.size()));
    }

    // Warm-up: primes the allocator, OS page cache, and QtConcurrent thread pool.
    qInfo("Warm-up run...");
    if (!runOnce(git, logData, nullptr))
        return 1;

    // Timed runs.
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
        "Result  min=%lldms  median=%lldms  mean=%lldms  max=%lldms",
        static_cast<long long>(timings.front()),
        static_cast<long long>(median),
        static_cast<long long>(mean),
        static_cast<long long>(timings.back()));

    return 0;
}

#include "BenchmarkParseLog.moc"
