#pragma once

#include <QString>
#include <QStringList>

namespace Graph
{
    struct CommitEntry
    {
        QString sha;
        QStringList parents;
    };
} // namespace Graph
