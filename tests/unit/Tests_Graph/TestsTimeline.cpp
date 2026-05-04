#include <graph/Timeline.h>

#include <gtest/gtest.h>

using namespace Graph;

// ---------------------------------------------------------------------------
// findEmpty — sorted empty-slot invariant
// ---------------------------------------------------------------------------

TEST(TimelineTest, findEmptyOnEmptyTimelineReturnsMinusOne)
{
    Timeline t;
    EXPECT_EQ(t.findEmpty(0), -1);
}

TEST(TimelineTest, findEmptyRespectsPositionLowerBound)
{
    Timeline t;
    t.append(StrandGlyph(GlyphType::Empty)); // slot 0
    t.append(StrandGlyph(GlyphType::Active)); // slot 1
    t.append(StrandGlyph(GlyphType::Empty)); // slot 2

    EXPECT_EQ(t.findEmpty(0), 0);
    EXPECT_EQ(t.findEmpty(1), 2);
    EXPECT_EQ(t.findEmpty(2), 2);
    EXPECT_EQ(t.findEmpty(3), -1);
}

// ---------------------------------------------------------------------------
// setType — empty-slot list is kept consistent
// ---------------------------------------------------------------------------

TEST(TimelineTest, setTypeToEmptyAddsToEmptySlots)
{
    Timeline t;
    t.append(StrandGlyph(GlyphType::Active)); // slot 0
    t.append(StrandGlyph(GlyphType::Active)); // slot 1

    EXPECT_EQ(t.findEmpty(0), -1);

    t.setType(1, StrandGlyph(GlyphType::Empty));

    EXPECT_EQ(t.findEmpty(0), 1);
}

TEST(TimelineTest, setTypeFromEmptyRemovesFromEmptySlots)
{
    Timeline t;
    t.append(StrandGlyph(GlyphType::Empty)); // slot 0 — in empty list

    EXPECT_EQ(t.findEmpty(0), 0);

    t.setType(0, StrandGlyph(GlyphType::Active));

    EXPECT_EQ(t.findEmpty(0), -1);
}

TEST(TimelineTest, setTypeEmptyToEmptyIsIdempotent)
{
    Timeline t;
    t.append(StrandGlyph(GlyphType::Empty)); // slot 0

    t.setType(0, StrandGlyph(GlyphType::Empty));

    EXPECT_EQ(t.findEmpty(0), 0);
    EXPECT_EQ(t.count(), 1);
}

// ---------------------------------------------------------------------------
// removeLast — empty-slot list is kept consistent
// ---------------------------------------------------------------------------

TEST(TimelineTest, removeLastActiveDoesNotAffectEmptySlots)
{
    Timeline t;
    t.append(StrandGlyph(GlyphType::Empty)); // slot 0
    t.append(StrandGlyph(GlyphType::Active)); // slot 1

    t.removeLast();

    EXPECT_EQ(t.count(), 1);
    EXPECT_EQ(t.findEmpty(0), 0);
}

TEST(TimelineTest, removeLastEmptyRemovesFromEmptySlots)
{
    Timeline t;
    t.append(StrandGlyph(GlyphType::Active)); // slot 0
    t.append(StrandGlyph(GlyphType::Empty)); // slot 1

    t.removeLast();

    EXPECT_EQ(t.count(), 1);
    EXPECT_EQ(t.findEmpty(0), -1);
}

// ---------------------------------------------------------------------------
// findType
// ---------------------------------------------------------------------------

TEST(TimelineTest, findTypeReturnsAbsoluteIndexNotRelativeToPos)
{
    Timeline t;
    t.append(StrandGlyph(GlyphType::Active));   // 0
    t.append(StrandGlyph(GlyphType::Inactive)); // 1
    t.append(StrandGlyph(GlyphType::Active));   // 2

    // Search from pos=1 — skips slot 0, finds the Active at slot 2.
    EXPECT_EQ(t.findType(StrandGlyph(GlyphType::Active), 1), 2);
}

TEST(TimelineTest, findTypeReturnsMinusOneWhenNotFound)
{
    Timeline t;
    t.append(StrandGlyph(GlyphType::Active));

    EXPECT_EQ(t.findType(StrandGlyph(GlyphType::Inactive), 0), -1);
}
