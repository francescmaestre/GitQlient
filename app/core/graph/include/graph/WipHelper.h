#pragma once

#include <GitWip.h>
#include <QLogger>
#include <cache/GitCache.h>

#include <QSharedPointer>

namespace WipHelper
{
    inline bool update(const QSharedPointer<GitBase>& git, const QSharedPointer<GitCache> cache)
    {
        GitWip wip(git);

        const auto files = wip.getUntrackedFiles();
        cache->setUntrackedFilesList(std::move(files));

        if (const auto info = wip.getWipInfo(); info->second.isValid())
            return cache->updateWipCommit(info->first, info->second);

        return false;
    }
} // namespace WipHelper
