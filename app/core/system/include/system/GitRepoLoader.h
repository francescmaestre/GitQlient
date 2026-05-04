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

#include <GitExecResult.h>
#include <RevisionFiles.h>
#include <cache/Commit.h>

#include <QObject>
#include <QSharedPointer>
#include <QVector>

#include <functional>

class GitBase;
class SacredTimeline;
namespace Graph
{
    class TemporalLoom;
}
class GitTags;
class AGitProcess;
class GitRequestorProcess;

class GitRepoLoader : public QObject
{
    Q_OBJECT

signals:
    void loadingStarted();
    void loadingFinished(bool full);
    void loadingMessage(const QString& message);
    void loadingProgress(int done, int total);
    void cancelPending(QPrivateSignal);

public slots:
    void loadLogHistory();
    void loadReferences();
    void loadAll();

public:
    explicit GitRepoLoader(
        QSharedPointer<GitBase> gitBase,
        QSharedPointer<SacredTimeline> cache,
        const QSharedPointer<Graph::TemporalLoom>& loom,
        QObject* parent = nullptr);
    void cancelAll();
    void setShowAll(bool showAll = true) { mShowAll = showAll; }

private:
    bool mShowAll = true;
    bool mIsLoading = false;
    bool mRefreshReferences = true;
    std::atomic<int> mPendingSteps{0};
    QSharedPointer<GitBase> mGitBase;
    QSharedPointer<SacredTimeline> mCommitCache;
    QSharedPointer<Graph::TemporalLoom> mLoom;
    QSharedPointer<GitTags> mGitTags;
    AGitProcess* mLogRequestor = nullptr;
    GitRequestorProcess* mRefsRequestor = nullptr;

    void fetchReferences();
    void onReferencesReceived(QByteArray rawData);
    void fetchCommitLog();
    void onCommitLogReceived(QByteArray rawLog);
    void onCommitsParsed();
    QVector<Commit> parseUnsignedLog(QByteArray& log, const std::function<void(int)>& onProgress) const;
    QVector<Commit> parseSignedLog(QByteArray& log, const std::function<void(int)>& onProgress) const;
    void onLoadStepComplete();

protected:
    struct WipData
    {
        bool showSignature = false;
        QString wipParentSha;
        RevisionFiles wipFiles;
        QVector<QString> untrackedFiles;
    };

    /** Resolves the canonical working directory via git. Override in tests to skip the git call. */
    virtual bool resolveWorkingDirectory();

    /** Fetches WIP state and config flags needed during log parsing. Override in tests to return empty data. */
    virtual WipData fetchWipData() const;

    /** Factory hook: returns the process used to fetch the commit log. Override in tests to inject a fake. */
    virtual AGitProcess* createLogRequestor();
};
