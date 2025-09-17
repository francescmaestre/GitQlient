#include <TestsGraphCache.h>

#include <graph/TemporalLoom.h>

#include <QTest>

void TestsGraphCache::initTestCase()
{
}

void TestsGraphCache::cleanupTestCase()
{
}

void TestsGraphCache::test_buildTemporalLoom()
{
   /* This creates the following graph (S = Start, E = End):
    * S-O-O---O-O-O----O-O-E
    *   |---O-------O--|
    *       |---------O
    */
   QVector<Relationship> vector;
   vector.append(Relationship { "0000000000000000000000000000000000000000", { "6a5ce03e6aab8184601a7e68daccb4463772c3b8" } });
   vector.append(Relationship { "6a5ce03e6aab8184601a7e68daccb4463772c3b8", { "cd77dc03082595418d26c2c36b376f66562093e9" } });
   vector.append(Relationship { "cd77dc03082595418d26c2c36b376f66562093e9", { "ad86b2e4dc69144eeeb1818906e04980336ef820", "3bb3bea6eaf48249f834fa16c5c4107c9562d5f9" } });
   vector.append(Relationship { "14d71c6ed6db586b19aa260635d678e207cce01c", { "2fb103c1ac7f9ab11922c47b522893ea6efe2b06" } });
   vector.append(Relationship { "3bb3bea6eaf48249f834fa16c5c4107c9562d5f9", { "2fb103c1ac7f9ab11922c47b522893ea6efe2b06" } });
   vector.append(Relationship { "ad86b2e4dc69144eeeb1818906e04980336ef820", { "0af130f65eb19b5d5c9952b9e89b3e30edd03ff8" } });
   vector.append(Relationship { "0af130f65eb19b5d5c9952b9e89b3e30edd03ff8", { "bab2fb76670af2a326ebda7a6ad41105aa1ffef1" } });
   vector.append(Relationship { "bab2fb76670af2a326ebda7a6ad41105aa1ffef1", { "88262659d57fd1dccbf155161af5d20dc31f0f3d" } });
   vector.append(Relationship { "2fb103c1ac7f9ab11922c47b522893ea6efe2b06", { "90b17b31217912bbaf6b67d85016e49cc35ca222" } });
   vector.append(Relationship { "88262659d57fd1dccbf155161af5d20dc31f0f3d", { "90b17b31217912bbaf6b67d85016e49cc35ca222" } });
   vector.append(Relationship { "90b17b31217912bbaf6b67d85016e49cc35ca222", { "57284a144f785a5607ea3c6152b901946dbd1af1" } });
   vector.append(Relationship { "57284a144f785a5607ea3c6152b901946dbd1af1", {} });

   Graph::TemporalLoom loom;
   QVector<Graph::Timeline> timelines;

   for (auto &v : vector)
      timelines += loom.createTimeline(v.sha, v.parents);

   QVERIFY(timelines.count() == 12);

   auto i = 0;

   QVERIFY(timelines[i].count() == 1);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Branch);
   ++i;

   QVERIFY(timelines[i].count() == 1);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Active);
   ++i;

   QVERIFY(timelines[i].count() == 2);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::MergeForkLeft);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::HeadRight);
   ++i;

   QVERIFY(timelines[i].count() == 3);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(2).getType(), Graph::StateType::Branch);
   ++i;

   QVERIFY(timelines[i].count() == 3);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::Active);
   QCOMPARE(timelines[i].at(2).getType(), Graph::StateType::Inactive);
   ++i;

   QVERIFY(timelines[i].count() == 3);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Active);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(2).getType(), Graph::StateType::Inactive);
   ++i;

   QVERIFY(timelines[i].count() == 3);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Active);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(2).getType(), Graph::StateType::Inactive);
   ++i;

   QVERIFY(timelines[i].count() == 3);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Active);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(2).getType(), Graph::StateType::Inactive);
   ++i;

   QVERIFY(timelines[i].count() == 3);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::MergeForkLeft);
   QCOMPARE(timelines[i].at(2).getType(), Graph::StateType::TailRight);
   ++i;

   QVERIFY(timelines[i].count() == 2);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Active);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::Inactive);
   ++i;

   QVERIFY(timelines[i].count() == 2);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::MergeForkLeft);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::TailRight);
   ++i;

   QVERIFY(timelines[i].count() == 1);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Initial);
   ++i;
}

void TestsGraphCache::test_buildTemporalLoom2()
{
   /* This creates the following graph (S = Start, E = End):
    * S-O-O-O-O-O---O-O-O----O-O-E
    *         |---O-------O--|
    *             |---------O
    */
   QVector<Relationship> vector;
   vector.append(Relationship { "0000000000000000000000000000000000000000", { "6a5ce03e6aab8184601a7e68daccb4463772c3b8" } });
   vector.append(Relationship { "6a5ce03e6aab8184601a7e68daccb4463772c3b8", { "cd77dc03082595418d26c2c36b376f66562093e9" } });
   vector.append(Relationship { "cd77dc03082595418d26c2c36b376f66562093e9", { "ad86b2e4dc69144eeeb1818906e04980336ef820", "3bb3bea6eaf48249f834fa16c5c4107c9562d5f9" } });
   vector.append(Relationship { "14d71c6ed6db586b19aa260635d678e207cce01c", { "2fb103c1ac7f9ab11922c47b522893ea6efe2b06" } });
   vector.append(Relationship { "3bb3bea6eaf48249f834fa16c5c4107c9562d5f9", { "2fb103c1ac7f9ab11922c47b522893ea6efe2b06" } });
   vector.append(Relationship { "ad86b2e4dc69144eeeb1818906e04980336ef820", { "0af130f65eb19b5d5c9952b9e89b3e30edd03ff8" } });
   vector.append(Relationship { "0af130f65eb19b5d5c9952b9e89b3e30edd03ff8", { "bab2fb76670af2a326ebda7a6ad41105aa1ffef1" } });
   vector.append(Relationship { "bab2fb76670af2a326ebda7a6ad41105aa1ffef1", { "88262659d57fd1dccbf155161af5d20dc31f0f3d" } });
   vector.append(Relationship { "2fb103c1ac7f9ab11922c47b522893ea6efe2b06", { "90b17b31217912bbaf6b67d85016e49cc35ca222" } });
   vector.append(Relationship { "88262659d57fd1dccbf155161af5d20dc31f0f3d", { "90b17b31217912bbaf6b67d85016e49cc35ca222" } });
   vector.append(Relationship { "90b17b31217912bbaf6b67d85016e49cc35ca222", { "57284a144f785a5607ea3c6152b901946dbd1af1" } });
   vector.append(Relationship { "57284a144f785a5607ea3c6152b901946dbd1af1", { "ae68f4a68e4fa6e84fa6e8f4a6e84fa6e8f4dd49" } });
   vector.append(Relationship { "ae68f4a68e4fa6e84fa6e8f4a6e84fa6e8f4dd49", { "df65b4d6f5b4d6f5b4d7f8b9d1fb23df7b8dfdfb" } });
   vector.append(Relationship { "df65b4d6f5b4d6f5b4d7f8b9d1fb23df7b8dfdfb", { "98746513298746513213213afafa654987faff44" } });
   vector.append(Relationship { "98746513298746513213213afafa654987faff44", {} });

   Graph::TemporalLoom loom;
   QVector<Graph::Timeline> timelines;

   for (auto &v : vector)
      timelines += loom.createTimeline(v.sha, v.parents);

   QVERIFY(timelines.count() == 15);

   auto i = 0;

   QVERIFY(timelines[i].count() == 1);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Branch);
   ++i;

   QVERIFY(timelines[i].count() == 1);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Active);
   ++i;

   QVERIFY(timelines[i].count() == 2);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::MergeForkLeft);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::HeadRight);
   ++i;

   QVERIFY(timelines[i].count() == 3);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(2).getType(), Graph::StateType::Branch);
   ++i;

   QVERIFY(timelines[i].count() == 3);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::Active);
   QCOMPARE(timelines[i].at(2).getType(), Graph::StateType::Inactive);
   ++i;

   QVERIFY(timelines[i].count() == 3);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Active);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(2).getType(), Graph::StateType::Inactive);
   ++i;

   QVERIFY(timelines[i].count() == 3);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Active);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(2).getType(), Graph::StateType::Inactive);
   ++i;

   QVERIFY(timelines[i].count() == 3);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Active);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(2).getType(), Graph::StateType::Inactive);
   ++i;

   QVERIFY(timelines[i].count() == 3);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::MergeForkLeft);
   QCOMPARE(timelines[i].at(2).getType(), Graph::StateType::TailRight);
   ++i;

   QVERIFY(timelines[i].count() == 2);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Active);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::Inactive);
   ++i;

   QVERIFY(timelines[i].count() == 2);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::MergeForkLeft);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::TailRight);
   ++i;

   QVERIFY(timelines[i].count() == 1);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Active);
   ++i;

   QVERIFY(timelines[i].count() == 1);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Active);
   ++i;

   QVERIFY(timelines[i].count() == 1);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Active);
   ++i;

   QVERIFY(timelines[i].count() == 1);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Initial);
   ++i;
}

void TestsGraphCache::test_buildTemporalLoom3()
{
   /* This creates the following graph (S = Start, E = End):
    * S-O-O-O-O-O---O-O-O----O-O-E
    *         |---O-------O--|
    *             |---------O
    */
   QVector<Relationship> vector;
   vector.append(Relationship { "0000000000000000000000000000000000000000", { "3449920531c42369cf3520c5cd01893e44925cd2" } });
   vector.append(Relationship { "3449920531c42369cf3520c5cd01893e44925cd2", { "dea6ee29347158758f62ebef510a1ca9911b5d86" } });
   vector.append(Relationship { "dea6ee29347158758f62ebef510a1ca9911b5d86", { "e5c0a0410bd14e395a51c53fd2f5be20770e8c38", "b56f93bafba603a0f2c9980ff4cae9b94086e19d" } });
   vector.append(Relationship { "e5c0a0410bd14e395a51c53fd2f5be20770e8c38", { "931b941418893c57c1344b7a03c4923c2c093a94" } });
   vector.append(Relationship { "7c662658394f2945bdc8d74091045b1cf60fa0dc", { "931b941418893c57c1344b7a03c4923c2c093a94", "87cd8736346391d2e2ba1390071d6b98a640f8eb" } });
   vector.append(Relationship { "87cd8736346391d2e2ba1390071d6b98a640f8eb", { "931b941418893c57c1344b7a03c4923c2c093a94" } });
   vector.append(Relationship { "1c4a31f62be75409fcf128c7d8d0efe4e05a8854", { "b56f93bafba603a0f2c9980ff4cae9b94086e19d" } });
   vector.append(Relationship { "b56f93bafba603a0f2c9980ff4cae9b94086e19d", { "922372f005570c245ab7c923b4312695b2d232be" } });
   vector.append(Relationship { "922372f005570c245ab7c923b4312695b2d232be", { "8f6a9bb673e546626218c06fdc0429ce407e17a9" } });
   vector.append(Relationship { "8f6a9bb673e546626218c06fdc0429ce407e17a9", { "39c8ed0c6b066e09eedb3593f21137d5968c5619" } });
   vector.append(Relationship { "39c8ed0c6b066e09eedb3593f21137d5968c5619", { "931b941418893c57c1344b7a03c4923c2c093a94" } });
   vector.append(Relationship { "931b941418893c57c1344b7a03c4923c2c093a94", { "931b941418893c57c1344b7a03c4923c2c093a9a" } });
   vector.append(Relationship { "931b941418893c57c1344b7a03c4923c2c093a9a", {} });

   Graph::TemporalLoom loom;
   QVector<Graph::Timeline> timelines;

   for (auto &v : vector)
      timelines += loom.createTimeline(v.sha, v.parents);

   QVERIFY(timelines.count() == 13);

   auto i = 0;

   QVERIFY(timelines[i].count() == 1);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Branch);
   ++i;

   QVERIFY(timelines[i].count() == 1);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Active);
   ++i;

   QVERIFY(timelines[i].count() == 2);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::MergeForkLeft);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::HeadRight);
   ++i;

   QVERIFY(timelines[i].count() == 2);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Active);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::Inactive);
   ++i;

   QVERIFY(timelines[i].count() == 4);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(2).getType(), Graph::StateType::MergeForkLeft);
   QCOMPARE(timelines[i].at(3).getType(), Graph::StateType::HeadRight);
   ++i;

   QVERIFY(timelines[i].count() == 4);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(2).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(3).getType(), Graph::StateType::Active);
   ++i;

   QVERIFY(timelines[i].count() == 5);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(2).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(3).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(4).getType(), Graph::StateType::Branch);
   ++i;

   QVERIFY(timelines[i].count() == 5);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::MergeForkLeft);
   QCOMPARE(timelines[i].at(2).getType(), Graph::StateType::Cross);
   QCOMPARE(timelines[i].at(3).getType(), Graph::StateType::Cross);
   QCOMPARE(timelines[i].at(4).getType(), Graph::StateType::TailRight);
   ++i;

   QVERIFY(timelines[i].count() == 4);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::Active);
   QCOMPARE(timelines[i].at(2).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(3).getType(), Graph::StateType::Inactive);
   ++i;

   QVERIFY(timelines[i].count() == 4);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::Active);
   QCOMPARE(timelines[i].at(2).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(3).getType(), Graph::StateType::Inactive);
   ++i;

   QVERIFY(timelines[i].count() == 4);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::Active);
   QCOMPARE(timelines[i].at(2).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(3).getType(), Graph::StateType::Inactive);
   ++i;

   QVERIFY(timelines[i].count() == 4);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::MergeForkLeft);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::Tail);
   QCOMPARE(timelines[i].at(2).getType(), Graph::StateType::Tail);
   QCOMPARE(timelines[i].at(3).getType(), Graph::StateType::TailRight);
   ++i;

   QVERIFY(timelines[i].count() == 1);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Initial);
   ++i;
}

void TestsGraphCache::test_buildTemporalLoom4()
{
   /* This creates the following graph (S = Start, E = End):
    * S-O-O-O-O-O---O-O-O----O-O-E
    *         |---O-------O--|
    *             |---------O
    */
   QVector<Relationship> vector;
   vector.append(Relationship { "0000000000000000000000000000000000000000", { "3449920531c42369cf3520c5cd01893e44925cd2" } });
   vector.append(Relationship { "3449920531c42369cf3520c5cd01893e44925cd2", { "dea6ee29347158758f62ebef510a1ca9911b5d86" } });
   vector.append(Relationship { "dea6ee29347158758f62ebef510a1ca9911b5d86", { "e5c0a0410bd14e395a51c53fd2f5be20770e8c38", "b56f93bafba603a0f2c9980ff4cae9b94086e19d" } });
   vector.append(Relationship { "e5c0a0410bd14e395a51c53fd2f5be20770e8c38", { "931b941418893c57c1344b7a03c4923c2c093a94" } });
   vector.append(Relationship { "7c662658394f2945bdc8d74091045b1cf60fa0dc", { "931b941418893c57c1344b7a03c4923c2c093a94", "87cd8736346391d2e2ba1390071d6b98a640f8eb" } });
   vector.append(Relationship { "87cd8736346391d2e2ba1390071d6b98a640f8eb", { "931b941418893c57c1344b7a03c4923c2c093a94" } });
   vector.append(Relationship { "1c4a31f62be75409fcf128c7d8d0efe4e05a8854", { "b56f93bafba603a0f2c9980ff4cae9b94086e19d" } });
   vector.append(Relationship { "b56f93bafba603a0f2c9980ff4cae9b94086e19d", { "922372f005570c245ab7c923b4312695b2d232be" } });
   vector.append(Relationship { "922372f005570c245ab7c923b4312695b2d232be", { "8f6a9bb673e546626218c06fdc0429ce407e17a9" } });
   vector.append(Relationship { "8f6a9bb673e546626218c06fdc0429ce407e17a9", { "39c8ed0c6b066e09eedb3593f21137d5968c5619" } });
   vector.append(Relationship { "39c8ed0c6b066e09eedb3593f21137d5968c5619", { "931b941418893c57c1344b7a03c4923c2c093a94" } });
   vector.append(Relationship { "931b941418893c57c1344b7a03c4923c2c093a94", { "931b941418893c57c1344b7a03c4923c2c093a9a" } });
   vector.append(Relationship { "931b941418893c57c1344b7a03c4923c2c093a9a", {} });

   Graph::TemporalLoom loom;
   QVector<Graph::Timeline> timelines;

   for (auto &v : vector)
      timelines += loom.createTimeline(v.sha, v.parents);

   QVERIFY(timelines.count() == 13);

   auto i = 0;

   QVERIFY(timelines[i].count() == 1);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Branch);
   ++i;

   QVERIFY(timelines[i].count() == 1);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Active);
   ++i;

   QVERIFY(timelines[i].count() == 2);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::MergeForkLeft);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::HeadRight);
   ++i;

   QVERIFY(timelines[i].count() == 2);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Active);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::Inactive);
   ++i;

   QVERIFY(timelines[i].count() == 4);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(2).getType(), Graph::StateType::MergeForkLeft);
   QCOMPARE(timelines[i].at(3).getType(), Graph::StateType::HeadRight);
   ++i;

   QVERIFY(timelines[i].count() == 4);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(2).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(3).getType(), Graph::StateType::Active);
   ++i;

   QVERIFY(timelines[i].count() == 5);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(2).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(3).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(4).getType(), Graph::StateType::Branch);
   ++i;

   QVERIFY(timelines[i].count() == 5);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::MergeForkLeft);
   QCOMPARE(timelines[i].at(2).getType(), Graph::StateType::Cross);
   QCOMPARE(timelines[i].at(3).getType(), Graph::StateType::Cross);
   QCOMPARE(timelines[i].at(4).getType(), Graph::StateType::TailRight);
   ++i;

   QVERIFY(timelines[i].count() == 4);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::Active);
   QCOMPARE(timelines[i].at(2).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(3).getType(), Graph::StateType::Inactive);
   ++i;

   QVERIFY(timelines[i].count() == 4);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::Active);
   QCOMPARE(timelines[i].at(2).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(3).getType(), Graph::StateType::Inactive);
   ++i;

   QVERIFY(timelines[i].count() == 4);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::Active);
   QCOMPARE(timelines[i].at(2).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(3).getType(), Graph::StateType::Inactive);
   ++i;

   QVERIFY(timelines[i].count() == 4);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::MergeForkLeft);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::Tail);
   QCOMPARE(timelines[i].at(2).getType(), Graph::StateType::Tail);
   QCOMPARE(timelines[i].at(3).getType(), Graph::StateType::TailRight);
   ++i;

   QVERIFY(timelines[i].count() == 1);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Initial);
   ++i;
}

void TestsGraphCache::test_buildTemporalLoomInfiniteColumns()
{
   QVector<Relationship> vector;
   vector.append(Relationship { "0000000000000000000000000000000000000000", { "3449920531c42369cf3520c5cd01893e44925cd2" } });
   vector.append(Relationship { "3449920531c42369cf3520c5cd01893e44925cd0", { "wololowololowololowololowololowololowolo" } });
   vector.append(Relationship { "3449920531c42369cf3520c5cd01893e44925cd1", { "wololowololowololowololowololowololowolo" } });
   vector.append(Relationship { "3449920531c42369cf3520c5cd01893e44925cd3", { "wololowololowololowololowololowololowolo" } });
   vector.append(Relationship { "3449920531c42369cf3520c5cd01893e44925cd4", { "wololowololowololowololowololowololowolo" } });
   vector.append(Relationship { "3449920531c42369cf3520c5cd01893e44925cd5", { "wololowololowololowololowololowololowolo" } });
   vector.append(Relationship { "3449920531c42369cf3520c5cd01893e44925cd6", { "wololowololowololowololowololowololowolo" } });
   vector.append(Relationship { "3449920531c42369cf3520c5cd01893e44925cd7", { "wololowololowololowololowololowololowolo" } });
   vector.append(Relationship { "3449920531c42369cf3520c5cd01893e44925cd8", { "wololowololowololowololowololowololowolo" } });
   vector.append(Relationship { "3449920531c42369cf3520c5cd01893e44925cd9", { "wololowololowololowololowololowololowolo" } });
   vector.append(Relationship { "3449920531c42369cf3520c5cd01893e44925cd2", { "wololowololowololowololowololowololowolo" } });
   vector.append(Relationship { "wololowololowololowololowololowololowolo", {} });

   Graph::TemporalLoom loom;
   QVector<Graph::Timeline> timelines;

   for (auto &v : vector)
      timelines += loom.createTimeline(v.sha, v.parents);

   QVERIFY(timelines.count() == 12);

   auto i = 0;

   QVERIFY(timelines[i].count() == 1);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Branch);
   ++i;

   QVERIFY(timelines[i].count() == 2);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::Branch);
   ++i;

   QVERIFY(timelines[i].count() == 3);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(2).getType(), Graph::StateType::Branch);
   ++i;

   QVERIFY(timelines[i].count() == 4);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(2).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(3).getType(), Graph::StateType::Branch);
   ++i;

   QVERIFY(timelines[i].count() == 5);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(2).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(3).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(4).getType(), Graph::StateType::Branch);
   ++i;

   QVERIFY(timelines[i].count() == 6);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(2).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(3).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(4).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(5).getType(), Graph::StateType::Branch);
   ++i;

   QVERIFY(timelines[i].count() == 7);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(2).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(3).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(4).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(5).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(6).getType(), Graph::StateType::Branch);
   ++i;

   QVERIFY(timelines[i].count() == 8);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(2).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(3).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(4).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(5).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(6).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(7).getType(), Graph::StateType::Branch);
   ++i;

   QVERIFY(timelines[i].count() == 9);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(2).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(3).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(4).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(5).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(6).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(7).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(8).getType(), Graph::StateType::Branch);
   ++i;

   QVERIFY(timelines[i].count() == 10);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(2).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(3).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(4).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(5).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(6).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(7).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(8).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(9).getType(), Graph::StateType::Branch);
   ++i;

   QVERIFY(timelines[i].count() == 10);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Active);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(2).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(3).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(4).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(5).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(6).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(7).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(8).getType(), Graph::StateType::Inactive);
   QCOMPARE(timelines[i].at(9).getType(), Graph::StateType::Inactive);
   ++i;

   QVERIFY(timelines[i].count() == 10);
   QCOMPARE(timelines[i].at(0).getType(), Graph::StateType::Initial);
   QCOMPARE(timelines[i].at(1).getType(), Graph::StateType::Tail);
   QCOMPARE(timelines[i].at(2).getType(), Graph::StateType::Tail);
   QCOMPARE(timelines[i].at(3).getType(), Graph::StateType::Tail);
   QCOMPARE(timelines[i].at(4).getType(), Graph::StateType::Tail);
   QCOMPARE(timelines[i].at(5).getType(), Graph::StateType::Tail);
   QCOMPARE(timelines[i].at(6).getType(), Graph::StateType::Tail);
   QCOMPARE(timelines[i].at(7).getType(), Graph::StateType::Tail);
   QCOMPARE(timelines[i].at(8).getType(), Graph::StateType::Tail);
   QCOMPARE(timelines[i].at(9).getType(), Graph::StateType::TailRight);
}
