#include <graph/Timeline.h>

#include <algorithm>
#include <ranges>

namespace Graph
{
    int Timeline::findEmpty(int pos) const
    {
        const auto it = std::ranges::lower_bound(mEmptySlots, pos);
        return it == mEmptySlots.end() ? -1 : *it;
    }

    int Timeline::findType(StrandGlyph glyph, int pos) const
    {
        const auto begin = mStates.cbegin() + pos;
        const auto it = std::ranges::find_if(begin, mStates.cend(), [&glyph](const Strand& s) {
            return s == glyph;
        });
        return it == mStates.cend() ? -1 : static_cast<int>(std::ranges::distance(mStates.cbegin(), it));
    }

    void Timeline::setType(int lane, StrandGlyph glyph)
    {
        const bool wasEmpty = mStates[lane] == StrandGlyph(GlyphType::Empty);
        const bool isNowEmpty = glyph == StrandGlyph(GlyphType::Empty);
        mStates[lane].setType(glyph);
        if (wasEmpty && !isNowEmpty)
            mEmptySlots.erase(std::ranges::find(mEmptySlots, lane));
        else if (!wasEmpty && isNowEmpty)
        {
            const auto it = std::ranges::lower_bound(mEmptySlots, lane);
            mEmptySlots.insert(it, lane);
        }
    }

    void Timeline::setSide(int lane, GlyphSide side) { mStates[lane].setSide(side); }

    void Timeline::append(StrandGlyph glyph)
    {
        const int lane = mStates.size();
        mStates.append(Strand(glyph));
        if (glyph == StrandGlyph(GlyphType::Empty))
            mEmptySlots.push_back(lane);
    }

    void Timeline::removeLast()
    {
        if (mStates.last() == StrandGlyph(GlyphType::Empty))
            mEmptySlots.pop_back();
        mStates.pop_back();
    }
} // namespace Graph
