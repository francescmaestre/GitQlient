#pragma once

#include <qnamespace.h>

namespace GitQlient
{
    enum GitQlientBranchItemRole
    {
        IsCurrentBranchRole = Qt::UserRole,
        FullNameRole,
        LocalBranchRole,
        ShaRole,
        IsLeaf,
        IsRoot
    };
}
