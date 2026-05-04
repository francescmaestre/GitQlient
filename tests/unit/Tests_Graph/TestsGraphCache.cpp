#include <graph/Weaver.h>

#include <gtest/gtest.h>

namespace
{
    struct Relationship
    {
        QString sha;
        QStringList parents;
    };
}

class WeaverTest : public ::testing::Test
{
protected:
    QVector<Graph::Timeline> build(const QVector<Relationship>& commits)
    {
        Graph::Weaver weaver;
        QVector<Graph::Timeline> timelines;
        for (const auto& r : commits)
            timelines += weaver.createTimeline(r.sha, r.parents);
        return timelines;
    }
};

TEST_F(WeaverTest, buildTemporalLoom)
{
    /* This creates the following graph (S = Start, E = End):
     * S-O-O---O-O-O----O-O-E
     *   |---O-------O--|
     *       |---------O
     */
    const QVector<Relationship> commits {
        { "0000000000000000000000000000000000000000", { "6a5ce03e6aab8184601a7e68daccb4463772c3b8" } },
        { "6a5ce03e6aab8184601a7e68daccb4463772c3b8", { "cd77dc03082595418d26c2c36b376f66562093e9" } },
        { "cd77dc03082595418d26c2c36b376f66562093e9",
          { "ad86b2e4dc69144eeeb1818906e04980336ef820", "3bb3bea6eaf48249f834fa16c5c4107c9562d5f9" } },
        { "14d71c6ed6db586b19aa260635d678e207cce01c", { "2fb103c1ac7f9ab11922c47b522893ea6efe2b06" } },
        { "3bb3bea6eaf48249f834fa16c5c4107c9562d5f9", { "2fb103c1ac7f9ab11922c47b522893ea6efe2b06" } },
        { "ad86b2e4dc69144eeeb1818906e04980336ef820", { "0af130f65eb19b5d5c9952b9e89b3e30edd03ff8" } },
        { "0af130f65eb19b5d5c9952b9e89b3e30edd03ff8", { "bab2fb76670af2a326ebda7a6ad41105aa1ffef1" } },
        { "bab2fb76670af2a326ebda7a6ad41105aa1ffef1", { "88262659d57fd1dccbf155161af5d20dc31f0f3d" } },
        { "2fb103c1ac7f9ab11922c47b522893ea6efe2b06", { "90b17b31217912bbaf6b67d85016e49cc35ca222" } },
        { "88262659d57fd1dccbf155161af5d20dc31f0f3d", { "90b17b31217912bbaf6b67d85016e49cc35ca222" } },
        { "90b17b31217912bbaf6b67d85016e49cc35ca222", { "57284a144f785a5607ea3c6152b901946dbd1af1" } },
        { "57284a144f785a5607ea3c6152b901946dbd1af1", {} },
    };

    const auto timelines = build(commits);
    ASSERT_EQ(timelines.count(), 12);

    auto i = 0;

    ASSERT_EQ(timelines[i].count(), 1);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Branch));
    ++i;

    ASSERT_EQ(timelines[i].count(), 1);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Active));
    ++i;

    ASSERT_EQ(timelines[i].count(), 2);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::MergeFork, Graph::GlyphSide::Left));
    EXPECT_EQ(timelines[i].at(1).getType(), Graph::StrandGlyph(Graph::GlyphType::Head, Graph::GlyphSide::Right));
    ++i;

    ASSERT_EQ(timelines[i].count(), 3);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(1).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(2).getType(), Graph::StrandGlyph(Graph::GlyphType::Branch));
    ++i;

    ASSERT_EQ(timelines[i].count(), 3);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(1).getType(), Graph::StrandGlyph(Graph::GlyphType::Active));
    EXPECT_EQ(timelines[i].at(2).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    ++i;

    ASSERT_EQ(timelines[i].count(), 3);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Active));
    EXPECT_EQ(timelines[i].at(1).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(2).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    ++i;

    ASSERT_EQ(timelines[i].count(), 3);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Active));
    EXPECT_EQ(timelines[i].at(1).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(2).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    ++i;

    ASSERT_EQ(timelines[i].count(), 3);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Active));
    EXPECT_EQ(timelines[i].at(1).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(2).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    ++i;

    ASSERT_EQ(timelines[i].count(), 3);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(1).getType(), Graph::StrandGlyph(Graph::GlyphType::MergeFork, Graph::GlyphSide::Left));
    EXPECT_EQ(timelines[i].at(2).getType(), Graph::StrandGlyph(Graph::GlyphType::Tail, Graph::GlyphSide::Right));
    ++i;

    ASSERT_EQ(timelines[i].count(), 2);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Active));
    EXPECT_EQ(timelines[i].at(1).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    ++i;

    ASSERT_EQ(timelines[i].count(), 2);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::MergeFork, Graph::GlyphSide::Left));
    EXPECT_EQ(timelines[i].at(1).getType(), Graph::StrandGlyph(Graph::GlyphType::Tail, Graph::GlyphSide::Right));
    ++i;

    ASSERT_EQ(timelines[i].count(), 1);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Initial));
}

TEST_F(WeaverTest, buildTemporalLoom2)
{
    /* This creates the following graph (S = Start, E = End):
     * S-O-O-O-O-O---O-O-O----O-O-E
     *         |---O-------O--|
     *             |---------O
     */
    const QVector<Relationship> commits {
        { "0000000000000000000000000000000000000000", { "6a5ce03e6aab8184601a7e68daccb4463772c3b8" } },
        { "6a5ce03e6aab8184601a7e68daccb4463772c3b8", { "cd77dc03082595418d26c2c36b376f66562093e9" } },
        { "cd77dc03082595418d26c2c36b376f66562093e9",
          { "ad86b2e4dc69144eeeb1818906e04980336ef820", "3bb3bea6eaf48249f834fa16c5c4107c9562d5f9" } },
        { "14d71c6ed6db586b19aa260635d678e207cce01c", { "2fb103c1ac7f9ab11922c47b522893ea6efe2b06" } },
        { "3bb3bea6eaf48249f834fa16c5c4107c9562d5f9", { "2fb103c1ac7f9ab11922c47b522893ea6efe2b06" } },
        { "ad86b2e4dc69144eeeb1818906e04980336ef820", { "0af130f65eb19b5d5c9952b9e89b3e30edd03ff8" } },
        { "0af130f65eb19b5d5c9952b9e89b3e30edd03ff8", { "bab2fb76670af2a326ebda7a6ad41105aa1ffef1" } },
        { "bab2fb76670af2a326ebda7a6ad41105aa1ffef1", { "88262659d57fd1dccbf155161af5d20dc31f0f3d" } },
        { "2fb103c1ac7f9ab11922c47b522893ea6efe2b06", { "90b17b31217912bbaf6b67d85016e49cc35ca222" } },
        { "88262659d57fd1dccbf155161af5d20dc31f0f3d", { "90b17b31217912bbaf6b67d85016e49cc35ca222" } },
        { "90b17b31217912bbaf6b67d85016e49cc35ca222", { "57284a144f785a5607ea3c6152b901946dbd1af1" } },
        { "57284a144f785a5607ea3c6152b901946dbd1af1", { "ae68f4a68e4fa6e84fa6e8f4a6e84fa6e8f4dd49" } },
        { "ae68f4a68e4fa6e84fa6e8f4a6e84fa6e8f4dd49", { "df65b4d6f5b4d6f5b4d7f8b9d1fb23df7b8dfdfb" } },
        { "df65b4d6f5b4d6f5b4d7f8b9d1fb23df7b8dfdfb", { "98746513298746513213213afafa654987faff44" } },
        { "98746513298746513213213afafa654987faff44", {} },
    };

    const auto timelines = build(commits);
    ASSERT_EQ(timelines.count(), 15);

    auto i = 0;

    ASSERT_EQ(timelines[i].count(), 1);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Branch));
    ++i;

    ASSERT_EQ(timelines[i].count(), 1);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Active));
    ++i;

    ASSERT_EQ(timelines[i].count(), 2);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::MergeFork, Graph::GlyphSide::Left));
    EXPECT_EQ(timelines[i].at(1).getType(), Graph::StrandGlyph(Graph::GlyphType::Head, Graph::GlyphSide::Right));
    ++i;

    ASSERT_EQ(timelines[i].count(), 3);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(1).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(2).getType(), Graph::StrandGlyph(Graph::GlyphType::Branch));
    ++i;

    ASSERT_EQ(timelines[i].count(), 3);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(1).getType(), Graph::StrandGlyph(Graph::GlyphType::Active));
    EXPECT_EQ(timelines[i].at(2).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    ++i;

    ASSERT_EQ(timelines[i].count(), 3);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Active));
    EXPECT_EQ(timelines[i].at(1).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(2).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    ++i;

    ASSERT_EQ(timelines[i].count(), 3);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Active));
    EXPECT_EQ(timelines[i].at(1).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(2).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    ++i;

    ASSERT_EQ(timelines[i].count(), 3);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Active));
    EXPECT_EQ(timelines[i].at(1).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(2).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    ++i;

    ASSERT_EQ(timelines[i].count(), 3);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(1).getType(), Graph::StrandGlyph(Graph::GlyphType::MergeFork, Graph::GlyphSide::Left));
    EXPECT_EQ(timelines[i].at(2).getType(), Graph::StrandGlyph(Graph::GlyphType::Tail, Graph::GlyphSide::Right));
    ++i;

    ASSERT_EQ(timelines[i].count(), 2);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Active));
    EXPECT_EQ(timelines[i].at(1).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    ++i;

    ASSERT_EQ(timelines[i].count(), 2);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::MergeFork, Graph::GlyphSide::Left));
    EXPECT_EQ(timelines[i].at(1).getType(), Graph::StrandGlyph(Graph::GlyphType::Tail, Graph::GlyphSide::Right));
    ++i;

    ASSERT_EQ(timelines[i].count(), 1);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Active));
    ++i;

    ASSERT_EQ(timelines[i].count(), 1);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Active));
    ++i;

    ASSERT_EQ(timelines[i].count(), 1);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Active));
    ++i;

    ASSERT_EQ(timelines[i].count(), 1);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Initial));
}

TEST_F(WeaverTest, buildTemporalLoom3)
{
    const QVector<Relationship> commits {
        { "0000000000000000000000000000000000000000", { "3449920531c42369cf3520c5cd01893e44925cd2" } },
        { "3449920531c42369cf3520c5cd01893e44925cd2", { "dea6ee29347158758f62ebef510a1ca9911b5d86" } },
        { "dea6ee29347158758f62ebef510a1ca9911b5d86",
          { "e5c0a0410bd14e395a51c53fd2f5be20770e8c38", "b56f93bafba603a0f2c9980ff4cae9b94086e19d" } },
        { "e5c0a0410bd14e395a51c53fd2f5be20770e8c38", { "931b941418893c57c1344b7a03c4923c2c093a94" } },
        { "7c662658394f2945bdc8d74091045b1cf60fa0dc",
          { "931b941418893c57c1344b7a03c4923c2c093a94", "87cd8736346391d2e2ba1390071d6b98a640f8eb" } },
        { "87cd8736346391d2e2ba1390071d6b98a640f8eb", { "931b941418893c57c1344b7a03c4923c2c093a94" } },
        { "1c4a31f62be75409fcf128c7d8d0efe4e05a8854", { "b56f93bafba603a0f2c9980ff4cae9b94086e19d" } },
        { "b56f93bafba603a0f2c9980ff4cae9b94086e19d", { "922372f005570c245ab7c923b4312695b2d232be" } },
        { "922372f005570c245ab7c923b4312695b2d232be", { "8f6a9bb673e546626218c06fdc0429ce407e17a9" } },
        { "8f6a9bb673e546626218c06fdc0429ce407e17a9", { "39c8ed0c6b066e09eedb3593f21137d5968c5619" } },
        { "39c8ed0c6b066e09eedb3593f21137d5968c5619", { "931b941418893c57c1344b7a03c4923c2c093a94" } },
        { "931b941418893c57c1344b7a03c4923c2c093a94", { "931b941418893c57c1344b7a03c4923c2c093a9a" } },
        { "931b941418893c57c1344b7a03c4923c2c093a9a", {} },
    };

    const auto timelines = build(commits);
    ASSERT_EQ(timelines.count(), 13);

    auto i = 0;

    ASSERT_EQ(timelines[i].count(), 1);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Branch));
    ++i;

    ASSERT_EQ(timelines[i].count(), 1);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Active));
    ++i;

    ASSERT_EQ(timelines[i].count(), 2);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::MergeFork, Graph::GlyphSide::Left));
    EXPECT_EQ(timelines[i].at(1).getType(), Graph::StrandGlyph(Graph::GlyphType::Head, Graph::GlyphSide::Right));
    ++i;

    ASSERT_EQ(timelines[i].count(), 2);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Active));
    EXPECT_EQ(timelines[i].at(1).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    ++i;

    ASSERT_EQ(timelines[i].count(), 4);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(1).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(2).getType(), Graph::StrandGlyph(Graph::GlyphType::MergeFork, Graph::GlyphSide::Left));
    EXPECT_EQ(timelines[i].at(3).getType(), Graph::StrandGlyph(Graph::GlyphType::Head, Graph::GlyphSide::Right));
    ++i;

    ASSERT_EQ(timelines[i].count(), 4);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(1).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(2).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(3).getType(), Graph::StrandGlyph(Graph::GlyphType::Active));
    ++i;

    ASSERT_EQ(timelines[i].count(), 5);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(1).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(2).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(3).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(4).getType(), Graph::StrandGlyph(Graph::GlyphType::Branch));
    ++i;

    ASSERT_EQ(timelines[i].count(), 5);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(1).getType(), Graph::StrandGlyph(Graph::GlyphType::MergeFork, Graph::GlyphSide::Left));
    EXPECT_EQ(timelines[i].at(2).getType(), Graph::StrandGlyph(Graph::GlyphType::Cross));
    EXPECT_EQ(timelines[i].at(3).getType(), Graph::StrandGlyph(Graph::GlyphType::Cross));
    EXPECT_EQ(timelines[i].at(4).getType(), Graph::StrandGlyph(Graph::GlyphType::Tail, Graph::GlyphSide::Right));
    ++i;

    ASSERT_EQ(timelines[i].count(), 4);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(1).getType(), Graph::StrandGlyph(Graph::GlyphType::Active));
    EXPECT_EQ(timelines[i].at(2).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(3).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    ++i;

    ASSERT_EQ(timelines[i].count(), 4);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(1).getType(), Graph::StrandGlyph(Graph::GlyphType::Active));
    EXPECT_EQ(timelines[i].at(2).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(3).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    ++i;

    ASSERT_EQ(timelines[i].count(), 4);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(1).getType(), Graph::StrandGlyph(Graph::GlyphType::Active));
    EXPECT_EQ(timelines[i].at(2).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(3).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    ++i;

    ASSERT_EQ(timelines[i].count(), 4);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::MergeFork, Graph::GlyphSide::Left));
    EXPECT_EQ(timelines[i].at(1).getType(), Graph::StrandGlyph(Graph::GlyphType::Tail));
    EXPECT_EQ(timelines[i].at(2).getType(), Graph::StrandGlyph(Graph::GlyphType::Tail));
    EXPECT_EQ(timelines[i].at(3).getType(), Graph::StrandGlyph(Graph::GlyphType::Tail, Graph::GlyphSide::Right));
    ++i;

    ASSERT_EQ(timelines[i].count(), 1);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Initial));
}

TEST_F(WeaverTest, buildTemporalLoom4)
{
    /* Same topology as buildTemporalLoom3 with different SHAs to verify
     * the algorithm is stateless across Weaver instances. */
    const QVector<Relationship> commits {
        { "0000000000000000000000000000000000000000", { "3449920531c42369cf3520c5cd01893e44925cd2" } },
        { "3449920531c42369cf3520c5cd01893e44925cd2", { "dea6ee29347158758f62ebef510a1ca9911b5d86" } },
        { "dea6ee29347158758f62ebef510a1ca9911b5d86",
          { "e5c0a0410bd14e395a51c53fd2f5be20770e8c38", "b56f93bafba603a0f2c9980ff4cae9b94086e19d" } },
        { "e5c0a0410bd14e395a51c53fd2f5be20770e8c38", { "931b941418893c57c1344b7a03c4923c2c093a94" } },
        { "7c662658394f2945bdc8d74091045b1cf60fa0dc",
          { "931b941418893c57c1344b7a03c4923c2c093a94", "87cd8736346391d2e2ba1390071d6b98a640f8eb" } },
        { "87cd8736346391d2e2ba1390071d6b98a640f8eb", { "931b941418893c57c1344b7a03c4923c2c093a94" } },
        { "1c4a31f62be75409fcf128c7d8d0efe4e05a8854", { "b56f93bafba603a0f2c9980ff4cae9b94086e19d" } },
        { "b56f93bafba603a0f2c9980ff4cae9b94086e19d", { "922372f005570c245ab7c923b4312695b2d232be" } },
        { "922372f005570c245ab7c923b4312695b2d232be", { "8f6a9bb673e546626218c06fdc0429ce407e17a9" } },
        { "8f6a9bb673e546626218c06fdc0429ce407e17a9", { "39c8ed0c6b066e09eedb3593f21137d5968c5619" } },
        { "39c8ed0c6b066e09eedb3593f21137d5968c5619", { "931b941418893c57c1344b7a03c4923c2c093a94" } },
        { "931b941418893c57c1344b7a03c4923c2c093a94", { "931b941418893c57c1344b7a03c4923c2c093a9a" } },
        { "931b941418893c57c1344b7a03c4923c2c093a9a", {} },
    };

    const auto timelines = build(commits);
    ASSERT_EQ(timelines.count(), 13);

    auto i = 0;

    ASSERT_EQ(timelines[i].count(), 1);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Branch));
    ++i;

    ASSERT_EQ(timelines[i].count(), 1);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Active));
    ++i;

    ASSERT_EQ(timelines[i].count(), 2);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::MergeFork, Graph::GlyphSide::Left));
    EXPECT_EQ(timelines[i].at(1).getType(), Graph::StrandGlyph(Graph::GlyphType::Head, Graph::GlyphSide::Right));
    ++i;

    ASSERT_EQ(timelines[i].count(), 2);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Active));
    EXPECT_EQ(timelines[i].at(1).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    ++i;

    ASSERT_EQ(timelines[i].count(), 4);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(1).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(2).getType(), Graph::StrandGlyph(Graph::GlyphType::MergeFork, Graph::GlyphSide::Left));
    EXPECT_EQ(timelines[i].at(3).getType(), Graph::StrandGlyph(Graph::GlyphType::Head, Graph::GlyphSide::Right));
    ++i;

    ASSERT_EQ(timelines[i].count(), 4);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(1).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(2).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(3).getType(), Graph::StrandGlyph(Graph::GlyphType::Active));
    ++i;

    ASSERT_EQ(timelines[i].count(), 5);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(1).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(2).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(3).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(4).getType(), Graph::StrandGlyph(Graph::GlyphType::Branch));
    ++i;

    ASSERT_EQ(timelines[i].count(), 5);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(1).getType(), Graph::StrandGlyph(Graph::GlyphType::MergeFork, Graph::GlyphSide::Left));
    EXPECT_EQ(timelines[i].at(2).getType(), Graph::StrandGlyph(Graph::GlyphType::Cross));
    EXPECT_EQ(timelines[i].at(3).getType(), Graph::StrandGlyph(Graph::GlyphType::Cross));
    EXPECT_EQ(timelines[i].at(4).getType(), Graph::StrandGlyph(Graph::GlyphType::Tail, Graph::GlyphSide::Right));
    ++i;

    ASSERT_EQ(timelines[i].count(), 4);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(1).getType(), Graph::StrandGlyph(Graph::GlyphType::Active));
    EXPECT_EQ(timelines[i].at(2).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(3).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    ++i;

    ASSERT_EQ(timelines[i].count(), 4);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(1).getType(), Graph::StrandGlyph(Graph::GlyphType::Active));
    EXPECT_EQ(timelines[i].at(2).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(3).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    ++i;

    ASSERT_EQ(timelines[i].count(), 4);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(1).getType(), Graph::StrandGlyph(Graph::GlyphType::Active));
    EXPECT_EQ(timelines[i].at(2).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(3).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    ++i;

    ASSERT_EQ(timelines[i].count(), 4);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::MergeFork, Graph::GlyphSide::Left));
    EXPECT_EQ(timelines[i].at(1).getType(), Graph::StrandGlyph(Graph::GlyphType::Tail));
    EXPECT_EQ(timelines[i].at(2).getType(), Graph::StrandGlyph(Graph::GlyphType::Tail));
    EXPECT_EQ(timelines[i].at(3).getType(), Graph::StrandGlyph(Graph::GlyphType::Tail, Graph::GlyphSide::Right));
    ++i;

    ASSERT_EQ(timelines[i].count(), 1);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Initial));
}

TEST_F(WeaverTest, buildTemporalLoomInfiniteColumns)
{
    const QVector<Relationship> commits {
        { "0000000000000000000000000000000000000000", { "3449920531c42369cf3520c5cd01893e44925cd2" } },
        { "3449920531c42369cf3520c5cd01893e44925cd0", { "wololowololowololowololowololowololowolo" } },
        { "3449920531c42369cf3520c5cd01893e44925cd1", { "wololowololowololowololowololowololowolo" } },
        { "3449920531c42369cf3520c5cd01893e44925cd3", { "wololowololowololowololowololowololowolo" } },
        { "3449920531c42369cf3520c5cd01893e44925cd4", { "wololowololowololowololowololowololowolo" } },
        { "3449920531c42369cf3520c5cd01893e44925cd5", { "wololowololowololowololowololowololowolo" } },
        { "3449920531c42369cf3520c5cd01893e44925cd6", { "wololowololowololowololowololowololowolo" } },
        { "3449920531c42369cf3520c5cd01893e44925cd7", { "wololowololowololowololowololowololowolo" } },
        { "3449920531c42369cf3520c5cd01893e44925cd8", { "wololowololowololowololowololowololowolo" } },
        { "3449920531c42369cf3520c5cd01893e44925cd9", { "wololowololowololowololowololowololowolo" } },
        { "3449920531c42369cf3520c5cd01893e44925cd2", { "wololowololowololowololowololowololowolo" } },
        { "wololowololowololowololowololowololowolo", {} },
    };

    const auto timelines = build(commits);
    ASSERT_EQ(timelines.count(), 12);

    auto i = 0;

    ASSERT_EQ(timelines[i].count(), 1);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Branch));
    ++i;

    ASSERT_EQ(timelines[i].count(), 2);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(1).getType(), Graph::StrandGlyph(Graph::GlyphType::Branch));
    ++i;

    ASSERT_EQ(timelines[i].count(), 3);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(1).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(2).getType(), Graph::StrandGlyph(Graph::GlyphType::Branch));
    ++i;

    ASSERT_EQ(timelines[i].count(), 4);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(1).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(2).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(3).getType(), Graph::StrandGlyph(Graph::GlyphType::Branch));
    ++i;

    ASSERT_EQ(timelines[i].count(), 5);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(1).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(2).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(3).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(4).getType(), Graph::StrandGlyph(Graph::GlyphType::Branch));
    ++i;

    ASSERT_EQ(timelines[i].count(), 6);
    for (int j = 0; j < 5; ++j)
        EXPECT_EQ(timelines[i].at(j).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(5).getType(), Graph::StrandGlyph(Graph::GlyphType::Branch));
    ++i;

    ASSERT_EQ(timelines[i].count(), 7);
    for (int j = 0; j < 6; ++j)
        EXPECT_EQ(timelines[i].at(j).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(6).getType(), Graph::StrandGlyph(Graph::GlyphType::Branch));
    ++i;

    ASSERT_EQ(timelines[i].count(), 8);
    for (int j = 0; j < 7; ++j)
        EXPECT_EQ(timelines[i].at(j).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(7).getType(), Graph::StrandGlyph(Graph::GlyphType::Branch));
    ++i;

    ASSERT_EQ(timelines[i].count(), 9);
    for (int j = 0; j < 8; ++j)
        EXPECT_EQ(timelines[i].at(j).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(8).getType(), Graph::StrandGlyph(Graph::GlyphType::Branch));
    ++i;

    ASSERT_EQ(timelines[i].count(), 10);
    for (int j = 0; j < 9; ++j)
        EXPECT_EQ(timelines[i].at(j).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    EXPECT_EQ(timelines[i].at(9).getType(), Graph::StrandGlyph(Graph::GlyphType::Branch));
    ++i;

    ASSERT_EQ(timelines[i].count(), 10);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Active));
    for (int j = 1; j < 10; ++j)
        EXPECT_EQ(timelines[i].at(j).getType(), Graph::StrandGlyph(Graph::GlyphType::Inactive));
    ++i;

    ASSERT_EQ(timelines[i].count(), 10);
    EXPECT_EQ(timelines[i].at(0).getType(), Graph::StrandGlyph(Graph::GlyphType::Initial));
    for (int j = 1; j < 9; ++j)
        EXPECT_EQ(timelines[i].at(j).getType(), Graph::StrandGlyph(Graph::GlyphType::Tail));
    EXPECT_EQ(timelines[i].at(9).getType(), Graph::StrandGlyph(Graph::GlyphType::Tail, Graph::GlyphSide::Right));
}
