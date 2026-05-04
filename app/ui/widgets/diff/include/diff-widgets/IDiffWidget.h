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

#include <QFrame>

class GitBase;
class SacredTimeline;

class IDiffWidget : public QFrame
{
    Q_OBJECT
signals:

public:
    explicit IDiffWidget(
        const QSharedPointer<GitBase>& git, QSharedPointer<SacredTimeline> cache, QWidget* parent = nullptr);

    virtual bool reload() = 0;
    virtual void updateFontSize() = 0;
    QString getCurrentSha() const { return mCurrentSha; }
    QString getPreviousSha() const { return mPreviousSha; }

protected:
    QSharedPointer<GitBase> mGit;
    QSharedPointer<SacredTimeline> mCache;
    QString mCurrentSha;
    QString mPreviousSha;
};
