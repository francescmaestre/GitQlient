#include <cache/SacredTimeline.h>

#include <GitBase.h>
#include <GitExecResult.h>
#include <GitWip.h>
#include <QLogger>

#include <algorithm>
#include <ranges>

using namespace QLogger;

SacredTimeline::SacredTimeline(QObject* parent)
    : QObject(parent)
{
}

SacredTimeline::~SacredTimeline() { clearInternalData(); }

std::span<Commit>
SacredTimeline::processCommits(const QString& parentSha, const RevisionFiles& files, QVector<Commit> commits)
{
    QMutexLocker lock(&mCommitsMutex);

    mCommitsCache = std::move(commits);

    mInitialized = true;
    mConfigured = false;

    mCommitsMap.clear();
    mCommitsMap.squeeze();
    mUntrackedFiles.clear();
    mUntrackedFiles.squeeze();

    QLog_Debug("Cache", QString("Adding WIP revision."));

    insertWipRevision(parentSha, files);

    const auto totalCommits = mCommitsCache.count();

    QLog_Debug("Cache", QString("Configuring the cache for {%1} elements.").arg(totalCommits));

    mCommitsMap.reserve(totalCommits);

    QLog_Debug("Cache", QString("Adding committed revisions."));

    // Pass 1: build SHA → pointer index for all real commits.
    // WIP (index 0) is already in mCommitsMap via insertWipRevision.
    for (auto& commit : mCommitsCache | std::views::drop(1))
        mCommitsMap[commit.sha] = &commit;

    // Pass 2: wire parent → child links using the complete index.
    // Iterating all commits (including WIP at index 0) means the WIP's
    // parent link is established here without any special-case logic.
    for (auto& commit : mCommitsCache)
        for (const auto& pSha : std::as_const(commit.mParentsSha))
            if (auto* parent = mCommitsMap.value(pSha, nullptr))
                parent->appendChild(&commit);

    return std::span<Commit>(mCommitsCache.data(), mCommitsCache.size());
}

Commit SacredTimeline::commitInfo(int row)
{
    QMutexLocker lock(&mCommitsMutex);

    return row >= 0 && row < mCommitsCache.count() ? mCommitsCache.at(row) : Commit();
}

auto SacredTimeline::searchCommit(const QString& text, const int startingPoint) const
{
    return std::ranges::find_if(
        mCommitsCache.constBegin() + startingPoint, mCommitsCache.constEnd(), [&text](const Commit& info) {
            return info.contains(text);
        });
}

auto SacredTimeline::reverseSearchCommit(const QString& text, int startingPoint) const
{
    const auto startEndPos = startingPoint > 0 ? mCommitsCache.count() - startingPoint + 1 : 0;

    return std::ranges::find_if(
        mCommitsCache.crbegin() + startEndPos, mCommitsCache.crend(), [&text](const Commit& info) {
            return info.contains(text);
        });
}

Commit SacredTimeline::searchCommitInfo(const QString& text, int startingPoint, bool reverse)
{
    QMutexLocker lock(&mCommitsMutex);
    Commit commit;

    if (!reverse)
    {
        auto commitIter = searchCommit(text, startingPoint);

        if (commitIter == mCommitsCache.constEnd())
            commitIter = searchCommit(text);

        if (commitIter != mCommitsCache.constEnd())
            commit = *commitIter;
    }
    else
    {
        auto commitIter = reverseSearchCommit(text, startingPoint);

        if (commitIter == mCommitsCache.crend())
            commitIter = reverseSearchCommit(text);

        if (commitIter != mCommitsCache.crend())
            commit = *commitIter;
    }

    return commit;
}

Commit SacredTimeline::commitInfo(const QString& sha)
{
    QMutexLocker lock(&mCommitsMutex);

    if (!sha.isEmpty())
    {
        const auto c = mCommitsMap.value(sha, nullptr);

        if (!c)
        {
            const auto range = std::as_const(mCommitsMap).asKeyValueRange();
            const auto it = std::ranges::find_if(range, [&sha](const auto& pair) {
                return pair.first.startsWith(sha);
            });

            if (it != range.end())
                return *it->second;

            return Commit();
        }

        return *c;
    }

    return Commit();
}

std::optional<RevisionFiles> SacredTimeline::revisionFile(const QString& sha1, const QString& sha2) const
{
    QMutexLocker lock(&mRevisionsMutex);

    const auto iter = mRevisionFilesMap.constFind(qMakePair(sha1, sha2));

    if (iter != mRevisionFilesMap.cend())
        return *iter;

    return std::nullopt;
}

void SacredTimeline::clearReferences()
{
    QMutexLocker lock(&mReferencesMutex);
    mReferences.clear();
    mReferences.squeeze();
}

void SacredTimeline::insertWipRevision(const QString parentSha, const RevisionFiles& files)
{
    auto newParentSha = parentSha;

    QLog_Debug("Cache", QString("Updating the WIP commit. The actual parent has SHA {%1}.").arg(newParentSha));

    insertRevisionFile(ZERO_SHA, newParentSha, files);

    QStringList parents;

    if (!newParentSha.isEmpty())
        parents.append(newParentSha);

    const auto log = files.count() == mUntrackedFiles.count() ? tr("No local changes") : tr("Local changes");
    Commit c(ZERO_SHA, parents, std::chrono::seconds(QDateTime::currentSecsSinceEpoch()), log);

    // Slot 0 is pre-reserved by parseUnsignedLog/parseSignedLog with an empty sha.
    // Both the placeholder (empty sha) and an existing WIP (ZERO_SHA) should be overwritten in place.
    if (!mCommitsCache[0].sha.isEmpty() && mCommitsCache[0].sha != ZERO_SHA)
        mCommitsCache.prepend(std::move(c));
    else
        mCommitsCache[0] = std::move(c);

    mCommitsMap.insert(ZERO_SHA, &mCommitsCache[0]);
}

bool SacredTimeline::insertRevisionFiles(const QString& sha1, const QString& sha2, const RevisionFiles& file)
{
    QMutexLocker lock(&mRevisionsMutex);

    return insertRevisionFile(sha1, sha2, file);
}

bool SacredTimeline::insertRevisionFile(const QString& sha1, const QString& sha2, const RevisionFiles& file)
{
    const auto key = qMakePair(sha1, sha2);
    const auto emptyShas = !sha1.isEmpty() && !sha2.isEmpty();
    const auto isWip = sha1 == ZERO_SHA;

    if (emptyShas || isWip)
    {
        const auto it = mRevisionFilesMap.constFind(key);
        if (it == mRevisionFilesMap.cend() || *it != file)
        {
            QLog_Debug("Cache", QString("Adding the revisions files between {%1} and {%2}.").arg(sha1, sha2));
            mRevisionFilesMap.insert(key, file);
            return true;
        }
    }

    return false;
}

void SacredTimeline::insertReference(const QString& sha, References::Type type, const QString& reference)
{
    QMutexLocker lock(&mReferencesMutex);

    QLog_Trace("Cache", QString("Adding a new reference with SHA {%1}.").arg(sha));

    mReferences[sha].addReference(type, reference);
}

void SacredTimeline::deleteReference(const QString& sha, References::Type type, const QString& reference)
{
    QMutexLocker lock(&mReferencesMutex);

    mReferences[sha].removeReference(type, reference);
}

bool SacredTimeline::hasReferences(const QString& sha)
{
    QMutexLocker lock(&mReferencesMutex);

    return mReferences.contains(sha) && !mReferences.value(sha).isEmpty();
}

QStringList SacredTimeline::getReferences(const QString& sha, References::Type type)
{
    QMutexLocker lock(&mReferencesMutex);

    return mReferences.value(sha).getReferences(type);
}

QString SacredTimeline::getShaOfReference(const QString& referenceName, References::Type type) const
{
    QMutexLocker lock(&mReferencesMutex);

    for (const auto& [sha, refs] : mReferences.asKeyValueRange())
    {
        if (const auto list = refs.findReferences(type))
        {
            for (const auto& reference : list->get())
            {
                if (reference == referenceName)
                    return sha;
            }
        }
    }

    return QString();
}

void SacredTimeline::reloadCurrentBranchInfo(const QString& currentBranch, const QString& currentSha)
{
    QMutexLocker lock(&mReferencesMutex);

    const auto range = std::as_const(mReferences).asKeyValueRange();
    const auto it = std::ranges::find_if(range, [&currentBranch](const auto& pair) {
        return pair.second.getReferences(References::Type::LocalBranch).contains(currentBranch);
    });

    if (it != range.end())
    {
        const auto sha = it->first;
        mReferences[sha].removeReference(References::Type::LocalBranch, currentBranch);
        if (mReferences.value(sha).isEmpty())
            mReferences.remove(sha);
    }

    if (!currentBranch.isEmpty())
        mReferences[currentSha].addReference(References::Type::LocalBranch, currentBranch);
}

bool SacredTimeline::updateWipCommit(const QString& parentSha, const RevisionFiles& files)
{
    QMutexLocker lock(&mRevisionsMutex);
    QMutexLocker lock2(&mCommitsMutex);

    if (mConfigured)
    {
        insertWipRevision(parentSha, files);
        return true;
    }

    return false;
}

void SacredTimeline::insertCommit(Commit commit)
{
    QMutexLocker lock2(&mCommitsMutex);

    const auto sha = commit.sha;
    const auto parentSha = commit.firstParent();

    mCommitsCache[0].setParents({sha});
    mCommitsCache.insert(1, std::move(commit));
    mCommitsCache[1].appendChild(&mCommitsCache[0]);

    for (int i = 1; i < mCommitsCache.count(); ++i)
    {
        mCommitsCache[i].pos = i;
        mCommitsMap[mCommitsCache[i].sha] = &mCommitsCache[i];
    }

    mCommitsMap[parentSha]->removeChild(mCommitsMap[ZERO_SHA]);
    mCommitsMap[parentSha]->appendChild(mCommitsMap[sha]);
}

void SacredTimeline::updateCommitInfo(const Commit& info)
{
    QMutexLocker lock(&mCommitsMutex);

    if (const auto it = mCommitsMap.find(info.sha); it != mCommitsMap.end())
        **it = info;
}

void SacredTimeline::updateCommit(const QString& oldSha, Commit newCommit)
{
    QMutexLocker lock(&mCommitsMutex);
    QMutexLocker lock2(&mRevisionsMutex);

    auto& oldCommit = mCommitsMap[oldSha];
    const auto oldCommitParens = oldCommit->parents();
    const auto newCommitSha = newCommit.sha;
    const auto newPos = newCommit.pos;

    mCommitsCache[newPos] = std::move(newCommit);
    mCommitsMap.remove(oldSha);
    mCommitsMap.insert(newCommitSha, &mCommitsCache[newPos]);

    for (const auto& parent : oldCommitParens)
    {
        mCommitsMap[parent]->removeChild(oldCommit);
        mCommitsMap[parent]->appendChild(mCommitsMap[newCommitSha]);
    }

    const auto tags = getReferences(oldSha, References::Type::LocalTag);
    for (const auto& tag : tags)
    {
        insertReference(newCommitSha, References::Type::LocalTag, tag);
        deleteReference(oldSha, References::Type::LocalTag, tag);
    }

    const auto localBranches = getReferences(oldSha, References::Type::LocalBranch);
    for (const auto& branch : localBranches)
    {
        insertReference(newCommitSha, References::Type::LocalBranch, branch);
        deleteReference(oldSha, References::Type::LocalBranch, branch);
    }
}

std::span<Commit> SacredTimeline::getCommits() { return std::span<Commit>(mCommitsCache.data(), mCommitsCache.size()); }

bool SacredTimeline::pendingLocalChanges()
{
    QMutexLocker lock(&mCommitsMutex);

    auto localChanges = false;

    if (const auto commit = mCommitsMap.value(ZERO_SHA, nullptr))
    {
        if (const auto rf = revisionFile(ZERO_SHA, commit->firstParent()); rf)
            localChanges = rf.value().count() - mUntrackedFiles.count() > 0;
    }

    return localChanges;
}

QVector<QPair<QString, QStringList>> SacredTimeline::getBranches(References::Type type)
{
    QMutexLocker lock(&mReferencesMutex);
    QVector<QPair<QString, QStringList>> branches;

    for (const auto& [sha, refs] : mReferences.asKeyValueRange())
        branches.append(QPair<QString, QStringList>(sha, refs.getReferences(type)));

    return branches;
}

QMap<QString, QString> SacredTimeline::getTags(References::Type tagType) const
{
    QMutexLocker lock(&mReferencesMutex);

    QMap<QString, QString> tags;

    for (const auto& [sha, refs] : mReferences.asKeyValueRange())
        for (const auto& tag : refs.getReferences(tagType))
            tags[tag] = sha;

    return tags;
}

void SacredTimeline::updateTags(QMap<QString, QString> remoteTags)
{
    for (const auto& [name, sha] : remoteTags.asKeyValueRange())
        insertReference(sha, References::Type::RemoteTag, name);

    emit cacheUpdated();
}

void SacredTimeline::clearInternalData()
{
    mCommitsMap.clear();
    mCommitsMap.squeeze();
    mRevisionFilesMap.clear();
    mRevisionFilesMap.squeeze();
    mUntrackedFiles.clear();
    mUntrackedFiles.squeeze();
    mReferences.clear();
    mReferences.squeeze();
}

int SacredTimeline::commitCount() const
{
    QMutexLocker lock(&mCommitsMutex);
    return mCommitsCache.count();
}

QVector<Graph::CommitEntry> SacredTimeline::getCommitBatch(int from, int count) const
{
    // Acquire mCommitsMutex once for the entire batch — avoids per-commit lock overhead
    // when Phase 2 of graph computation is fetching 100 commits at a time.
    QMutexLocker lock(&mCommitsMutex);
    QVector<Graph::CommitEntry> batch;
    const auto end = std::min(from + count, static_cast<int>(mCommitsCache.size()));
    batch.reserve(end - from);
    for (auto i = from; i < end; ++i)
        batch.append({ mCommitsCache[i].sha, mCommitsCache[i].parents() });
    return batch;
}

void SacredTimeline::setUntrackedFilesList(QVector<QString> untrackedFiles)
{
    mUntrackedFiles.clear();
    mUntrackedFiles.squeeze();
    mUntrackedFiles = std::move(untrackedFiles);
}

bool SacredTimeline::refreshWip(const QSharedPointer<GitBase>& git)
{
    GitWip wip(git);

    const auto files = wip.getUntrackedFiles();
    setUntrackedFilesList(std::move(files));

    if (const auto info = wip.getWipInfo(); info->second.isValid())
        return updateWipCommit(info->first, info->second);

    return false;
}
