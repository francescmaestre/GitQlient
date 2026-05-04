#include <graph/TemporalLoom.h>

#include <support/SignalWaiter.h>

#include <gtest/gtest.h>

using namespace Graph;

namespace
{
    // Used by addTimelineBatch tests — two commits processed in order.
    const QVector<Graph::CommitEntry> kTwoCommits {
        { "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa", {} },
        { "0000000000000000000000000000000000000000", { "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" } },
    };

    // Used by insertTopCommit tests — mimics the real call site after insertCommit():
    //   position 0: WIP whose parent is the newly inserted commit
    //   position 1: the new commit whose parent is the previous HEAD
    const Graph::CommitEntry kWipEntry {
        "0000000000000000000000000000000000000000",
        { "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb" },
    };
    const Graph::CommitEntry kNewCommitEntry {
        "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb",
        { "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" },
    };
} // namespace

// ---------------------------------------------------------------------------
// init — resets state and bumps generation
// ---------------------------------------------------------------------------

TEST(TemporalLoomTest, initClearsPreviousMultiverse)
{
    TemporalLoom loom;
    loom.insertTopCommit(kWipEntry, kNewCommitEntry);

    ASSERT_GT(loom.timelinesCount(kNewCommitEntry.sha), 0);

    loom.init();

    EXPECT_EQ(loom.timelinesCount(kNewCommitEntry.sha), 0);
}

TEST(TemporalLoomTest, initBumpsGeneration)
{
    TemporalLoom loom;
    const auto before = loom.generation();
    loom.init();
    EXPECT_GT(loom.generation(), before);
}

// ---------------------------------------------------------------------------
// insertTopCommit — populates the two affected entries, leaves others intact
// ---------------------------------------------------------------------------

TEST(TemporalLoomTest, insertTopCommitPopulatesTimelineForBothEntries)
{
    TemporalLoom loom;
    loom.insertTopCommit(kWipEntry, kNewCommitEntry);

    EXPECT_GT(loom.timelinesCount(kWipEntry.sha), 0);
    EXPECT_GT(loom.timelinesCount(kNewCommitEntry.sha), 0);
}

TEST(TemporalLoomTest, insertTopCommitReturnsZeroForUnknownSha)
{
    TemporalLoom loom;
    loom.insertTopCommit(kWipEntry, kNewCommitEntry);

    EXPECT_EQ(loom.timelinesCount("cccccccccccccccccccccccccccccccccccccccc"), 0);
}

TEST(TemporalLoomTest, insertTopCommitDoesNotClearExistingEntries)
{
    TemporalLoom loom;
    loom.init();
    loom.addTimelineBatch(kTwoCommits, loom.generation());

    ASSERT_GT(loom.timelinesCount("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"), 0);

    // Insert a new commit on top — existing entries must be preserved.
    loom.insertTopCommit(kWipEntry, kNewCommitEntry);

    EXPECT_GT(loom.timelinesCount("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"), 0);
}

// ---------------------------------------------------------------------------
// addTimelineBatch — generation guard
// ---------------------------------------------------------------------------

TEST(TemporalLoomTest, addTimelineBatchAcceptsBatchWithCurrentGeneration)
{
    TemporalLoom loom;
    loom.init();
    const auto gen = loom.generation();

    const bool accepted = loom.addTimelineBatch(kTwoCommits, gen);

    EXPECT_TRUE(accepted);
    EXPECT_GT(loom.timelinesCount("0000000000000000000000000000000000000000"), 0);
}

TEST(TemporalLoomTest, addTimelineBatchRejectsStaleGeneration)
{
    TemporalLoom loom;
    loom.init();
    const auto staleGen = loom.generation() - 1;

    const bool accepted = loom.addTimelineBatch(kTwoCommits, staleGen);

    EXPECT_FALSE(accepted);
    EXPECT_EQ(loom.timelinesCount("0000000000000000000000000000000000000000"), 0);
}

TEST(TemporalLoomTest, initAfterAddTimelineBatchInvalidatesSubsequentBatch)
{
    TemporalLoom loom;
    loom.init();
    const auto gen = loom.generation();

    loom.init(); // bumps generation, invalidates gen

    EXPECT_FALSE(loom.addTimelineBatch(kTwoCommits, gen));
}

// ---------------------------------------------------------------------------
// getTimelineAt — bounds guarding
// ---------------------------------------------------------------------------

TEST(TemporalLoomTest, getTimelineAtOutOfRangeReturnsDefaultStrand)
{
    TemporalLoom loom;
    loom.insertTopCommit(kWipEntry, kNewCommitEntry);

    const auto sha = kNewCommitEntry.sha;
    const auto count = loom.timelinesCount(sha);
    EXPECT_EQ(loom.getTimelineAt(sha, count), Strand());
    EXPECT_EQ(loom.getTimelineAt(sha, -1), Strand());
}

TEST(TemporalLoomTest, getTimelineAtUnknownShaReturnsDefaultStrand)
{
    TemporalLoom loom;
    EXPECT_EQ(loom.getTimelineAt("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb", 0), Strand());
}

// ---------------------------------------------------------------------------
// getActiveLaneIndex
// ---------------------------------------------------------------------------

TEST(TemporalLoomTest, getActiveLaneIndexReturnsMinusOneForUnknownSha)
{
    TemporalLoom loom;
    EXPECT_EQ(loom.getActiveLaneIndex("bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"), -1);
}

TEST(TemporalLoomTest, getActiveLaneIndexMatchesActiveStrandPosition)
{
    TemporalLoom loom;
    loom.insertTopCommit(kWipEntry, kNewCommitEntry);

    const auto sha = kNewCommitEntry.sha;
    const int idx = loom.getActiveLaneIndex(sha);

    ASSERT_GE(idx, 0);
    EXPECT_TRUE(loom.getTimelineAt(sha, idx).isActive());
}

// ---------------------------------------------------------------------------
// notifyExtended — signal emission
// ---------------------------------------------------------------------------

TEST(TemporalLoomTest, notifyExtendedEmitsSignalForCurrentGeneration)
{
    TemporalLoom loom;
    bool emitted = false;
    QObject::connect(&loom, &TemporalLoom::multiverseExtended, [&emitted] {
        emitted = true;
    });

    loom.notifyExtended(loom.generation());

    EXPECT_TRUE(emitted);
}

TEST(TemporalLoomTest, notifyExtendedSuppressesSignalForStaleGeneration)
{
    TemporalLoom loom;
    bool emitted = false;
    QObject::connect(&loom, &TemporalLoom::multiverseExtended, [&emitted] {
        emitted = true;
    });

    loom.notifyExtended(loom.generation() + 1);

    EXPECT_FALSE(emitted);
}
