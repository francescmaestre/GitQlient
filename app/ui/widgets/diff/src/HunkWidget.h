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

class FileDiffView;
class GitBase;
class SacredTimeline;
class QTemporaryFile;

class HunkWidget : public QFrame
{
    Q_OBJECT

signals:
    void hunkStaged();

public:
    explicit HunkWidget(
        QSharedPointer<GitBase> git,
        QSharedPointer<SacredTimeline> cache,
        const QString& fileName,
        const QString& header,
        const QString& hunk,
        bool isCached = false,
        bool isEditable = false,
        QWidget* parent = nullptr);

private:
    QSharedPointer<GitBase> mGit;
    QSharedPointer<SacredTimeline> mCache;
    QString mFileName;
    QString mHeader;
    QString mHunk;
    bool mIsCached = false;
    FileDiffView* mHunkView = nullptr;
    int mLineToDiscard{};

    QTemporaryFile* createPatchFile();
    void discardHunk();
    void stageHunk();
    void stageLine();
    void discardLine();
    void revertLine();
    void showContextMenu(const QPoint& pos);
};
