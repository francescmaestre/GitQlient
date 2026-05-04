#pragma once

/****************************************************************************************
 ** GitQlient is an application to manage and operate one or several Git repositories. With
 ** GitQlient you will be able to add commits, branches and manage all the options Git provides.
 ** Copyright (C) 2021  Francesc Maestre
 **
 ** LinkedIn: https://www.linkedin.com/in/francescmaestre/
 **
 ** This program is free software; you can redistribute it and/or
 ** modify it under the terms of the GNU Lesser General Public
 ** License as published by the Free Software Foundation; either
 ** version 2 of the License, or (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 ** Lesser General Public License for more details.
 **
 ** You should have received a copy of the GNU Lesser General Public
 ** License along with this library; if not, write to the Free Software
 ** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 ***************************************************************************************/

#include <graph/CommitEntry.h>
#include <graph/Weaver.h>

#include <QHash>
#include <QObject>
#include <QReadWriteLock>
#include <QVector>

#include <QAtomicInteger>

namespace Graph
{
    /**
     * @brief Stores and manages the visual lane layout for every commit in the history graph.
     *
     * Lane data is built in two distinct flows:
     *
     * <b>Initial load</b> (GitRepoLoader):
     *  -# Phase 1 calls init(), which resets state, bumps the generation counter, and
     *     pre-allocates the internal hash for the expected commit count.
     *  -# Phase 2 runs on a background thread, calling addTimelineBatch() in batches of
     *     100 commits. It emits multiverseExtended() every 500 commits so the view can
     *     repaint progressively. Phase 2 aborts when generation() changes, which happens
     *     whenever init() is called again.
     *
     * <b>UI-triggered rebuild</b> (HistoryWidget, GraphContextMenu, CommitChangesWidget):
     *  - createMultiverse() resets state and runs synchronously on the calling thread,
     *    rebuilding all lane data in one pass. The caller then emits cacheUpdated() to
     *    trigger a repaint.
     *
     * All read accessors (timelinesCount(), getTimelineAt(), getActiveLaneIndex()) hold a
     * shared read lock so the UI thread can safely query lane data while Phase 2 is writing.
     */
    class TemporalLoom : public QObject
    {
        Q_OBJECT

    signals:
        /** Emitted by Phase 2 every 500 commits and at completion to trigger incremental repaints. */
        void multiverseExtended();

    public:
        explicit TemporalLoom(QObject* parent = nullptr);

        /**
         * @brief Resets all lane state and prepares for a new load.
         *
         * Bumps the internal generation counter so any running Phase 2 worker detects
         * the reset via addTimelineBatch()'s return value and stops.
         * If @a reserveCount > 0, pre-allocates the internal hash to avoid rehashing
         * during the subsequent Phase 2 run.
         */
        void init(int reserveCount = 0);

        /**
         * @brief Updates lane data for a single new commit prepended at the top of the timeline.
         *
         * Called after insertCommit() adds one linear commit at position 1 (right after WIP).
         * Only the WIP entry and the new commit entry are recomputed — everything below is
         * unchanged because the Weaver state at position 2 is identical before and after the
         * insertion for a plain, non-merge commit. O(1) vs the O(N) full rebuild.
         *
         * @param wipEntry       (ZERO_SHA, [newCommitSha]) — the updated WIP parents.
         * @param newCommitEntry (newCommitSha, [parentSha]) — the new commit to prepend.
         */
        void insertTopCommit(const CommitEntry& wipEntry, const CommitEntry& newCommitEntry);

        /**
         * @brief Inserts one batch of lane data during an async Phase 2 run.
         *
         * @param batch       Pairs of (sha, parents) to compute lanes for.
         * @param generation  Generation snapshot taken before Phase 2 started.
         * @return @c true if the batch was written; @c false if @a generation no longer
         *         matches the current one — the caller should stop.
         */
        bool addTimelineBatch(const QVector<CommitEntry>& batch, quint32 generation);

        /**
         * @brief Emits multiverseExtended() if @a generation still matches the current one.
         *
         * A mismatch means a new load has started; the signal is suppressed to avoid
         * triggering a repaint for stale data.
         */
        void notifyExtended(quint32 generation);

        /** Returns the current generation counter so Phase 2 workers can detect a superseding load. */
        quint32 generation() const { return mGeneration.loadAcquire(); }

        /** Returns the number of lane columns recorded for commit @a sha. */
        int timelinesCount(const QString& sha) const;

        /** Returns the lane cell at @a index for commit @a sha, or a default Strand if out of range. */
        Strand getTimelineAt(const QString& sha, int index) const;

        /**
         * @brief Returns the index of the lane that carries commit @a sha's own node.
         * @return The active lane index, or -1 if no active lane is found.
         */
        int getActiveLaneIndex(const QString& sha) const;

    private:
        /** Incremented by init() to signal any running Phase 2 to abort. */
        QAtomicInteger<quint32> mGeneration{0};
        /**
         * Protects mWeaver and mMultiverse: writers (init, addTimelineBatch) take
         * exclusive lock; readers (timelinesCount, getTimelineAt, getActiveLaneIndex) share it.
         */
        mutable QReadWriteLock mLock;
        Weaver mWeaver;
        QHash<QString, Timeline> mMultiverse;
    };
} // namespace Graph
