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

#include <graph/StrandGlyph.h>

namespace Graph
{
    class Strand
    {
    public:
        constexpr Strand() = default;
        constexpr Strand(StrandGlyph glyph)
            : mGlyph(glyph)
        {
        }
        constexpr Strand(GlyphType type, GlyphSide side = GlyphSide::Center)
            : mGlyph(type, side)
        {
        }

        constexpr bool operator==(const Strand& other) const { return mGlyph == other.mGlyph; }
        constexpr bool operator==(const StrandGlyph& glyph) const { return mGlyph == glyph; }

        constexpr bool isHead() const { return mGlyph.type == GlyphType::Head; }
        constexpr bool isTail() const { return mGlyph.type == GlyphType::Tail; }
        constexpr bool isJoin() const { return mGlyph.type == GlyphType::Join; }
        constexpr bool isMerge() const { return mGlyph.type == GlyphType::MergeFork; }

        constexpr bool isFreeLane() const
        {
            return mGlyph.type == GlyphType::Inactive || mGlyph.type == GlyphType::Cross
                || mGlyph.type == GlyphType::Join;
        }

        constexpr bool isActive() const
        {
            return mGlyph.type == GlyphType::Active || mGlyph.type == GlyphType::Initial
                || mGlyph.type == GlyphType::Branch || mGlyph.type == GlyphType::MergeFork;
        }

        constexpr StrandGlyph getType() const { return mGlyph; }
        constexpr void setType(StrandGlyph glyph) { mGlyph = glyph; }
        constexpr void setSide(GlyphSide side) { mGlyph.side = side; }

    private:
        StrandGlyph mGlyph;
    };
} // namespace Graph
