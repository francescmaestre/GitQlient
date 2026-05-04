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
#include <QMap>

class GitBase;
class QVBoxLayout;
class QPushButton;
class MergeInfoWidget;
class QLineEdit;
class QTextEdit;
class FileDiffWidget;
class RevisionFiles;
class SacredTimeline;
class QListWidget;
class QListWidgetItem;
class QStackedWidget;

class MergeWidget : public QFrame
{
    Q_OBJECT

signals:
    void signalMergeFinished();

public:
    enum class ConflictReason
    {
        Merge,
        CherryPick,
        Pull
    };

    explicit MergeWidget(
        const QSharedPointer<SacredTimeline>& gitQlientCache, const QSharedPointer<GitBase>& git, QWidget* parent = nullptr);

    void configure(const RevisionFiles& files, ConflictReason reason);
    void configureForCherryPick(const RevisionFiles& files, const QStringList& pendingShas = QStringList());
    void configureForRebase();

private:
    QSharedPointer<SacredTimeline> mGitQlientCache;
    QSharedPointer<GitBase> mGit;
    QListWidget* mConflictFiles = nullptr;
    QListWidget* mMergedFiles = nullptr;
    QLineEdit* mCommitTitle = nullptr;
    QTextEdit* mDescription = nullptr;
    QPushButton* mMergeBtn = nullptr;
    QPushButton* mAbortBtn = nullptr;
    ConflictReason mReason = ConflictReason::Merge;
    QStackedWidget* mStacked = nullptr;
    FileDiffWidget* mFileDiff = nullptr;
    QStringList mPendingShas;

    void fillButtonFileList(const RevisionFiles& files);
    void changeDiffView(QListWidgetItem* item);
    void abort();
    bool checkMsg(QString& msg);
    void commit();
    void removeMergeComponents();
    void onConflictResolved(const QString& fileName);
    void cherryPickCommit();
};
