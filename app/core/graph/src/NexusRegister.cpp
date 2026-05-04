#include <graph/NexusRegister.h>

#include <algorithm>
#include <ranges>

void NexusRegister::clear()
{
    nextShaVec.clear();
    nextShaVec.squeeze();
    shaToLanes.clear();
    shaToLanes.squeeze();
}

int NexusRegister::findNextSha(const QString& next, int pos) const
{
    const auto it = shaToLanes.constFind(next);
    if (it == shaToLanes.cend())
        return -1;
    for (const int lane : *it)
    {
        if (lane >= pos)
            return lane;
    }
    return -1;
}

void NexusRegister::setNextSha(int lane, const QString& sha)
{
    const QString oldSha = nextShaVec[lane];
    if (!oldSha.isEmpty())
    {
        auto& oldLanes = shaToLanes[oldSha];
        oldLanes.removeOne(lane);
        if (oldLanes.isEmpty())
            shaToLanes.remove(oldSha);
    }

    nextShaVec[lane] = sha;

    if (!sha.isEmpty())
    {
        auto& lanes = shaToLanes[sha];
        const auto it = std::ranges::lower_bound(lanes, lane);
        lanes.insert(it, lane);
    }
}

void NexusRegister::append(const QString& sha)
{
    const int lane = nextShaVec.size();
    nextShaVec.append(sha);
    if (!sha.isEmpty())
        shaToLanes[sha].append(lane);
}

void NexusRegister::removeLast()
{
    const QString sha = nextShaVec.last();
    if (!sha.isEmpty())
    {
        auto& lanes = shaToLanes[sha];
        lanes.removeLast();
        if (lanes.isEmpty())
            shaToLanes.remove(sha);
    }
    nextShaVec.pop_back();
}
