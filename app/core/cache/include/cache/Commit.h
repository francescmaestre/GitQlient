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

#include <QDateTime>
#include <QStringList>
#include <QVector>

#include <chrono>

#include <cache/References.h>

class Commit
{
public:
    enum class Field
    {
        SHA,
        PARENTS_SHA,
        COMMITTER,
        AUTHOR,
        DATE,
        SHORT_LOG,
        LONG_LOG
    };

    Commit() = default;
    ~Commit() = default;
    Commit(QByteArray commitData);

    Commit(QByteArray commitData, const QString& gpg, bool goodSignature);
    explicit Commit(
        const QString& sha, const QStringList& parents, std::chrono::seconds commitDate, const QString& log);

    bool operator==(const Commit& commit) const;

    bool isValid() const;
    bool contains(const QString& value) const;

    int parentsCount() const;
    QString firstParent() const;
    QStringList parents() const;
    void setParents(const QStringList& parents);
    bool isInWorkingBranch() const;

    void appendChild(Commit* commit) { mChilds.append(commit); }
    void removeChild(Commit* commit);
    bool hasChilds() const { return !mChilds.empty(); }
    QString getFirstChildSha() const;
    int getChildsCount() const { return mChilds.count(); }

    bool isSigned() const { return !gpgKey.isEmpty(); }
    bool verifiedSignature() const { return mGoodSignature && !gpgKey.isEmpty(); }

    uint pos = 0;
    QString sha;
    QString committer;
    QString author;
    std::chrono::seconds dateSinceEpoch;
    QString shortLog;
    QString longLog;
    QString gpgKey;

private:
    bool mGoodSignature = false;
    QStringList mParentsSha;
    QVector<Commit*> mChilds;

    friend class SacredTimeline;

    void parseDiff(QByteArray& data, qsizetype startingField);
};
