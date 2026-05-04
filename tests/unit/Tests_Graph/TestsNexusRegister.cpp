#include <graph/NexusRegister.h>

#include <gtest/gtest.h>

// ---------------------------------------------------------------------------
// findNextSha — position filtering
// ---------------------------------------------------------------------------

TEST(NexusRegisterTest, findNextShaReturnsMinusOneWhenShaAbsent)
{
    NexusRegister reg;
    reg.append("aaa");
    EXPECT_EQ(reg.findNextSha("zzz", 0), -1);
}

TEST(NexusRegisterTest, findNextShaRespectsPositionFilter)
{
    NexusRegister reg;
    reg.append("aaa"); // lane 0
    reg.append("bbb"); // lane 1
    reg.append("aaa"); // lane 2 — same sha as lane 0, added via setNextSha below

    // Overwrite lane 2 with "aaa"
    reg.setNextSha(2, "aaa");

    EXPECT_EQ(reg.findNextSha("aaa", 0), 0);
    EXPECT_EQ(reg.findNextSha("aaa", 1), 2);
    EXPECT_EQ(reg.findNextSha("aaa", 3), -1);
}

// ---------------------------------------------------------------------------
// setNextSha — both data structures stay consistent
// ---------------------------------------------------------------------------

TEST(NexusRegisterTest, setNextShaReplacesOldShaAndRegistersNew)
{
    NexusRegister reg;
    reg.append("aaa"); // lane 0

    reg.setNextSha(0, "bbb");

    EXPECT_EQ(reg.findNextSha("aaa", 0), -1); // old sha gone
    EXPECT_EQ(reg.findNextSha("bbb", 0), 0);  // new sha registered
}

TEST(NexusRegisterTest, setNextShaToEmptyStringRemovesShaFromIndex)
{
    NexusRegister reg;
    reg.append("aaa"); // lane 0

    reg.setNextSha(0, QString());

    EXPECT_EQ(reg.findNextSha("aaa", 0), -1);
}

TEST(NexusRegisterTest, setNextShaOnLastLaneOfShaErasesKeyFromIndex)
{
    NexusRegister reg;
    reg.append("aaa"); // lane 0 — only lane for "aaa"

    reg.setNextSha(0, "bbb");

    // "aaa" should be gone from the index entirely; searching any pos returns -1.
    EXPECT_EQ(reg.findNextSha("aaa", 0), -1);
}

// ---------------------------------------------------------------------------
// removeLast — consistency when last lane carries a sha
// ---------------------------------------------------------------------------

TEST(NexusRegisterTest, removeLastWithShaUpdatesIndex)
{
    NexusRegister reg;
    reg.append("aaa"); // lane 0
    reg.append("bbb"); // lane 1

    reg.removeLast(); // removes lane 1 / "bbb"

    EXPECT_EQ(reg.count(), 1);
    EXPECT_EQ(reg.findNextSha("bbb", 0), -1);
    EXPECT_EQ(reg.findNextSha("aaa", 0), 0); // lane 0 unaffected
}

TEST(NexusRegisterTest, removeLastWithEmptyShaLeavesIndexUnchanged)
{
    NexusRegister reg;
    reg.append("aaa"); // lane 0
    reg.append(QString()); // lane 1 — no sha

    reg.removeLast();

    EXPECT_EQ(reg.count(), 1);
    EXPECT_EQ(reg.findNextSha("aaa", 0), 0);
}

// ---------------------------------------------------------------------------
// clear
// ---------------------------------------------------------------------------

TEST(NexusRegisterTest, clearResetsCountAndIndex)
{
    NexusRegister reg;
    reg.append("aaa");
    reg.append("bbb");

    reg.clear();

    EXPECT_EQ(reg.count(), 0);
    EXPECT_EQ(reg.findNextSha("aaa", 0), -1);
}
