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

class SacredTimeline;
class GitBase;
class QPushButton;
class QToolButton;
class QMenu;

class BranchesWidgetMinimal : public QFrame
{
    Q_OBJECT
signals:
    void showFullBranchesView();
    void commitSelected(const QString& sha);
    void stashSelected(const QString& stashId);

public:
    explicit BranchesWidgetMinimal(
        const QSharedPointer<SacredTimeline>& cache, const QSharedPointer<GitBase> git, QWidget* parent = nullptr);

    void configureLocalMenu(const QString& sha, const QString& branch);
    void configureRemoteMenu(const QString& sha, const QString& branch);
    void configureTagsMenu(const QString& sha, const QString& tag);
    void configureStashesMenu(const QString& stashId, const QString& name);
    void configureSubmodulesMenu(const QString& name);
    void clearActions();

private:
    QSharedPointer<GitBase> mGit;
    QSharedPointer<SacredTimeline> mCache;
    QPushButton* mBack = nullptr;
    QToolButton* mLocal = nullptr;
    QMenu* mLocalMenu = nullptr;
    QToolButton* mRemote = nullptr;
    QMenu* mRemoteMenu = nullptr;
    QToolButton* mTags = nullptr;
    QMenu* mTagsMenu = nullptr;
    QToolButton* mStashes = nullptr;
    QMenu* mStashesMenu = nullptr;
    QToolButton* mSubmodules = nullptr;
    QMenu* mSubmodulesMenu = nullptr;
    QMenu* mCurrentMenuShown = nullptr;

    bool eventFilter(QObject* obj, QEvent* event);
    void addActionToMenu(const QString& sha, const QString& name, QMenu* menu);
};
