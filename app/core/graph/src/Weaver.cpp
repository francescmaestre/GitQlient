#include <graph/Weaver.h>

#include <ranges>

namespace
{
    static const QString ZERO_SHA = QStringLiteral("0000000000000000000000000000000000000000");
}

namespace Graph
{

    Weaver::Weaver() { spliceStrand(StrandGlyph(GlyphType::Branch), ZERO_SHA, 0); }

    Timeline Weaver::createTimeline(const QString& sha, const QStringList& parents)
    {
        const auto isRoot = parents.isEmpty();
        const auto isMerge = parents.count() > 1;
        const auto isNexus = isNexusPoint(sha); // also repositions activeLane when sha moved lanes

        inscribeNode(sha, parents, isNexus, isMerge, isRoot);

        const auto snapshot = mTimeline;
        mNexusRegister.setNextSha(activeLane, isRoot ? QString() : parents.first());

        reweaveStrands(isNexus, isMerge);

        return snapshot;
    }

    void Weaver::inscribeNode(const QString& sha, const QStringList& parents, bool isNexus, bool isMerge, bool isRoot)
    {
        if (isNexus)
            weaveNexusPoint(sha);

        if (isMerge)
            weaveConvergence(parents);
        else if (isRoot)
            mTimeline.setType(activeLane, StrandGlyph(GlyphType::Initial));
    }

    void Weaver::reweaveStrands(bool isNexus, bool isMerge)
    {
        if (isMerge)
            resolveConvergence();
        if (isNexus)
            resolveNexusPoint();
        if (hasEmergentStrand())
            activateEmergentStrand();
    }

    bool Weaver::isNexusPoint(const QString& sha)
    {
        int pos = mNexusRegister.findNextSha(sha, 0);
        const auto isDiscontinuity = activeLane != pos;
        auto isNexus = pos == -1 ? false : mNexusRegister.findNextSha(sha, pos + 1) != -1;

        if (isDiscontinuity)
            realignActiveStrand(sha);

        return isNexus;
    }

    void Weaver::weaveNexusPoint(const QString& sha)
    {
        auto rangeEnd = 0;
        auto idx = 0;
        auto rangeStart = rangeEnd = idx = mNexusRegister.findNextSha(sha, 0);

        while (idx != -1)
        {
            rangeEnd = idx;
            mTimeline.setType(idx, StrandGlyph(GlyphType::Tail));
            idx = mNexusRegister.findNextSha(sha, idx + 1);
        }

        mTimeline.setType(activeLane, StrandGlyph(GlyphType::MergeFork));

        const auto startType = mTimeline.at(rangeStart).getType().type;
        if (startType == GlyphType::MergeFork || startType == GlyphType::Tail)
            mTimeline.setSide(rangeStart, GlyphSide::Left);

        const auto endType = mTimeline.at(rangeEnd).getType().type;
        if (endType == GlyphType::MergeFork || endType == GlyphType::Tail)
            mTimeline.setSide(rangeEnd, GlyphSide::Right);

        for (int i = rangeStart + 1; i < rangeEnd; ++i)
        {
            const auto glyphType = mTimeline.at(i).getType().type;
            if (glyphType == GlyphType::Inactive)
                mTimeline.setType(i, StrandGlyph(GlyphType::Cross));
            else if (glyphType == GlyphType::Empty)
                mTimeline.setType(i, StrandGlyph(GlyphType::CrossEmpty));
        }
    }

    void Weaver::weaveConvergence(const QStringList& parents)
    {
        const auto activeLaneGlyph = mTimeline.at(activeLane).getType();
        const auto wasMergeFork = activeLaneGlyph.type == GlyphType::MergeFork;
        const auto wasFork = wasMergeFork && activeLaneGlyph.side == GlyphSide::Center;
        const auto wasFork_L = wasMergeFork && activeLaneGlyph.side == GlyphSide::Left;
        const auto wasFork_R = wasMergeFork && activeLaneGlyph.side == GlyphSide::Right;
        auto startJoinWasACross = false;
        auto endJoinWasACross = false;

        mTimeline.setType(activeLane, StrandGlyph(GlyphType::MergeFork));

        auto rangeStart = activeLane;
        auto rangeEnd = activeLane;

        for (const auto& parent : parents | std::views::drop(1))
        {
            int idx = mNexusRegister.findNextSha(parent, 0);

            if (idx != -1)
            {
                if (idx > rangeEnd)
                {
                    rangeEnd = idx;
                    endJoinWasACross = mTimeline.at(idx).getType().type == GlyphType::Cross;
                }

                if (idx < rangeStart)
                {
                    rangeStart = idx;
                    startJoinWasACross = mTimeline.at(idx).getType().type == GlyphType::Cross;
                }

                mTimeline.setType(idx, StrandGlyph(GlyphType::Join));
            }
            else
                rangeEnd = spliceStrand(StrandGlyph(GlyphType::Head), parent, rangeEnd + 1);
        }

        const auto startGlyphType = mTimeline.at(rangeStart).getType().type;
        if ((startGlyphType == GlyphType::MergeFork && !wasFork && !wasFork_R)
            || (startGlyphType == GlyphType::Join && !startJoinWasACross) || startGlyphType == GlyphType::Head)
            mTimeline.setSide(rangeStart, GlyphSide::Left);

        if (rangeEnd != rangeStart)
        {
            const auto endGlyphType = mTimeline.at(rangeEnd).getType().type;
            if ((endGlyphType == GlyphType::MergeFork && !wasFork && !wasFork_L)
                || (endGlyphType == GlyphType::Join && !endJoinWasACross) || endGlyphType == GlyphType::Head)
                mTimeline.setSide(rangeEnd, GlyphSide::Right);
        }

        for (int i = rangeStart + 1; i < rangeEnd; i++)
        {
            const auto glyphType = mTimeline.at(i).getType().type;
            if (glyphType == GlyphType::Inactive)
                mTimeline.setType(i, StrandGlyph(GlyphType::Cross));
            else if (glyphType == GlyphType::Empty)
                mTimeline.setType(i, StrandGlyph(GlyphType::CrossEmpty));
            else if (glyphType == GlyphType::Tail)
                mTimeline.setType(i, StrandGlyph(GlyphType::Tail));
        }
    }

    void Weaver::realignActiveStrand(const QString& sha)
    {
        if (mTimeline.at(activeLane) == StrandGlyph(GlyphType::Initial))
            mTimeline.setType(activeLane, StrandGlyph(GlyphType::Empty));
        else
            mTimeline.setType(activeLane, StrandGlyph(GlyphType::Inactive));

        int idx = mNexusRegister.findNextSha(sha, 0);
        if (idx != -1)
            mTimeline.setType(idx, StrandGlyph(GlyphType::Active));
        else
            idx = spliceStrand(StrandGlyph(GlyphType::Branch), sha, activeLane);

        activeLane = idx;
    }

    void Weaver::resolveConvergence()
    {
        for (int i = 0; i < mTimeline.count(); ++i)
        {
            const auto& strand = mTimeline.at(i);
            if (strand.isHead() || strand.isJoin() || strand == StrandGlyph(GlyphType::Cross))
                mTimeline.setType(i, StrandGlyph(GlyphType::Inactive));
            else if (strand == StrandGlyph(GlyphType::CrossEmpty))
                mTimeline.setType(i, StrandGlyph(GlyphType::Empty));
            else if (strand.isMerge())
                mTimeline.setType(i, StrandGlyph(GlyphType::Active));
        }
    }

    void Weaver::resolveNexusPoint()
    {
        for (int i = 0; i < mTimeline.count(); ++i)
        {
            const auto& strand = mTimeline.at(i);
            if (strand == StrandGlyph(GlyphType::Cross))
                mTimeline.setType(i, StrandGlyph(GlyphType::Inactive));
            else if (strand.isTail() || strand == StrandGlyph(GlyphType::CrossEmpty))
                mTimeline.setType(i, StrandGlyph(GlyphType::Empty));

            if (strand.isMerge())
                mTimeline.setType(i, StrandGlyph(GlyphType::Active));
        }

        while (mTimeline.last() == StrandGlyph(GlyphType::Empty))
        {
            mTimeline.removeLast();
            mNexusRegister.removeLast();
        }
    }

    bool Weaver::hasEmergentStrand()
    {
        return mTimeline.count() > activeLane ? mTimeline.at(activeLane) == StrandGlyph(GlyphType::Branch) : false;
    }

    void Weaver::activateEmergentStrand() { mTimeline.setType(activeLane, StrandGlyph(GlyphType::Active)); }

    int Weaver::spliceStrand(StrandGlyph glyph, const QString& next, int pos)
    {
        if (pos < mTimeline.count())
        {
            pos = mTimeline.findEmpty(pos);
            if (pos != -1)
            {
                mTimeline.setType(pos, glyph);
                mNexusRegister.setNextSha(pos, next);
                return pos;
            }
        }

        mTimeline.append(glyph);
        mNexusRegister.append(next);
        return mTimeline.count() - 1;
    }
} // namespace Graph
