#include <graph/Strand.h>

#include <gtest/gtest.h>

using namespace Graph;

TEST(StrandGlyphTest, equalityChecksBothTypeAndSide)
{
    EXPECT_EQ(StrandGlyph(GlyphType::MergeFork, GlyphSide::Left), StrandGlyph(GlyphType::MergeFork, GlyphSide::Left));
    EXPECT_NE(StrandGlyph(GlyphType::MergeFork, GlyphSide::Left), StrandGlyph(GlyphType::MergeFork, GlyphSide::Right));
    EXPECT_NE(StrandGlyph(GlyphType::Head, GlyphSide::Left), StrandGlyph(GlyphType::Tail, GlyphSide::Left));
}

TEST(StrandTest, isFreeLaneCoversExactlyInactiveCrossAndJoin)
{
    EXPECT_TRUE(Strand(GlyphType::Inactive).isFreeLane());
    EXPECT_TRUE(Strand(GlyphType::Cross).isFreeLane());
    EXPECT_TRUE(Strand(GlyphType::Join).isFreeLane());

    EXPECT_FALSE(Strand(GlyphType::Active).isFreeLane());
    EXPECT_FALSE(Strand(GlyphType::MergeFork).isFreeLane());
    EXPECT_FALSE(Strand(GlyphType::Head).isFreeLane());
    EXPECT_FALSE(Strand(GlyphType::Tail).isFreeLane());
    EXPECT_FALSE(Strand(GlyphType::Empty).isFreeLane());
    EXPECT_FALSE(Strand(GlyphType::Branch).isFreeLane());
    EXPECT_FALSE(Strand(GlyphType::Initial).isFreeLane());
}

TEST(StrandTest, isActiveCoversExactlyActiveBranchInitialAndMergeFork)
{
    EXPECT_TRUE(Strand(GlyphType::Active).isActive());
    EXPECT_TRUE(Strand(GlyphType::Branch).isActive());
    EXPECT_TRUE(Strand(GlyphType::Initial).isActive());
    EXPECT_TRUE(Strand(GlyphType::MergeFork).isActive());

    EXPECT_FALSE(Strand(GlyphType::Inactive).isActive());
    EXPECT_FALSE(Strand(GlyphType::Head).isActive());
    EXPECT_FALSE(Strand(GlyphType::Tail).isActive());
    EXPECT_FALSE(Strand(GlyphType::Join).isActive());
    EXPECT_FALSE(Strand(GlyphType::Cross).isActive());
    EXPECT_FALSE(Strand(GlyphType::Empty).isActive());
}

TEST(StrandTest, setSidePreservesTypeAndUpdatesSide)
{
    Strand s(GlyphType::MergeFork, GlyphSide::Center);
    s.setSide(GlyphSide::Left);
    EXPECT_EQ(s.getType(), StrandGlyph(GlyphType::MergeFork, GlyphSide::Left));
}
