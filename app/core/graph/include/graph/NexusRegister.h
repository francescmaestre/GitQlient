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

#include <QHash>
#include <QString>
#include <QVector>

class NexusRegister
{
public:
    NexusRegister() = default;
    void clear();
    int findNextSha(const QString& next, int pos) const;
    void setNextSha(int lane, const QString& sha);
    void append(const QString& sha);
    void removeLast();
    int count() const { return nextShaVec.count(); }
    const QString& at(int index) const { return nextShaVec.at(index); }

private:
    QVector<QString> nextShaVec;
    QHash<QString, QVector<int>> shaToLanes;
};
