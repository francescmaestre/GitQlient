#include <graph/TemporalLoom.h>

#include <algorithm>

namespace Graph
{
    TemporalLoom::TemporalLoom(QObject* parent)
        : QObject(parent)
    {
    }

    void TemporalLoom::init(int reserveCount)
    {
        QWriteLocker locker(&mLock);
        // Bump generation so any running Phase 2 (addTimelineBatch loop) detects
        // the reset and returns without writing into the freshly-cleared multiverse.
        ++mGeneration;
        mWeaver = {};
        mMultiverse.clear();
        mMultiverse.squeeze();
        if (reserveCount > 0)
            mMultiverse.reserve(reserveCount);
    }

    void TemporalLoom::insertTopCommit(const CommitEntry& wipEntry, const CommitEntry& newCommitEntry)
    {
        // Use a fresh local Weaver — only the top 2 entries change.
        // The Weaver state after processing these 2 commits is identical to what it was
        // at position 2 before the insertion, so all existing entries below are still valid.
        Weaver localWeaver;
        auto wipTimeline = localWeaver.createTimeline(wipEntry.sha, wipEntry.parents);
        auto newTimeline = localWeaver.createTimeline(newCommitEntry.sha, newCommitEntry.parents);

        QWriteLocker locker(&mLock);
        mMultiverse[wipEntry.sha] = std::move(wipTimeline);
        mMultiverse[newCommitEntry.sha] = std::move(newTimeline);
    }

    bool TemporalLoom::addTimelineBatch(const QVector<CommitEntry>& batch, quint32 generation)
    {
        QWriteLocker locker(&mLock);
        // Re-check generation under the write lock — init() also holds this lock
        // while resetting state, so by the time we acquire it the generation is stable.
        if (mGeneration.loadRelaxed() != generation)
            return false;

        for (const auto& entry : batch)
            mMultiverse[entry.sha] = mWeaver.createTimeline(entry.sha, entry.parents);

        return true;
    }

    void TemporalLoom::notifyExtended(quint32 generation)
    {
        if (mGeneration.loadAcquire() == generation)
            emit multiverseExtended();
    }

    int TemporalLoom::timelinesCount(const QString& sha) const
    {
        QReadLocker locker(&mLock);
        return mMultiverse.value(sha).count();
    }

    Strand TemporalLoom::getTimelineAt(const QString& sha, int index) const
    {
        QReadLocker locker(&mLock);
        if (const auto& timeline = mMultiverse.value(sha); index >= 0 && index < timeline.count())
            return timeline.at(index);

        return Strand();
    }

    int TemporalLoom::getActiveLaneIndex(const QString& sha) const
    {
        QReadLocker locker(&mLock);
        const auto& timeline = mMultiverse.value(sha);
        const auto it = std::ranges::find_if(timeline, [](const Strand& s) {
            return s.isActive();
        });
        return it == timeline.end() ? -1 : static_cast<int>(std::ranges::distance(timeline.begin(), it));
    }

} // namespace Graph
