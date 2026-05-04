#include <system/GitRepoLoader.h>

#include <GitBase.h>
#include <GitBranches.h>
#include <GitConfig.h>
#include <GitLocal.h>
#include <GitRequestorProcess.h>
#include <GitTags.h>
#include <GitWip.h>
#include <QLogger>
#include <cache/SacredTimeline.h>
#include <graph/TemporalLoom.h>
#include <system/SettingsKeys.h>

#include <QDir>
#include <QFutureWatcher>
#include <QLocale>
#include <QSettings>
#include <QThread>
#include <QtConcurrent>

#include <algorithm>
#include <atomic>
#include <cstring>

using namespace QLogger;
using namespace System;

static const char* GIT_LOG_FORMAT("%m%HX%P%n%cn<%ce>%n%an<%ae>%n%at%n%s ");

GitRepoLoader::GitRepoLoader(
    QSharedPointer<GitBase> gitBase,
    QSharedPointer<SacredTimeline> cache,
    const QSharedPointer<Graph::TemporalLoom>& loom,
    QObject* parent)
    : QObject(parent)
    , mGitBase(gitBase)
    , mCommitCache(std::move(cache))
    , mLoom(loom)
    , mGitTags(new GitTags(mGitBase))
{
    connect(mGitTags.get(), &GitTags::remoteTagsReceived, mCommitCache.get(), &SacredTimeline::updateTags);
}

void GitRepoLoader::cancelAll() { emit cancelPending(QPrivateSignal()); }

AGitProcess* GitRepoLoader::createLogRequestor() { return new GitRequestorProcess(mGitBase->config()); }

void GitRepoLoader::loadLogHistory()
{
    if (mIsLoading)
        QLog_Warning("Git", "Git is currently loading data.");
    else
    {
        if (mGitBase->getWorkingDir().isEmpty())
            QLog_Error("Git", "No working directory set.");
        else
        {
            mRefreshReferences = false;
            mIsLoading = true;

            if (resolveWorkingDirectory())
            {
                mGitBase->updateCurrentBranch();

                QLog_Info("Git", "Requesting references...");

                mPendingSteps = 1;

                fetchCommitLog();
            }
            else
                QLog_Error("Git", "The working directory is not a Git repository.");
        }
    }
}

void GitRepoLoader::loadReferences()
{
    if (mIsLoading)
        QLog_Warning("Git", "Git is currently loading data.");
    else
    {
        if (mGitBase->getWorkingDir().isEmpty())
            QLog_Error("Git", "No working directory set.");
        else
        {
            mRefreshReferences = true;
            mIsLoading = true;

            if (resolveWorkingDirectory())
            {
                mGitBase->updateCurrentBranch();

                QLog_Info("Git", "Requesting references...");

                mPendingSteps = 1;

                fetchReferences();
            }
            else
                QLog_Error("Git", "The working directory is not a Git repository.");
        }
    }
}

void GitRepoLoader::loadAll()
{
    if (mIsLoading)
        QLog_Warning("Git", "Git is currently loading data.");
    else
    {
        if (mGitBase->getWorkingDir().isEmpty())
            QLog_Error("Git", "No working directory set.");
        else
        {
            mRefreshReferences = true;
            mIsLoading = true;

            if (resolveWorkingDirectory())
            {
                mGitBase->updateCurrentBranch();

                QLog_Info("Git", "Requesting revisions and references...");

                mPendingSteps = 2;

                fetchCommitLog();
                fetchReferences();
            }
            else
                QLog_Error("Git", "The working directory is not a Git repository.");
        }
    }
}

bool GitRepoLoader::resolveWorkingDirectory()
{
    QLog_Debug("Git", "Configuring repository directory.");

    const auto ret = mGitBase->run("git rev-parse --show-cdup");

    if (ret.success)
    {
        QDir d(QString("%1/%2").arg(mGitBase->getWorkingDir(), ret.output.trimmed()));
        mGitBase->setWorkingDir(d.absolutePath());

        return true;
    }

    return false;
}

void GitRepoLoader::fetchReferences()
{
    QLog_Debug("Git", "Loading references...");

    mRefsRequestor = new GitRequestorProcess(mGitBase->config());
    connect(mRefsRequestor, &GitRequestorProcess::procDataReady, this, &GitRepoLoader::onReferencesReceived);
    connect(this, &GitRepoLoader::cancelPending, mRefsRequestor, &AGitProcess::onCancel);

    mRefsRequestor->run("git show-ref -d");

    mGitTags->getRemoteTags();
}

void GitRepoLoader::onReferencesReceived(QByteArray rawData)
{
    if (mRefreshReferences)
        mCommitCache->clearReferences();

    QString prevRefSha;

    // Stream line-by-line with indexOf('\n') — avoids materialising all 8,434 reference
    // lines as separate QByteArray objects, same as the parseUnsignedLog optimisation.
    int start = 0;
    int end;

    while ((end = rawData.indexOf('\n', start)) != -1)
    {
        const auto lineLen = end - start;

        if (lineLen > 0)
        {
            // SHA is always the first 40 bytes — pure ASCII, fromLatin1 avoids UTF-8 overhead.
            auto revSha = QString::fromLatin1(rawData.constData() + start, 40);
            // refName starts at byte 41 (after the space separator).
            const auto refName = rawData.mid(start + 41, lineLen - 41);

            if (!refName.startsWith("refs/tags/") || (refName.startsWith("refs/tags/") && refName.endsWith("^{}")))
            {
                References::Type type;
                QString name;

                if (refName.startsWith("refs/tags/"))
                {
                    type = References::Type::LocalTag;
                    // mid(10) without a length — the old code passed reference.length() which
                    // over-counted by 41 (the sha + space prefix that is no longer in refName).
                    name = QString::fromUtf8(refName.mid(10));
                    name.remove("^{}");
                }
                else if (refName.startsWith("refs/heads/"))
                {
                    type = References::Type::LocalBranch;
                    name = QString::fromUtf8(refName.mid(11));
                }
                else if (refName.startsWith("refs/remotes/") && !refName.endsWith("HEAD"))
                {
                    type = References::Type::RemoteBranche;
                    name = QString::fromUtf8(refName.mid(13));
                }
                else
                {
                    start = end + 1;
                    continue;
                }

                mCommitCache->insertReference(revSha, type, name);
            }
            prevRefSha = revSha;
        }
        start = end + 1;
    }

    mCommitCache->reloadCurrentBranchInfo(mGitBase->getCurrentBranch(), mGitBase->getLastCommit().output.trimmed());

    onLoadStepComplete();
}

void GitRepoLoader::fetchCommitLog()
{
    QLog_Debug("Git", "Loading revisions...");

    QSettings settings;
    const auto maxCommits = settings.value(GlobalKey::MaxCommits, 0).toInt();
    const auto commitsToRetrieve = maxCommits != 0 ? QString::fromUtf8("-n %1").arg(maxCommits)
        : mShowAll
        ? QString("--all")
        : mGitBase->getCurrentBranch();

    const auto baseCmd
        = QString("git log --author-date-order --no-color --log-size --parents --boundary -z --pretty=format:%1 %2")
              .arg(QString::fromUtf8(GIT_LOG_FORMAT), commitsToRetrieve);

    if (!mCommitCache->isInitialized())
        emit loadingStarted();

    mLogRequestor = createLogRequestor();
    connect(mLogRequestor, &AGitProcess::procDataReady, this, &GitRepoLoader::onCommitLogReceived);
    connect(this, &GitRepoLoader::cancelPending, mLogRequestor, &AGitProcess::onCancel);

    qDebug() << QDateTime::currentDateTime().toString() << "GitRepoLoader::fetchCommitLog";
    mLogRequestor->run(baseCmd);
}

GitRepoLoader::WipData GitRepoLoader::fetchWipData() const
{
    WipData data;

    QScopedPointer<GitConfig> gitConfig(new GitConfig(mGitBase));
    const auto serverUrl = gitConfig->getServerHost();

    if (serverUrl.contains("github"))
        QLog_Info("Git", "Requesting PR status!");

    const auto ret = gitConfig->getGitValue("log.showSignature");
    data.showSignature = ret.success && ret.output.contains("true");

    QScopedPointer<GitWip> git(new GitWip(mGitBase));
    data.untrackedFiles = git->getUntrackedFiles();

    if (const auto wipOpt = git->getWipInfo())
    {
        data.wipParentSha = wipOpt->first;
        data.wipFiles = wipOpt->second;
    }

    return data;
}

void GitRepoLoader::onCommitLogReceived(QByteArray rawLog)
{
    qDebug() << QDateTime::currentDateTime().toString() << "GitRepoLoader::onCommitLogReceived";

    QLog_Info("Git", "Revisions received!");

    const auto initialized = mCommitCache->isInitialized();

    if (!initialized)
        emit loadingStarted();

    // Parse log and build the commit index on a worker thread for large repos so the
    // UI stays responsive. Small repos (< 8 MB) run synchronously to avoid the
    // thread-pool and cross-thread signal overhead that dominates at that scale.
    auto parseAndIndex = [this](QByteArray rawLog) mutable {
        QLog_Debug("Git", "Processing revisions...");

        const auto wipData = fetchWipData();

        if (!rawLog.isEmpty())
        {
            emit loadingMessage(tr("Parsing commit log..."));
            emit loadingProgress(0, 100);
            const auto onParseProgress = [this](int pct) {
                emit loadingProgress(pct, 100);
            };
            auto commits = wipData.showSignature
                ? parseSignedLog(rawLog, onParseProgress)
                : parseUnsignedLog(rawLog, onParseProgress);

            qDebug() << QDateTime::currentDateTime().toString() << "GitRepoLoader::parseAndIndex";

            qDebug() << tr("Building commit index (%1 commits)...").arg(QLocale().toString(commits.count()));
            emit loadingMessage(tr("Building commit index (%1 commits)...").arg(QLocale().toString(commits.count())));

            mCommitCache->setUntrackedFilesList(wipData.untrackedFiles);
            auto indexedCommits = mCommitCache->processCommits(
                wipData.wipParentSha, wipData.wipFiles, std::move(commits));

            // Reserve hash space up-front so Phase 2 avoids rehashing. Graph lanes
            // are built entirely in Phase 2 so the UI can appear without waiting.
            mLoom->init(static_cast<int>(indexedCommits.size()));
        }
    };

    constexpr qsizetype kAsyncThreshold = 8 * 1024 * 1024; // 8 MB ≈ 40k commits

    if (rawLog.size() < kAsyncThreshold)
    {
        parseAndIndex(std::move(rawLog));
        onCommitsParsed();
    }
    else
    {
        auto* parsingWatcher = new QFutureWatcher<void>(this);
        connect(parsingWatcher, &QFutureWatcher<void>::finished, this, [this, parsingWatcher]() {
            parsingWatcher->deleteLater();
            onCommitsParsed();
        });
        parsingWatcher->setFuture(QtConcurrent::run(std::move(parseAndIndex), std::move(rawLog)));
    }
}

void GitRepoLoader::onCommitsParsed()
{
    qDebug() << QDateTime::currentDateTime().toString() << "GitRepoLoader::onCommitsParsed";
    onLoadStepComplete(); // emits loadingFinished when all steps done, clears mIsLoading

    // Build graph lanes in background so the HistoryView can paint immediately.
    // addTimelineBatch checks the generation counter and exits early if a new
    // load has started (init() bumps it), so this is safe across repo switches.
    const auto totalCommits = mCommitCache->commitCount();

    if (totalCommits > 0)
    {
        const auto generation = mLoom->generation();
        // Capture shared-pointer copies, not `this`, so the graph builder keeps
        // objects alive independently of this GitRepoLoader's lifetime.
        const auto commitCache = mCommitCache;
        const auto loom = mLoom;

        auto graphBuildWatcher = new QFutureWatcher<void>(this);
        connect(graphBuildWatcher, &QFutureWatcher<void>::finished, this, [graphBuildWatcher]() {
            graphBuildWatcher->deleteLater();
        });

        graphBuildWatcher->setFuture(QtConcurrent::run([commitCache, loom, totalCommits, generation]() {
            qDebug() << QDateTime::currentDateTime().toString() << "GitRepoLoader::graphBuildWatcher start";
            constexpr int kBatchSize = 100;
            // Trigger a repaint every kLaneUpdateInterval commits so lanes fill
            // in progressively from the top while the user browses.
            constexpr int kLaneUpdateInterval = 500;
            int nextUpdate = kLaneUpdateInterval;

            for (int i = 0; i < totalCommits; i += kBatchSize)
            {
                const auto batch = commitCache->getCommitBatch(i, kBatchSize);
                if (batch.isEmpty())
                    break;

                if (!loom->addTimelineBatch(batch, generation))
                {
                    qDebug() << QDateTime::currentDateTime().toString() << "GitRepoLoader::graphBuildWatcher finish";
                    return;
                }

                if (i >= nextUpdate)
                {
                    qDebug()
                        << QDateTime::currentDateTime().toString()
                        << QString("GitRepoLoader::graphBuildWatcher {%1}").arg(i);
                    loom->notifyExtended(generation);
                    nextUpdate += kLaneUpdateInterval;
                }
            }

            qDebug() << QDateTime::currentDateTime().toString() << "GitRepoLoader::graphBuildWatcher finish";

            loom->notifyExtended(generation);
        }));
    }
}

// Splits the \0-delimited log buffer into numChunks ranges, each ending right after a '\0'.
static QVector<QPair<qsizetype, qsizetype>> computeChunkBoundaries(const QByteArray& log, int numChunks)
{
    QVector<QPair<qsizetype, qsizetype>> ranges;
    const qsizetype total = log.size();
    const qsizetype targetSize = total / numChunks;
    qsizetype start = 0;

    for (int i = 0; i < numChunks - 1 && start < total; ++i)
    {
        const qsizetype hint = start + targetSize;
        if (hint >= total)
            break;
        const auto nullPos = log.indexOf('\000', hint);
        if (nullPos == -1)
            break;
        ranges.append({start, nullPos + 1});
        start = nullPos + 1;
    }

    if (start < total)
        ranges.append({start, total});

    return ranges;
}

QVector<Commit> GitRepoLoader::parseUnsignedLog(QByteArray& log, const std::function<void(int)>& onProgress) const
{
    QVector<Commit> commits;
    commits.reserve(log.size() / 200 + 1);
    commits.append(Commit{}); // slot 0 reserved for WIP — avoids O(n) prepend in insertWipRevision

    const qsizetype totalBytes = log.size();
    const int numThreads = QThread::idealThreadCount();

    if (numThreads > 1)
    {
        const auto ranges = computeChunkBoundaries(log, numThreads);

        std::atomic<qsizetype> processedBytes{0};
        const auto reportEvery = std::max(qsizetype(1), totalBytes / 50);
        const char* rawData = log.constData();

        QVector<QFuture<QVector<Commit>>> futures;
        futures.reserve(ranges.size());

        for (const auto& [chunkStart, chunkEnd] : ranges)
        {
            futures.append(
                QtConcurrent::run(
                    [rawData, chunkStart, chunkEnd, totalBytes, reportEvery, &processedBytes, &onProgress]()
                        -> QVector<Commit> {
                        QVector<Commit> result;
                        result.reserve((chunkEnd - chunkStart) / 200);

                        const char* pos = rawData + chunkStart;
                        const char* const end = rawData + chunkEnd;

                        while (pos < end)
                        {
                            const char* null_ptr = static_cast<const char*>(
                                std::memchr(pos, '\0', static_cast<size_t>(end - pos)));
                            const char* recEnd = null_ptr ? null_ptr : end;
                            const qsizetype recLen = recEnd - pos;

                            if (auto commit = Commit(QByteArray::fromRawData(pos, recLen)); commit.isValid())
                                result.append(std::move(commit));

                            if (onProgress)
                            {
                                const qsizetype delta = recLen + (null_ptr ? 1 : 0);
                                const auto prev = processedBytes.fetch_add(delta);
                                if ((prev + delta) / reportEvery > prev / reportEvery)
                                    onProgress(static_cast<int>((prev + delta) * 100 / totalBytes));
                            }

                            pos = null_ptr ? null_ptr + 1 : end;
                        }

                        return result;
                    }));
        }

        // Merge results in log order and assign sequential positions (1-based; slot 0 is WIP).
        int pos = 0;
        for (auto& future : futures)
        {
            for (auto& commit : future.result())
            {
                commit.pos = ++pos;
                commits.append(std::move(commit));
            }
        }
    }
    else
    {
        // Single-core fallback — sequential scan.
        int pos = 0;
        qsizetype start = 0;
        qsizetype end;
        const auto reportEvery = std::max(qsizetype(1), totalBytes / 50);
        auto nextReport = reportEvery;

        while ((end = log.indexOf('\000', start)) != -1)
        {
            if (auto commit = Commit(log.mid(start, end - start)); commit.isValid())
            {
                commit.pos = ++pos;
                commits.append(std::move(commit));
            }
            start = end + 1;

            if (onProgress && start >= nextReport)
            {
                onProgress(totalBytes > 0 ? start * 100 / totalBytes : 0);
                nextReport += reportEvery;
            }
        }

        // git log -z may omit the trailing '\0' on the last record.
        if (start < totalBytes)
        {
            if (auto commit = Commit(log.mid(start)); commit.isValid())
            {
                commit.pos = ++pos;
                commits.append(std::move(commit));
            }
        }
    }

    if (onProgress)
        onProgress(100);

    return commits;
}

QVector<Commit> GitRepoLoader::parseSignedLog(QByteArray& log, const std::function<void(int)>& onProgress) const
{
    log.replace('\000', '\n');

    QVector<Commit> commits;
    commits.append(Commit{}); // slot 0 reserved for WIP — avoids O(n) prepend in insertWipRevision

    QByteArray commit;
    QByteArray gpg;
    QString gpgKey;
    auto processingCommit = false;
    auto pos = 1;
    qsizetype start = 0;
    qsizetype end;
    bool goodSignature = false;
    const auto totalBytes = log.size();
    const auto reportEvery = std::max(qsizetype(1), totalBytes / 50);
    auto nextReport = reportEvery;

    while ((end = log.indexOf('\n', start)) != -1)
    {
        QByteArray line(log.mid(start, end - start));
        start = end + 1;

        if (line.startsWith("gpg: "))
        {
            processingCommit = false;
            gpg.append(line);

            if (line.contains("using RSA key"))
            {
                gpgKey = QString::fromUtf8(line).split("using RSA key", Qt::SkipEmptyParts).last();
                gpgKey.append('\n');
            }
        }
        else if (line.startsWith("log size"))
        {
            if (!commit.isEmpty())
            {
                if (auto revision = Commit{commit, gpgKey, goodSignature}; revision.isValid())
                {
                    revision.pos = pos++;
                    commits.append(std::move(revision));

                    gpgKey.clear();
                }

                commit.clear();
            }
            processingCommit = true;

            if (!gpg.isEmpty())
            {
                goodSignature = gpg.contains("Good signature");
                gpg.clear();
            }
            else
                goodSignature = false;
        }
        else if (processingCommit)
            commit.append(line + '\n');

        if (onProgress && start >= nextReport)
        {
            onProgress(totalBytes > 0 ? start * 100 / totalBytes : 0);
            nextReport += reportEvery;
        }
    }

    if (onProgress)
        onProgress(100);

    return commits;
}

void GitRepoLoader::onLoadStepComplete()
{
    --mPendingSteps;

    if (mPendingSteps.load() == 0)
    {
        mCommitCache->setConfigurationDone();

        emit loadingFinished(mRefreshReferences);

        mIsLoading = false;
        mRefreshReferences = false;
    }
}
